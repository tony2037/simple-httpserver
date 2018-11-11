#include "client.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


int main(int argc, char *argv[])
{
	char *host = "localhost"; // target URL
	char *PORT_NUM = "80"; // port

	char request[0xfff], response[0xfff];
	char *requestLine = "Get / HTTP/1.1\r\n";
	char *headerFmt = "Host: %s\r\n";
	char *CRLF = "\r\n";

	int cfd; // Socket File Descriptor
	int gaiStatus; // getaddinfo status
	struct addrinfo hints;
	struct addrinfo *request; // point to getaddrinfo() result

	// Set up Header Buffer
	size_t bufferLen = strlen(headerFmt) + strlen(host) + 1;
	char *buffer = (char *)malloc(bufferLen);

	// Set up request message
	strcpy(request, requestLine);
	sprintf(buffer, headerFmt, host);
	strcat(request, buffer);
	strcat(request, CRLF);
	printf("%s", request);
	return 0;
}

