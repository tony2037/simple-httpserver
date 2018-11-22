#include "client.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <pthread.h>

char *PORT, *HOST;


void *secondRequest(void *query){
    // Build up socket
    int sockfd = 0;
    sockfd = socket(AF_INET , SOCK_STREAM , 0);

    if (sockfd == -1){
        printf("Fail to create a socket.");
    }

    // socket connection

    struct sockaddr_in info;
    memset(&info, 0, sizeof(info));
    info.sin_family = PF_INET;

    // localhost test
    info.sin_addr.s_addr = inet_addr(HOST);
    info.sin_port = htons(atoi(PORT));


    int err = connect(sockfd,(struct sockaddr *)&info,sizeof(info));
    if(err==-1){
        printf("Connection error");
    }

    void *request = malloc(500);
    memset(request, 0, 500);
    strcat((char *)request, "GET ");
    strcat((char *)request, (char *)query);
    strcat((char *)request, " HTTP/1.1\r\nHOST:");
    strcat((char *)request, HOST);
    strcat((char *)request, ":");
    strcat((char *)request, PORT);
    strcat((char *)request, "\r\n\r\n");

    printf("reuest: %s", (char *)request);

    //Send a message to server
    char receiveMessage[1024] = {};
    send(sockfd, request, strlen(request),0);
    recv(sockfd,receiveMessage,sizeof(receiveMessage),0);

    printf("\nRecieve: \n%s",receiveMessage);
    return 0;
}

void parseFirstRequest(char *recvmsg){
    char *status, *contentType, *content, *query;
    char *secContent[10];
    pthread_t tids[10];
    size_t threadID = 0;
    // Initialize secContent
    for(size_t i = 0; i < 10; ++i){
        secContent[i] = malloc(128);
	memset(secContent[i], 0, 128);
    }
    // Parsing First Receivement
    status = strtok(recvmsg, "\r\n");
    contentType = strtok(NULL, "\r\n");
    content = strtok(NULL, "\r\n");
    content = strtok(NULL, "\r\n");
    printf("\nstatus: %s\ncontenType: %s\ncontent: %s\n", status, contentType, content);

    if(strcmp(contentType, "Content-Type: directory") != 0)
        return;

    // Parsing in secContent
    query = strtok(content, " ");
    strcpy(secContent[threadID], query+1);
    printf("\n thread[%d]: %s", (int)threadID, secContent[threadID]);
    while((query = strtok(NULL, " ")) != NULL){
	++threadID;
        strcpy(secContent[threadID], query+1);
        printf("\n thread[%d]: %s", (int)threadID, secContent[threadID]);
    }
    ++threadID;

   // Multi threads request
   for(size_t i = 0; i < threadID; ++i){
       pthread_create(&tids[i], NULL, secondRequest, secContent[i]);
       pthread_join(tids[i], NULL);
   }
}


int main(int argc , char *argv[])
{

    // Parse arguments
    size_t i;
    char query[128], host[20], port[20];
    for(i = 1; i < argc && argv[i][0] == *"-"; ++i){
	if(argv[i][1] == *"t"){
	    strcpy(query, argv[++i]);
	    printf("T: %s\n", query);
	    continue;
	}
	else if(argv[i][1] == *"h"){
	    strcpy(host, argv[++i]);
	    printf("H: %s\n", host);
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
    PORT = port;
    HOST = host;

    // Build up socket
    int sockfd = 0;
    sockfd = socket(AF_INET , SOCK_STREAM , 0);

    if (sockfd == -1){
        printf("Fail to create a socket.");
    }

    // socket connection

    struct sockaddr_in info;
    memset(&info, 0, sizeof(info));
    info.sin_family = PF_INET;

    // localhost test
    info.sin_addr.s_addr = inet_addr(host);
    info.sin_port = htons(atoi(port));


    int err = connect(sockfd,(struct sockaddr *)&info,sizeof(info));
    if(err==-1){
        printf("Connection error");
    }

    // Build up request
    void *request = malloc(500);
    memset(request, 0, 500);
    strcat((char *)request, "GET ");
    strcat((char *)request, query);
    strcat((char *)request, " HTTP/1.1\r\nHOST:");
    strcat((char *)request, host);
    strcat((char *)request, ":");
    strcat((char *)request, port);
    strcat((char *)request, "\r\n\r\n");

    printf("reuest: %s", (char *)request);

    //Send a message to server
    char receiveMessage[1024] = {};
    send(sockfd, request, strlen(request),0);
    recv(sockfd,receiveMessage,sizeof(receiveMessage),0);

    printf("Recieve: \n%s",receiveMessage);
    //if(close(sockfd) == 0)
        //printf("\nclose Socket\n");
    parseFirstRequest(receiveMessage);
    return 0;
}

