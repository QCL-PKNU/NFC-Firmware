/*
 * KnfcSocket.c
 *
 *  Created on: Jun 17, 2013
 *      Author: youngsun
 */
 
#include "KnfcDalSocket.h"

#ifdef LINUX_SUPPORT

bool_t
KnfcSocketDataReady(KnfcHandle_t hnd) {
	return TRUE;
}

KnfcHandle_t
KnfcSocketOpen(void *pParam) {

	int c;
	int socket_desc;
	KnfcHandle_t hnd;
	struct sockaddr_in server, client;

	socket_desc = socket(AF_INET , SOCK_STREAM , 0);;
	
	if(socket_desc == -1) {
		perror("socket failed");
		return FALSE;
	}

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons( 8888 );

	//Bind
	if(bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
	{
		//print the error message
		perror("bind failed");
		return FALSE;
	}

	//Listen
	listen(socket_desc , 3);

	//Accept and incoming connection
	c = sizeof(struct sockaddr_in);

	//accept connection from an incoming client
	hnd = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
	
	if (hnd < 0)
	{
		perror("accept failed");
	}

	return hnd;
}

int32_t 
KnfcSocketRead(KnfcHandle_t hnd, void *pBuf, uint32_t nLength) {

	int32_t nRead;

	if(hnd < 0) {
		return -1;
	}

	nRead = recv(hnd, pBuf, nLength, 0);

	if(nRead == 0) {
		return -1;
	}

	return nRead;
}

int32_t
KnfcSocketWrite(KnfcHandle_t hnd, void *pBuf, uint32_t nLength) {

	int32_t nWrite;

	if(hnd < 0) {
		return -1;
	}
	
	nWrite = send(hnd, pBuf, nLength, MSG_NOSIGNAL);

	if(nWrite == 0) {
		return -1;
	}

	return nWrite;
}

void 
KnfcSocketClose(KnfcHandle_t hnd) {

	if(hnd < 0) {
		return;
	}

	close(hnd);
}
#endif
