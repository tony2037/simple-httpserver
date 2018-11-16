#include "client.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <arpa/inet.h>


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


    //Send a message to server
    char message[] = {"Hi server"};
    char receiveMessage[100] = {};
    send(sockfd,message,sizeof(message),0);
    recv(sockfd,receiveMessage,sizeof(receiveMessage),0);

    printf("%s",receiveMessage);
    printf("close Socket\n");
    close(sockfd);
    return 0;
}

