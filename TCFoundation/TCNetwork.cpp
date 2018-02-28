#include "TCNetwork.h"
#include "mystring.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>


#ifdef WIN32

#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG

TCNetwork::TCNetworkSetupCleanup TCNetwork::networkSetupCleanup;

TCNetwork::TCNetworkSetupCleanup::TCNetworkSetupCleanup(void)
{
	WSADATA wsaData;
	WORD requestedVersion = 0x0002;	// 2.0

	if (WSAStartup(requestedVersion, &wsaData) != 0)
	{
		debugPrintf("Error initializing WinSock 2.0!!!\n");
	}
	if (wsaData.wVersion != requestedVersion)
	{
		debugPrintf("Wrong WinSock version!!!\n");
	}
}

TCNetwork::TCNetworkSetupCleanup::~TCNetworkSetupCleanup(void)
{
	WSACleanup();
}
#endif // WIN32

TCNetwork::TCNetwork(void)
	:connected(0),
	 errorNumber(0),
	 errorString(NULL)
{
}

TCNetwork::~TCNetwork(void)
{
}

void TCNetwork::dealloc(void)
{
//	printf("TCNetwork::dealloc\n");
	delete[] errorString;
	closeConnection();
	TCObject::dealloc();
}

TCByte* TCNetwork::getData(int& length)
{
	TCByte* data;
	int n;

	if ((unsigned)socketRecv(dataSocket, (char *)&length, sizeof(length), 0) <
		sizeof(length))
	{
		setErrorNumber(TCNE_READ_PACKET_SIZE);
		closeConnection();
//		cleanShutdown();
	}
	if (length > MAX_PACKET_SIZE)
	{
//		printf("Bogus data received.\n");
		setErrorNumber(TCNE_PACKET_SIZE);
		closeConnection();
//		cleanShutdown();
	}
	data = new TCByte[length];
	if ((n = (int)socketRecv(dataSocket, (char *)data, length, 0)) < length)
	{
//		printf("Packet error.\n");
		setErrorNumber(TCNE_READ);
	}
	if (socketSend(dataSocket, (char *)&n, sizeof(n), 0) != sizeof(n))
	{
		setErrorNumber(TCNE_WRITE);
	}
	return data;
}

char* TCNetwork::getString(void)
{
	int length;

	return (char*)getData(length);
}

void TCNetwork::sendData(int length, const void* data)
{
	int ack;

	if (!connected)
	{
//		printf("Error:  Write attempt before opening connection.\n");
		setErrorNumber(TCNE_WRITE_BEFORE_CONNECT);
		return;
	}
	if (socketSend(dataSocket, (char *)&length, sizeof(length), 0) !=
		sizeof(length))
	{
		setErrorNumber(TCNE_WRITE);
		closeConnection();
		return;
	}
	if (socketSend(dataSocket, (char *)data, length, 0) != length)
	{
		setErrorNumber(TCNE_WRITE);
		closeConnection();
		return;
	}
	if ((unsigned)socketRecv(dataSocket, (char *)&ack, sizeof(ack), 0) <
		sizeof(ack))
	{
//		printf("Error: write to socket failed.\n");
		setErrorNumber(TCNE_WRITE);
		closeConnection();
	}
	else if (ack < length)
	{
//		printf("Error: write to socket failed.\n");
		setErrorNumber(TCNE_WRITE);
		closeConnection();
	}
}

void TCNetwork::sendString(const char* str)
{
	sendData((int)strlen(str)+1, str);
}

void TCNetwork::sendPrintf(const char* fmt, ...)
{
	va_list argPtr;
	char buf[1024];

	va_start(argPtr, fmt);
	vsprintf(buf, fmt, argPtr);
	va_end(argPtr);
	sendString(buf);
}

void TCNetwork::cleanShutdown(void)
{
	closeConnection();
	exit(0);
}

void TCNetwork::closeConnection(void)
{
//	printf("TCNetwork::closeConnection()\n");
	if (connected)
	{
//		printf("closing socket\n");
		closesocket(dataSocket);
	}
	connected = 0;
}

int TCNetwork::getInt(void)
{
	TCByte* data;
	int length;
	int retVal;

	data = getData(length);
	memcpy(&retVal, data, sizeof(retVal));
	delete[] data;
	return retVal;
}

float TCNetwork::getFloat(void)
{
	TCByte* data;
	int length;
	float retVal;

	data = getData(length);
	memcpy(&retVal, data, sizeof(retVal));
	delete[] data;
	return retVal;
}

void TCNetwork::setErrorString(const char* value, int sysErrno)
{
	delete[] errorString;
	if (value)
	{
		if (sysErrno)
		{
			char* sysErrorString = strerror(sysErrno);

			errorString = new char[strlen(value) + strlen(sysErrorString) + 5];
			sprintf(errorString, "%s: %s.", value, sysErrorString);
		}
		else
		{
			errorString = copyString(value);
		}
	}
	else
	{
		errorString = copyString(value);
	}
}

void TCNetwork::setErrorNumber(int value)
{
	errorNumber = value;
	switch (value)
	{
		case 0:
			setErrorString(NULL);
			break;
		case TCNE_READ_PACKET_SIZE:
			setErrorString("Error reading packet size.");
			break;
		case TCNE_PACKET_SIZE:
			setErrorString("Received packet size too big.");
			break;
		case TCNE_READ:
			setErrorString("Error reading data.");
			break;
		case TCNE_WRITE_BEFORE_CONNECT:
			setErrorString("Write attempt before connection opened.");
			break;
		case TCNE_WRITE:
			setErrorString("Error writing data.");
			break;
		default:
			errorNumber = -1;
			setErrorString("Unknown Error.");
	}
}

void TCNetwork::clearError(void)
{
	setErrorNumber(0);
}
