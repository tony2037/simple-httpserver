#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

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


int main(int argc , char *argv[])

{
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
    char inputBuffer[256] = {0};
    char message[] = {"Hi,this is server.\n"};
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
    serverInfo.sin_port = htons(1234);
    bind(sockfd,(struct sockaddr *)&serverInfo,sizeof(serverInfo));
    listen(sockfd,5);

    while(1){
        forClientSockfd = accept(sockfd,(struct sockaddr *) &clientInfo, (socklen_t *) &addrlen);
        recv(forClientSockfd,inputBuffer,sizeof(inputBuffer),0);
        printf("Receive: %s", inputBuffer);
	send(forClientSockfd,message,sizeof(message),0);
        printf("Get:%s\n",inputBuffer);
    }
    return 0;
}
