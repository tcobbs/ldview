#include "TCNetworkClient.h"
#include "mystring.h"

#ifdef WIN32
#ifndef ECONNREFUSED
#define ECONNREFUSED WSAECONNREFUSED
#endif

#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG

#else // WIN32
#if defined (_QT) || defined (__APPLE__)
#include <unistd.h>
#include <arpa/inet.h>
#endif // _QT || __APPLE__
#if !defined(__APPLE__) || !defined(TCP_NODELAY)
#include <netinet/tcp.h>
#endif // !__APPLE__ || !TCP_NODELAY

#endif // WIN32

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>


TCNetworkClient::TCNetworkClient(void)
		 :port(-1),
		  serverHost(copyString("localhost"))
{
}

TCNetworkClient::TCNetworkClient(int port)
		 :port(port),
		  serverHost(copyString("localhost"))
{
}

TCNetworkClient::TCNetworkClient(int port, char* serverHost)
		 :port(port),
		  serverHost(copyString(serverHost))
{
}

TCNetworkClient::~TCNetworkClient(void)
{
}

void TCNetworkClient::dealloc(void)
{
	delete[] serverHost;
	TCNetwork::dealloc();
}

void TCNetworkClient::setLinger(void)
{
	struct linger ling;

	ling.l_onoff = 1;
	ling.l_linger = 10;
	if (setsockopt(dataSocket, SOL_SOCKET, SO_LINGER, (const char*)&ling,
		 sizeof(ling)) < 0)
	{
		setErrorNumber(TCNCE_SET_LINGER);
//		printf("Error linger setting failed.\n");
	}
}

void TCNetworkClient::setTcpNoDelay(void)
{
#ifdef TCP_NODELAY
	int flag = 1;

	if (setsockopt(dataSocket, IPPROTO_TCP, TCP_NODELAY, (const char*)&flag,
		 sizeof(flag)) < 0)
	{
		setErrorNumber(TCNCE_SET_NODELAY);
	}
#endif // TCP_NODELAY
}

int TCNetworkClient::setupSocket(void)
{
	struct hostent* hostAddr;

	dataSocket = socket(AF_INET, SOCK_STREAM, 0);
	portOut.sin_family = AF_INET;
	portOut.sin_port = htons((unsigned short)port);
	hostAddr = gethostbyname(serverHost);
	if (hostAddr == NULL)
	{
		setErrorNumber(TCNCE_HOSTNAME_LOOKUP);
		return 0;
//		printf("Can't get address of %s.\n", serverHost);
//		exit(1);
	}
	memcpy(&portOut.sin_addr.s_addr, hostAddr->h_addr_list[0],
			 hostAddr->h_length);
	setLinger();
	return 1;
}

int TCNetworkClient::openConnection(void)
{
	if (port == -1)
	{
//		printf("Error: attempt to open connection before setting port.\n");
		setErrorNumber(TCNCE_NO_PORT);
		return 0;
	}
	closeConnection();
	if (!setupSocket())
	{
		return 0;
	}
	if (connect(dataSocket, (struct sockaddr*)&portOut, sizeof(portOut)) < 0)
	{
		if (WSAGetLastError() == ECONNREFUSED)
		{
			setErrorNumber(TCNCE_CONNECTION_REFUSED);
		}
		else
		{
			setErrorNumber(TCNCE_CONNECT);
		}
//		printf("Error connecting to server.\n");
		closesocket(dataSocket);
		return 0;
	}
	else
	{
		connected = 1;
		return 1;
	}
}

void TCNetworkClient::closeConnection(void)
{
//	printf("TCNetworkClient::closeConnection()\n");
	TCNetwork::closeConnection();
}

void TCNetworkClient::setServerHost(const char* hostname)
{
	delete[] serverHost;
	serverHost = copyString(hostname);
}

char* TCNetworkClient::getServerHost(void)
{
	return serverHost;
}

void TCNetworkClient::setErrorNumber(int value)
{
	errorNumber = value;
	switch (value)
	{
		case TCNCE_SET_LINGER:
			setErrorString("Error setting linger.", WSAGetLastError());
			break;
		case TCNCE_HOSTNAME_LOOKUP:
			setErrorString("Hostname lookup failure.");
			break;
		case TCNCE_NO_PORT:
			setErrorString("Attempt to open connection before setting port.");
			break;
		case TCNCE_CONNECT:
			setErrorString("Error connecting", WSAGetLastError());
			break;
		case TCNCE_CONNECTION_REFUSED:
			setErrorString("Error connecting:  Connection refused.");
			break;
		case TCNCE_SET_NODELAY:
			setErrorString("Error setting TCP no-delay.", WSAGetLastError());
			break;
		default:
			TCNetwork::setErrorNumber(value);
			break;
	}
}
