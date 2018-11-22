#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <glob.h>

struct {
    char *ext;
    char *filetype;
} extensions [] = {
    {"htm", "text/html" },
    {"html","text/html" },
    {"css","text/css" },
    {"h","text/x-h" },
    {"hh","text/x-h" },
    {"c","text/x-c" },
    {"cc","text/x-c" },
    {"json","application/json" },
    {0,0} };

enum {
    OK = 0,
    BAD_REQUEST,
    NOT_FOUND,
    METHOD_NOT_ALLOWED,
    UNSUPPORT_MEDIA_TYPE
};

const int status_code[] = {
    200, // OK
    400, // Bad Request
    404, // Not Found
    405, // Method Not Allowed
    415, // Unsupported Media Type
};

const char *status_info[] = {
	"OK",
	"BAD_REQUEST",
	"NOT_FOUND",
	"METHOD_NOT_ALLOWED",
	"UNSUPPORTED_MEDIA_TYPE",
};

void glob_dir(char *dir, char *glob_tmp){
    glob_t buf;
    size_t i;
    if(dir[strlen(dir) -1] != *"/")
        strcat(dir, "/");
    strcat(dir, "*");
    glob(dir, GLOB_NOSORT, NULL, &buf);
    for(i=0; i < buf.gl_pathc; ++i){
        printf("buf.gl_pathv[%d]= %s \n", (int)i, (buf.gl_pathv[i]));
	strcat(glob_tmp, buf.gl_pathv[i]);
	strcat(glob_tmp, " ");
    }
    globfree(&buf);
    return ;
}

void responseFormat(char *response, char *Method, char *Query, int fd){
    char *queryParse[5];
    size_t i = 0;
    char fullQuery[128] = {0};
    char queryLast[32] = {0};
    memcpy(fullQuery, Query, strlen(Query));
    memset((void *) response, 0, 256); // Initialize
    if(*Method != *"GET"){
        // 405 Method Not Allowed
        fprintf(stderr, "405\n");
	strcat(response, "HTTP/1.x 405 METHOD_NOT_ALLOWED\r\nContent-Type: \r\nServer: httpserver/1.x\r\n\r\n");
        send(fd, (void *)response, strlen(response), 0);
	close(fd);
	return;
    }

    if(Query[0] != *"/"){
        // 400 Bad Request
	fprintf(stderr, "400\n");
	strcat(response, "HTTP/1.x 400 BAD_REQUEST\r\nContent-Type: \r\nServer: httpserver/1.x\r\n\r\n");
	fprintf(stderr, "\nresponse:\n %s\n", response);
        send(fd, (void *)response, strlen(response), 0);
        close(fd);
	return;
    }

    // Parsing Query
    queryParse[i++] = strtok(Query, "/");
    fprintf(stderr, "Parsing Query %d: %s", (int)i, queryParse[i-1]);
    while(queryParse[i-1] != NULL){
      queryParse[i++] = strtok(NULL, "/");    
      fprintf(stderr, "Parsing Query %d: %s\n", (int)i, queryParse[i-1]);
      if(queryParse[i-1] != NULL){
          memset(queryLast, 0, 32);
          memcpy(queryLast, queryParse[i-1], strlen(queryParse[i-1]));
      }
    }
    --i; // Current i
    fprintf(stderr, "\nCurrent i: %d\n", (int)i);

    if(i == 0){
        // root
	char glob_result[1024] = {0};
	char dir[32] = {0};
	strcat(dir, "./testdic/");
        glob_dir(dir, glob_result);
        fprintf(stderr, "glob_result: %s\n", glob_result);	
	strcat(response, "HTTP/1.x 200 OK\r\nContent-Type: directory\r\nServer: httpserver/1.x\r\n\r\n");
	strcat(response, glob_result);
        send(fd, (void *)response, strlen(response), 0);
	close(fd);
	return;
    }
    else if(i > 0){
        // several directory
        fprintf(stderr, "Query i>0 :%s\n", fullQuery);
	// File type
	char *ft;
	ft = strtok(queryLast, ".");
	ft = strtok(NULL, ".");
	fprintf(stderr, "\nfile_type: %s\n", ft);
	if(ft == NULL){
	    // Directory
	    fprintf(stderr, "Directory\n");
	    char dicPath[32] = {0};
	    char globResult[1024] = {0};
	    strcat(dicPath, ".");
            strcat(dicPath, fullQuery);
	    fprintf(stderr, "dicPath: %s\n", dicPath);
	    glob_dir(dicPath, globResult);
	    strcat(response, "HTTP/1.x 200 OK\r\nContent-Type: directory");
	    strcat(response, "\r\nServer: httpserver/1.x\r\n\r\n");
            strcat(response, globResult);
            fprintf(stderr, "%s", response);
            send(fd, (void *)response, strlen(response), 0);
	    close(fd);
	    return; 
	}
	else{
            // file
	    int support = 0;
	    size_t j = 7;
	    size_t exti = 0;
	    while(j){
	        if(!strcmp(extensions[j].ext, ft)){
	            support = 1;
		    exti = j;
                }
		--j;
	    }

	    // support
	    if(support){
		fprintf(stderr, "File Support\n");
	        FILE *file;
                char filePath[128] = {0};
		char chunk[1024] = {0};
		strcat(filePath, ".");
		strcat(filePath, fullQuery);
                file = fopen(filePath, "r");
	        if(file){
		    // Read successful
                    if(!fread(chunk, 1, 1024, file)){
		        // Read fault 
	                strcat(response, "HTTP/1.x 404 NOT_FOUND\r\nContent-Type: \r\nServer: httpserver/1.x\r\n\r\n");
                        send(fd, (void *)response, strlen(response), 0);
			close(fd);
			return;
		    }
	            strcat(response, "HTTP/1.x 200 OK\r\nContent-Type: ");
		    strcat(response, extensions[exti].filetype);
		    strcat(response, "\r\nServer: httpserver/1.x\r\n\r\n");
		    strcat(response, chunk);
		    fprintf(stderr, "%s", response);
                    send(fd, (void *)response, strlen(response), 0);
		    close(fd);
		    return;
		}
	        else{
		    // No such file
	            strcat(response, "HTTP/1.x 404 NOT_FOUND\r\nContent-Type: \r\nServer: httpserver/1.x\r\n\r\n");
                    send(fd, (void *)response, strlen(response), 0);
		    close(fd);
		    return;
		}	
	    }
	    else{
		fprintf(stderr, "File not support\n");
	        strcat(response, "HTTP/1.x 415 UNSUPPORT_MEDIA_TYPE\r\nContent-Type: \r\nServer: httpserver/1.x\r\n\r\n");
                send(fd, (void *)response, strlen(response), 0);
		close(fd);
		return;
	    }

	}
	//fprintf(stderr, "\nfiletype: %s\n", ft);

        send(fd, (void *)response, strlen(response), 0);
        close(fd);
	return;
    }

    // Response
    send(fd, (void *)response, strlen(response), 0);
    close(fd);
    return;
}

void handle_socket(int fd){
    char buffer[256] = {0};
    char response[256] = {0};
    char *Method, *Query;
    //char Method[8] = {0};
    //char Query[64] = {0};
    recv(fd, (void *)buffer, sizeof(buffer), 0);
    strcat(response, buffer);
    printf("Receive: \n%s", buffer);
    // Parse
    Method = strtok(buffer, " ");
    Query = strtok(NULL, " ");
    printf("Method: %s\nQuery: %s\n", Method, Query);
    responseFormat(response, Method, Query, fd);
}

int main(int argc , char *argv[])

{
    /*/ cd /testdir
    if(chdir("testdir") == -1){
        printf("chdir error\n");
	exit(3);
    }*/

    // Parse arguments
    size_t i;
    char root[128], thread_number[20], port[20];
    for(i = 1; i < argc && argv[i][0] == *"-"; ++i){
        if(argv[i][1] == *"r"){
            strcpy(root, argv[++i]);
            printf("R: %s\n", root);
            continue;
        }
        else if(argv[i][1] == *"n"){
            strcpy(thread_number, argv[++i]);
            printf("N: %s\n", thread_number);
            continue;
        }
        else if(argv[i][1] == *"p"){
            strcpy(port, argv[++i]);
            printf("P: %s\n", port);
            continue;
        }
        else{
            printf("No such option.");
        }
    }


    // Build up socket
    int sockfd = 0,forClientSockfd = 0;
    sockfd = socket(AF_INET , SOCK_STREAM , 0);

    if (sockfd == -1){
        printf("Fail to create a socket.");
    }

    // The connection of socket
    struct sockaddr_in serverInfo,clientInfo;
    int addrlen = sizeof(clientInfo);
    memset(&serverInfo, 0, sizeof(serverInfo));

    serverInfo.sin_family = PF_INET;
    serverInfo.sin_addr.s_addr = INADDR_ANY;
    serverInfo.sin_port = htons(atoi(port));
    bind(sockfd,(struct sockaddr *)&serverInfo,sizeof(serverInfo));
    listen(sockfd, atoi(thread_number));

    while(1){
        forClientSockfd = accept(sockfd,(struct sockaddr *) &clientInfo, (socklen_t *) &addrlen);
	/*
        recv(forClientSockfd,inputBuffer,sizeof(inputBuffer),0);
        printf("Receive: \n%s", inputBuffer);
	send(forClientSockfd,message,sizeof(message),0);
	*/
	handle_socket(forClientSockfd);
    }
    return 0;
}
