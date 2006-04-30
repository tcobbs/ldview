#include "TCWebClient.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <limits.h>
#include <signal.h>

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/xtime.hpp>

#ifdef WIN32
//#define sleep(sec) Sleep((sec) * 1000)
#define timezone _timezone
#else // WIN32
#ifdef _QT
#define send(socket, buf, len, flags) write((socket), (buf), (len))
#define recv(socket, buf, len, flags) read((socket), (buf), (len))
#define WSAGetLastError() errno
#endif // _QT
#endif // WIN32

#include <TCFoundation/mystring.h>

//#define BUFFER_SIZE 1
#define BUFFER_SIZE 1024

static const char __pBase64[]="\
ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static char monthLongNames[12][20] =
{
	"January",
	"February",
	"March",
	"April",
	"May",
	"June",
	"July",
	"August",
	"September",
	"October",
	"November",
	"December"
};

static char monthShortNames[12][4] =
{
	"Jan",
	"Feb",
	"Mar",
	"Apr",
	"May",
	"Jun",
	"Jul",
	"Aug",
	"Sep",
	"Oct",
	"Nov",
	"Dec"
};

/*
static char dayLongNames[7][20] =
{
	"Sunday",
	"Monday",
	"Tuesday",
	"Wednesday",
	"Thursday",
	"Friday",
	"Saturday"
};

static char dayShortNames[7][4] =
{
	"Sun",
	"Mon",
	"Tue",
	"Wed",
	"Thu",
	"Fri",
	"Sat"
};
*/

void do_sleep(int sec)
{
	boost::xtime xt;

	boost::xtime_get(&xt, boost::TIME_UTC);
	xt.sec += sec;
	boost::thread::sleep(xt);
}

TCWebClient::TCWebClient(const char* url)
	:TCNetworkClient(80),
	 socketTimeout(30),
	 webServer(NULL),
	 serverPath(NULL),
	 url(copyString(url)),
	 referer(NULL),
	 filename(NULL),
	 outputDirectory(NULL),
	 dataFile(NULL),
	 dataFilePath(NULL),
	 contentType(NULL),
	 locationField(NULL),
	 chunked(false),
	 bufferLength(0),
	 readBuffer(NULL),
	 readBufferPosition(NULL),
	 maxRetries(3),
	 pageLength(0),
	 headerFetched(0),
	 serverTime(0),
	 lastModifiedTime(0),
	 pageData(NULL),
	 username(NULL),
	 password(NULL),
	 authorizationString(NULL),
	 bytesRead(0),
	 fetchThread(NULL),
	 mutex(new boost::mutex),
	 totalBytesRead(ZERO64),
	 doneFetching(0),
	 owner(NULL),
	 finishURLMemberFunction(NULL),
	 finishHeaderMemberFunction(NULL),
	 socketSetup(0),
	 retryCount(0),
	 maxRetryCount(10),
	 aborted(false)
{
	parseURL();
}

TCWebClient::~TCWebClient(void)
{
}

void TCWebClient::dealloc(void)
{
//	debugPrintf("TCWebClient::dealloc()\n");
	delete webServer;
	delete serverPath;
	delete url;
	delete referer;
	delete filename;
	delete outputDirectory;
	if (dataFile)
	{
		fclose(dataFile);
	}
	delete dataFilePath;
	delete contentType;
	delete locationField;
	delete readBuffer;
	// Do NOT delete readBufferPosition
	delete pageData;
	delete username;
	delete password;
	delete authorizationString;
	if (fetchThread)
	{
//		fetchThread->release();
//		fetchThread->join();
		delete fetchThread;
		fetchThread = NULL;
	}
	delete mutex;
	// Do NOT delete owner
	TCNetworkClient::dealloc();
}

void TCWebClient::abort(void)
{
	boost::mutex::scoped_lock lock(*mutex);

	aborted = true;
}

bool TCWebClient::getAborted(void)
{
	boost::mutex::scoped_lock lock(*mutex);

	return aborted;
}

int TCWebClient::openConnection(void)
{
	if (TCNetworkClient::openConnection())
	{
		if (setNonBlock())
		{
			return 1;
		}
	}
	return 0;
}

/*
int TCWebClient::openConnection(void)
{
	int result = 0;

	if (socketSetup && !connected)
	{
		if (connect(dataSocket, (struct sockaddr*)&portOut, sizeof(portOut)) < 0)
		{
			setErrorNumber(TCNCE_CONNECT);
			close(dataSocket);
			socketSetup = 0;
		}
		else
		{
			connected = 1;
			result = 1;
		}
	}
	else
	{
		result = TCNetworkClient::openConnection();
	}
	if (result)
	{
		result = setNonBlock();
	}
	return result;
}
*/

int TCWebClient::parseURL(void)
{
	char* spot = strstr(url, "://");
	char* hostSpot;

	delete webServer;
	webServer = NULL;
	delete serverPath;
	serverPath = NULL;
	if (spot)
	{
		hostSpot = spot + 3;
	}
	else
	{
		hostSpot = url;
	}
	if ((spot = strchr(hostSpot, '/')) != NULL)
	{
		int length = spot - hostSpot;

		webServer = new char[length + 1];
		strncpy(webServer, hostSpot, length);
		webServer[length] = 0;
		length = strlen(spot);
		serverPath = new char[length + 1];
		strcpy(serverPath, spot);
	}
	else
	{
		int length = strlen(hostSpot);

		webServer = new char[length + 1];
		strcpy(webServer, hostSpot);
		serverPath = new char[2];
		strcpy(serverPath, "/");
	}
	setServerHost(webServer);
	return 1;
}

void TCWebClient::setLocationField(const char* value)
{
	if (value != locationField)
	{
		const char *fieldEnd = strstr(value, "\r\n");

		delete locationField;
		if (fieldEnd)
		{
			int len = fieldEnd - value;

			locationField = new char[len + 1];
			strncpy(locationField, value, len);
			locationField[len] = 0;
		}
		else
		{
			locationField = copyString(value);
		}
	}
}

void TCWebClient::setContentType(const char* value)
{
	const char* crSpot = strchr(value, '\r');
	const char* lfSpot = strchr(value, '\n');
	const char* endSpot = 0;
	int length = -1;

	if (crSpot && lfSpot)
	{
		if (crSpot < lfSpot)
		{
			endSpot = crSpot;
		}
		else
		{
			endSpot = lfSpot;
		}
	}
	else if (crSpot)
	{
		endSpot = crSpot;
	}
	else if (lfSpot)
	{
		endSpot = lfSpot;
	}
	else
	{
		length = strlen(value);
	}
	delete contentType;
	if (length == -1)
	{
		length = endSpot - value;
	}
	contentType = new char[length + 1];
	strncpy(contentType, value, length);
	contentType[length] = 0;
}

time_t TCWebClient::scanDateString(const char* dateString)
{
	struct tm tmTime;
	struct tm tmLocalTime;
	struct tm tmGmtTime;
	time_t result = 0;
	const char *commaSpot = strchr(dateString, ',');
//	char dayOfWeek[128];
	bool isRfc822Time = false;
	bool isRfc850Time = false;
	bool isAscTime = false;
	bool failed = true;
	char month[128];

	if (*commaSpot)
	{
		if (commaSpot - dateString == 3)
		{
			isRfc822Time = true;
		}
		else
		{
			isRfc850Time = true;
		}
	}
	else
	{
		isAscTime = true;
	}
	memset(&tmTime, 0, sizeof(tmTime));
	if (isRfc822Time)
	{
		if (sscanf(dateString, "%*s %d %s %d %d:%d:%d ", &tmTime.tm_mday, month,
			&tmTime.tm_year, &tmTime.tm_hour, &tmTime.tm_min, &tmTime.tm_sec)
			== 6)
		{
			tmTime.tm_year -= 1900;
			failed = false;
		}
	}
	else if (isRfc850Time)
	{
		if (sscanf(dateString, "%*s %d-%3s-%d %d:%d:%d ", &tmTime.tm_mday,
			month, &tmTime.tm_year, &tmTime.tm_hour, &tmTime.tm_min,
			&tmTime.tm_sec) == 6)
		{
			// Year is already in 2-digit format.
			failed = false;
		}
	}
	else if (isAscTime)
	{
		if (sscanf(dateString, "%*s %s %d %d:%d:%d %d", month, &tmTime.tm_mday,
			&tmTime.tm_hour, &tmTime.tm_min, &tmTime.tm_sec, &tmTime.tm_year)
			== 6)
		{
			tmTime.tm_year -= 1900;
			failed = false;
		}
	}
	if (!failed)
	{
		int i;

		for (i = 0; i < 12; i++)
		{
			if (strcasecmp(month, monthShortNames[i]) == 0)
			{
				break;
			}
		}
		if (i == 12)
		{
			// The month name is supposed to be always a 3-letter abbreviation,
			// but go ahead and check the full name if we don't find a match.
			for (i = 0; i < 12; i++)
			{
				if (strcasecmp(month, monthLongNames[i]) == 0)
				{
					break;
				}
			}
		}
		if (i == 12)
		{
			// Not a good solution, but oh well.
			i = 0;
		}
		tmTime.tm_mon = i;
		tmTime.tm_isdst = 0;	// All HTTP times are GMT, so no DST
		// In addition to returning a time_t, the following normalizes the
		// input tmTime and fills in tm_wday and tm_yday.  (Notice that we
		// ignored those above.)
		result = mktime(&tmTime);
		// The above is still in GMT.  Convert it to the local time zone.
		tmLocalTime = *localtime(&result);
		tmGmtTime = *gmtime(&result);
		// Convert it back to a time_t:
		result += (mktime(&tmLocalTime) - mktime(&tmGmtTime));
	}
	return result;
}

bool TCWebClient::receiveHeader(void)
{
	bool retValue = false;
	bool done = false;

	while (!done && !getAborted())
	{
		if (waitForRead())
		{
			char buf[10241];
			int receiveLength;

			receiveLength = recv(dataSocket, buf, sizeof(buf) - 1, 0);
			if (receiveLength == SOCKET_ERROR)
			{
#ifdef WIN32
				int errorNumber = WSAGetLastError();
				char errorBuf[1024];

				FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, errorNumber, 0,
					errorBuf, sizeof(errorBuf), NULL);
				debugPrintf("Socket error: %s\n", errorBuf);
#endif
			}
			else if (receiveLength > 0)
			{
				char *tmpBuffer = new char[receiveLength + bufferLength];

				if (readBuffer)
				{
					memcpy(tmpBuffer, readBuffer, bufferLength);
					clearReadBuffer();
				}
				readBuffer = tmpBuffer;
				memcpy(readBuffer + bufferLength, buf, receiveLength);
				bufferLength += receiveLength;
				if (strstr(buf, "\r\n\r\n") ||
					strnstr(readBuffer, "\r\n\r\n", bufferLength))
				{
					retValue = true;
					done = true;
				}
			}
			else
			{
				done = true;
			}
		}
		else
		{
			done = true;
		}
	}
	readBufferPosition = readBuffer;
	return retValue;
}

void TCWebClient::parseHeaderFields(int headerLength)
{
	char *fieldData;

	fieldData = strncasestr(readBuffer, "\r\nContent-Length: ",
		headerLength);
	if (fieldData)
	{
		if (sscanf(fieldData, "\r\nContent-Length: %d", &pageLength) != 1)
		{
			pageLength = 0;
		}
	}
	fieldData = strncasestr(readBuffer, "\r\nContent-Type: ", headerLength);
	if (fieldData)
	{
		setContentType(fieldData + 16);
	}
	fieldData = strncasestr(readBuffer, "\r\nDate: ", headerLength);
	if (fieldData)
	{
		serverTime = scanDateString(fieldData + 8);
		if (serverTime)
		{
			time_t currentTime;

			currentTime = time(NULL);
			serverTimeDelta = serverTime - currentTime;
		}
	}
	fieldData = strncasestr(readBuffer, "\r\nLast-Modified: ",
		headerLength);
	if (fieldData)
	{
		lastModifiedTime = scanDateString(fieldData + 17);
	}
	fieldData = strncasestr(readBuffer, "\r\nTransfer-Encoding: ", headerLength);
	if (fieldData)
	{
		if (stringHasCaseInsensitivePrefix(fieldData + 21, "chunked"))
		{
			chunked = true;
		}
	}
	fieldData = strncasestr(readBuffer, "\r\nLocation: ", headerLength);
	if (fieldData)
	{
		setLocationField(fieldData + 12);
	}
}

bool TCWebClient::parseHeader(void)
{
	char *fieldData;
	bool retValue = false;
	int resultCode;
	int headerLength;

	fieldData = strnstr(readBuffer, "\r\n\r\n", bufferLength);
	if (fieldData)
	{
		headerLength = fieldData - readBuffer + 4;
	}
	else
	{
		return 0;
	}
	if (sscanf(readBuffer, "%*s %d", &resultCode) != 1)
	{
		return 0;
	}
	parseHeaderFields(headerLength);
	if (headerLength < bufferLength)
	{
		readBufferPosition = readBuffer + headerLength;
		bufferLength -= headerLength;
	}
	else
	{
		clearReadBuffer();
	}
	if (resultCode / 100 == 2)
	{
		retValue = true;
	}
	else
	{
		if (resultCode == 401)
		{
			debugPrintf(3, "Authorization required.\n");
			if (authorizationString)
			{
				setErrorNumber(WCE_BAD_AUTH);
			}
			else
			{
				setErrorNumber(WCE_AUTH);
				readBufferPosition = NULL;
			}
		}
		else if (resultCode == 404)
		{
			setErrorNumber(WCE_FILE_NOT_FOUND);
		}
		else if (resultCode == 302)
		{
			setErrorNumber(WCE_URL_MOVED);
		}
	}
	return retValue;
}

int TCWebClient::fetchHeader(int recursionCount)
{
//	char* result;
//	int length;

	if (recursionCount > 3)
	{
		// Don't allow more than 3 re-directs.
		return 0;
	}
	if (!sendFetchCommands())
	{
		return 0;
	}
//	debugPrintf("reading header: %d\n", dataSocket);
#ifdef WIN32
	WSASetLastError(0);
#else // WIN32
#ifdef _QT
	errno = 0;
#endif // _QT
#endif // WIN32
	if (!receiveHeader() || !readBuffer)
	{
		return 0;
	}
	if (!parseHeader())
	{
		if (errorNumber == WCE_URL_MOVED && locationField != NULL &&
			strlen(locationField))
		{
			errorNumber = 0;
			setErrorString(NULL);
			// Switch to the new URL.
			delete url;
			url = copyString(locationField);
			parseURL();
			// Reset all the header data
			pageLength = 0;
			delete contentType;
			contentType = NULL;
			serverTime = 0;
			serverTimeDelta = 0;
			lastModifiedTime = 0;
			chunked = false;
			delete locationField;
			locationField = NULL;
			closeConnection();
			delete readBuffer;
			bufferLength = 0;
			readBuffer = NULL;
			readBufferPosition = NULL;
			return fetchHeader(recursionCount + 1);
		}
		return 0;
	}
	headerFetched = 1;
	if (serverTime && serverTimeDelta && lastModifiedTime)
	{
		lastModifiedTime -= serverTimeDelta;
	}
	return 1;
}

void TCWebClient::appendToOutString(char*& outString, int& outSize, char c,
											 int& outSpot)
{
	if (outSpot >= outSize)
	{
		char* newOutString = new char[outSize * 2];

		memcpy(newOutString, outString, outSpot - 1);
		delete[] outString;
		outSize *= 2;
		outString = newOutString;
	}
	outString[outSpot++] = c;
}

char* TCWebClient::base64EncodedString(const char* inString)
{
	int inLength = strlen(inString);
	int outSize = inLength * 3 / 2;
	char* outString = new char[outSize];
	int inSpot = 0;
	int outSpot = 0;
	int state = 0;

	while (inSpot <= inLength)
	{
		int plainText = 0;
		int base64Text = 0;
		char c = (char)(inSpot < inLength ? inString[inSpot] : 0);

		switch (state)
		{
			case 0:
				if (inSpot >= inLength)
				{
					appendToOutString(outString, outSize, '\0', outSpot);
					return outString;
				}
				base64Text = (c >> 2);
				plainText = (c << 4) & 0x30;
				inSpot++;
				state = 2;
				break;
			case 2:
				base64Text = plainText | (c >> 4);
				plainText = (c << 2) & 0x3C;
				inSpot++;
				state = 4;
				break;
			case 4:
				base64Text = plainText | (c >> 6);
				plainText = c & 0x3F;
				inSpot++;
				state = 6;
				break;
			case 6:
				base64Text = plainText;
				state = 0;
				break;
			default:
				debugPrintf("Should we have gotten here?\n");
				break;
		}
		appendToOutString(outString, outSize, __pBase64[base64Text & 0x3f],
		 outSpot);
		if (inSpot > inLength)
		{
			switch (state)
			{
				case 0:
				case 2:
					appendToOutString(outString, outSize, '=', outSpot);
				case 4:
					appendToOutString(outString, outSize, '=', outSpot);
				case 6:
					appendToOutString(outString, outSize, '=', outSpot);
					appendToOutString(outString, outSize, '\0', outSpot);
					break;
				default:
					debugPrintf("We should not get here.\n");
					break;
			}
		}
	}
	return outString;
}

void TCWebClient::setAuthorizationString(const char* value)
{
	if (authorizationString != value)
	{
		delete authorizationString;
		authorizationString = copyString(value);
	}
}

char* TCWebClient::getAuthorizationString(void)
{
	if (!authorizationString && username && password)
	{
		char* buf = new char[strlen(username) + strlen(password) + 2];

		sprintf(buf, "%s:%s", username, password);
		authorizationString = base64EncodedString(buf);
		delete[] buf;
	}
	return authorizationString;
}

void TCWebClient::setUsername(const char* value)
{
	if (username != value)
	{
		delete[] username;
		username = copyString(value);
	}
}

void TCWebClient::setPassword(const char* value)
{
	if (password != value)
	{
		delete[] password;
		password = copyString(value);
	}
}

void TCWebClient::closeConnection(void)
{
	TCNetworkClient::closeConnection();
	socketSetup = 0;
}

int TCWebClient::setupSocket(void)
{
	if (TCNetworkClient::setupSocket())
	{
		socketSetup = 1;
		return 1;
	}
	else
	{
		return 0;
	}
}

int TCWebClient::sendFetchCommands(void)
{
	if (!connected)
	{
		if (!openConnection())
		{
			return 0;
		}
	}
	if (!headerFetched)
	{
		sendCommand("GET %s HTTP/1.1", serverPath);
		sendCommand("Host: %s", webServer);
		sendCommand("Connection: close");
		if (referer)
		{
			sendCommand("Referer: %s", referer);
		}
		if (getAuthorizationString())
		{
			sendCommand("Authorization: Basic %s", authorizationString);
		}
		sendCommand("");
	}
	return 1;
}

int TCWebClient::fetchURL(void)
{
/*
	while (1)
	{
		if (waitForRead())
		{
			char buf[10241];
			int receiveLength;

			receiveLength = recv(dataSocket, buf, sizeof(buf) - 1, 0);
			if (receiveLength == SOCKET_ERROR)
			{
				int errorNumber = WSAGetLastError();
				char errorBuf[1024];

				FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, errorNumber, 0,
					errorBuf, sizeof(errorBuf), NULL);
				printf("Socket error: %s\n", errorBuf);
			}
			else if (receiveLength > 0)
			{
				char *tmpBuffer = new char[receiveLength + bufferLength];

				if (readBuffer)
				{
					memcpy(tmpBuffer, readBuffer, bufferLength);
					delete readBuffer;
					readBuffer = NULL;
				}
				readBuffer = tmpBuffer;
				memcpy(readBuffer + bufferLength, buf, receiveLength);
				bufferLength += receiveLength;
			}
			else
			{
				break;
			}
		}
		else
		{
			return 0;
		}
	}
	readBufferPosition = readBuffer;
*/
	if (headerFetched || fetchHeader())
	{
		int length;

#ifdef WIN32
		WSASetLastError(0);
#else // WIN32
#ifdef _QT
		errno = 0;
#endif // _QT
#endif // WIN32
		if (outputDirectory)
		{
			if (downloadData())
			{
				doneFetching = 1;
				return 1;
			}
		}
		else
		{
			pageData = getData(length);
			if (pageData)
			{
				if (length != pageLength)
				{
					pageLength = length;
				}
				doneFetching = 1;
				return 1;
			}
		}
	}
	doneFetching = 1;
	return 0;
}

void TCWebClient::backgroundFetchURLStart(void)
{
	if (!fetchURL())
	{
		if (!errorString)
		{
			setErrorString("Unknown Error");
		}
	}
}

void TCWebClient::backgroundFetchHeaderStart(void)
{
	if (retryCount > 0)
	{
		do_sleep(1);
	}
	if (fetchHeader())
	{
		retryCount = 0;
	}
	else
	{
		if (!errorString)
		{
			setErrorString("Unknown Error");
		}
	}
}

void TCWebClient::backgroundFetchURLFinish(void)
{
	if (owner && finishURLMemberFunction)
	{
		((*owner).*finishURLMemberFunction)(this);
	}
}

void TCWebClient::backgroundFetchHeaderFinish(void)
{
	if (owner && finishHeaderMemberFunction)
	{
		((*owner).*finishHeaderMemberFunction)(this);
	}
}

int TCWebClient::retryFetchHeaderInBackground(void)
{
	if (++retryCount > maxRetryCount)
	{
		setErrorNumber(WCE_MAX_RETRIES);
		return 0;
	}
	else
	{
		return fetchHeaderInBackground();
	}
}

int TCWebClient::fetchInBackground(bool header)
{
	ThreadHelper threadHelper(this, header);

	if (fetchThread)
	{
		fetchThread->join();
		delete fetchThread;
	}
	try
	{
		fetchThread = new boost::thread(threadHelper);
	}
	catch (...)
	{
		if (getDebugLevel() > 0)
		{
			perror("error spawning thread");
		}
		setErrorNumber(WCE_THREAD_CREATE);
		return 0;
	}
	return 1;
}

int TCWebClient::fetchHeaderInBackground(void)
{
	return fetchInBackground(true);
}

int TCWebClient::fetchURLInBackground(void)
{
	return fetchInBackground(false);
}

int TCWebClient::setNonBlock(void)
{
#ifdef WIN32
	DWORD input = 1;

	if (ioctlsocket(dataSocket, FIONBIO, &input) == SOCKET_ERROR)
	{
		int errorNumber = WSAGetLastError();
		char buf[1024];

		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, errorNumber, 0, buf,
			1024, NULL);
		debugPrintf("error: %s\n", buf);
#else // WIN32
#ifdef _QT
	if (fcntl(dataSocket, F_SETFL, O_NDELAY) == -1)
	{
#endif // _QT
#endif // WIN32
		debugPrintf("Error setting non-blocking IO.\n");
		setErrorNumber(WCE_NON_BLOCK);
		closeConnection();
		return 0;
	}
	else
	{
		return 1;
	}
}

int TCWebClient::waitForActivity(fd_set* readDescs, fd_set* writeDescs)
{
	struct timeval timeout;
	time_t startTime = time(NULL);
	time_t timeLeft = socketTimeout;
	fd_set errorDescs;
	fd_set origReadDescs;
	fd_set origWriteDescs;

	if (readDescs)
	{
		origReadDescs = *readDescs;
	}
	if (writeDescs)
	{
		origWriteDescs = *writeDescs;
	}

	timeout.tv_sec = 0;
	timeout.tv_usec = 250000;
	while (1)
	{
		FD_ZERO(&errorDescs);
		FD_SET(dataSocket, &errorDescs);
		if (readDescs)
		{
			*readDescs = origReadDescs;
		}
		if (writeDescs)
		{
			*writeDescs = origWriteDescs;
		}
		int result = select(dataSocket + 1, readDescs, writeDescs, &errorDescs,
			&timeout);

		if (result > 0)
		{
			break;
		}
		if (result == 0 ||
#ifdef WIN32
			WSAGetLastError() == WSAEINTR)
#else // WIN32
#ifdef _QT
			WSAGetLastError() == EINTR)
#endif // _QT
#endif // WIN32
		{
			if (getAborted())
			{
				return 0;
			}
			timeLeft = startTime + socketTimeout - time(NULL);
			debugPrintf("Timeout: %d seconds left.\n", timeLeft);
			if (timeLeft <= 0)
			{
				setErrorNumber(WCE_READ_TIMEOUT);
				return 0;
			}
		}
		else
		{
			if (getDebugLevel() > 0)
			{
				perror("Error during select()");
			}
			setErrorNumber(WCE_SELECT);
			return 0;
		}
	}
	if (FD_ISSET(dataSocket, &errorDescs))
	{
		if (getDebugLevel() > 0)
		{
			perror("Socket error");
		}
		setErrorNumber(WCE_SOCKET);
		return 0;
	}
	if (readDescs && FD_ISSET(dataSocket, readDescs))
	{
		debugPrintf(5, "Read ready.\n");
	}
	if ((readDescs && !FD_ISSET(dataSocket, readDescs)) ||
		 (writeDescs && !FD_ISSET(dataSocket, writeDescs)))
	{
		debugPrintf("Timeout during read.\n");
		setErrorNumber(WCE_READ_TIMEOUT);
		return 0;
	}
	return 1;
}

int TCWebClient::waitForRead(void)
{
	fd_set readDescs;

	FD_ZERO(&readDescs);
	FD_SET(dataSocket, &readDescs);
	return waitForActivity(&readDescs, NULL);
}

int TCWebClient::waitForWrite(void)
{
	fd_set writeDescs;

	FD_ZERO(&writeDescs);
	FD_SET(dataSocket, &writeDescs);
	return waitForActivity(NULL, &writeDescs);
}

int TCWebClient::writePacket(const void* packet, int length)
{
	int bytesWritten;

	if ((bytesWritten = fwrite(packet, 1, length, dataFile)) < length)
	{
		debugPrintf("Partial file write of %s: %d < %d\n", dataFilePath,
			bytesWritten, length);
		setErrorNumber(WCE_DISK_FULL);
		return 0;
	}
	return 1;
}

bool TCWebClient::downloadChunkedData(void)
{
	char *line;
	bool retValue = true;

	while (retValue)
	{
		int lineLength;

		line = getLine(lineLength);
		if (line)
		{
			delete line;
			line = getLine(lineLength);
			if (line)
			{
				lineLength -= 3;

				if (!writePacket(line, lineLength))
				{
					retValue = false;
				}
				delete line;
			}
			else
			{
				break;
			}
		}
		else
		{
			break;
		}
	}
	return retValue;
}

int TCWebClient::downloadData(void)
{
	int bytesLeft;
	int result = 1;

	if (!openDataFile())
	{
		return 0;
	}
	debugPrintf("downloading file: %s\n", dataFilePath);
	if (chunked)
	{
		result = downloadChunkedData() ? 1 : 0;
	}
	else
	{
		if (pageLength)
		{
			bytesLeft = pageLength;
		}
		else
		{
			bytesLeft = INT_MAX;
		}
		if (readBufferPosition)
		{
			if (!writePacket(readBufferPosition, bufferLength))
			{
				result = 0;
			}
			else
			{
				bytesRead = bufferLength;
				bytesLeft -= bufferLength;
				clearReadBuffer();
			}
		}
		readBuffer = new char[BUFFER_SIZE];
		while (bytesLeft > 0 && result && !getAborted())
		{
			int packetBytesRead;
			int readSize;

			if (!waitForRead())
			{
				if (getAborted())
				{
					break;
				}
				if (getDebugLevel() > 0)
				{
					perror("Timeout!");
				}
				result = 0;
				break;
			}
			if (bytesLeft > BUFFER_SIZE)
			{
				readSize = BUFFER_SIZE;
			}
			else
			{
				readSize = bytesLeft;
			}
			packetBytesRead = recv(dataSocket, readBuffer, readSize, 0);
			if (packetBytesRead == 0)
			{
				debugPrintf("Ran out of data.\n");
				bytesLeft = 0;
			}
			if (packetBytesRead == -1)
			{
				if (checkBlockingError())
				{
					if (getAborted() || !waitForRead())
					{
						result = 0;
					}
				}
				else
				{
					if (getDebugLevel() > 0)
					{
						perror("Error reading from socket");
					}
					setErrorNumber(WCE_SOCKET_READ);
					result = 0;
				}
			}
			else
			{
				if (!writePacket(readBuffer, packetBytesRead))
				{
					result = 0;
				}
				bytesRead += packetBytesRead;
				totalBytesRead += packetBytesRead;
				bytesLeft -= packetBytesRead;
			}
		}
	}
	pageLength = bytesRead;
	clearReadBuffer();
	fclose(dataFile);
	dataFile = NULL;
	return result;
}

void TCWebClient::clearReadBuffer(void)
{
	delete readBuffer;
	readBuffer = NULL;
	readBufferPosition = NULL;
	bufferLength = 0;
}

TCByte *TCWebClient::getChunkedData(int &length)
{
	char *data;
	char *line;
	int dataSize = BUFFER_SIZE;

	length = 0;
	data = new char[dataSize];
	while (1)
	{
		int lineLength;

		line = getLine(lineLength);
		if (line)
		{
			delete line;
			line = getLine(lineLength);
			if (line)
			{
				int newDataSize = dataSize;

//				debugPrintf(2, "%s", line);
				lineLength -= 3;
				while (lineLength + length > newDataSize)
				{
					newDataSize *= 2;
				}
				if (newDataSize > dataSize)
				{
					char *tmpData = new char[newDataSize];

					if (length)
					{
						memcpy(tmpData, data, length);
					}
					delete data;
					data = tmpData;
					dataSize = newDataSize;
				}
				memcpy(data + length, line, lineLength);
				length += lineLength;
				delete line;
			}
			else
			{
				break;
			}
		}
		else
		{
			break;
		}
	}
	clearReadBuffer();
	return (TCByte *)data;
}

TCByte* TCWebClient::getData(int& length)
{
	TCByte* data;
	TCByte* readSpot;
	int bytesLeft;
	int dataSize;

	if (chunked)
	{
		return getChunkedData(length);
	}
	if (pageLength)
	{
		dataSize = pageLength;
		bytesLeft = pageLength;
	}
	else
	{
		debugPrintf("no page length.\n");
		bytesLeft = INT_MAX;
		dataSize = BUFFER_SIZE;
	}
	data = new TCByte[dataSize];
	if (readBufferPosition)
	{
		int size = bufferLength;

		if (dataSize < size)
		{
			dataSize = size * 2;
			delete[] data;
			data = new TCByte[dataSize];
		}
		memcpy(data, readBufferPosition, size);
		bytesLeft -= size;
		readSpot = data + size;
		bytesRead = size;
	}
	else
	{
		readSpot = data;
		bytesRead = 0;
	}
	while (bytesLeft > 0 && !getAborted())
	{
		int packetBytesRead;
		int readSize;

		if (!waitForRead())
		{
			bool aborted = getAborted();
			if (getDebugLevel() > 0 && !aborted)
			{
				perror("Timeout!");
			}
			delete[] data;
			length = 0;
			if (readBuffer)
			{
				clearReadBuffer();
			}
			return NULL;
		}
		if (bytesLeft > BUFFER_SIZE)
		{
			readSize = BUFFER_SIZE;
		}
		else
		{
			readSize = bytesLeft;
		}
		if (dataSize < readSpot - data + readSize)
		{
			int newDataSize = dataSize;
			TCByte* newData;

			while (newDataSize < bytesRead + readSize)
			{
				newDataSize *= 2;
			}
			newData = new TCByte[newDataSize];
			memcpy(newData, data, bytesRead);
			readSpot = newData + bytesRead;
			delete[] data;
			data = newData;
		}
		packetBytesRead = recv(dataSocket, (char *)readSpot, readSize, 0);
		if (packetBytesRead == 0)
		{
			bytesLeft = 0;
		}
		else if (packetBytesRead == -1)
		{
			if (checkBlockingError())
			{
				debugPrintf(4, ".");
				if (getAborted() || !waitForRead())
				{
					delete[] data;
					length = 0;
					if (readBuffer)
					{
						clearReadBuffer();
					}
					return NULL;
				}
			}
			else
			{
				if (getDebugLevel() > 0)
				{
					perror("Error reading from socket");
				}
				setErrorNumber(WCE_SOCKET_READ);
				delete[] data;
				length = 0;
				if (readBuffer)
				{
					clearReadBuffer();
				}
				return NULL;
			}
		}
		else if (packetBytesRead > 0)
		{
			debugPrintf(3, "Got some data.\n");
			bytesRead += packetBytesRead;
			totalBytesRead += packetBytesRead;
			readSpot += packetBytesRead;
			bytesLeft -= packetBytesRead;
		}
	}
	length = bytesRead;
	return data;
}

bool TCWebClient::checkBlockingError(void)
{
#ifdef WIN32
	if (WSAGetLastError() == WSAEWOULDBLOCK)
#else // WIN32
#ifdef _QT
	if (WSAGetLastError() == EAGAIN)
#endif // _QT
#endif // WIN32
	{
		return true;
	}
	else
	{
		return false;
	}
}

// try to read one line of data.
char* TCWebClient::getLine(int& length)
{
	char buf[BUFFER_SIZE];
	char* data = NULL;
	char* lineEnd;
	int bytesRead;

	length = 0;
	if (readBufferPosition)
	{
		if ((lineEnd = strnstr2(readBufferPosition, "\r\n", bufferLength, 1)) !=
			NULL)
		{
			length = lineEnd - readBufferPosition + 2;
			data = new char[length+1];
			memcpy(data, readBufferPosition, length);
			data[length] = 0;
			if (length < bufferLength)
			{
				readBufferPosition += length;
				bufferLength -= length;
			}
			else
			{
				clearReadBuffer();
			}
			length++;
			return data;
		}
		else
		{
			length = bufferLength;
			data = new char[length];
			memcpy(data, readBufferPosition, length);
			clearReadBuffer();
		}
	}
	while (1)
	{
		if (waitForRead())
		{
			char* tmpData;

			if ((bytesRead = recv(dataSocket, (char *)buf, BUFFER_SIZE, 0)) ==
				-1)
			{
				if (checkBlockingError())
				{
					if (!waitForRead())
					{
	//					delete[] data;
						if (readBuffer)
						{
							clearReadBuffer();
						}
						else
						{
							delete[] data;
						}
						return NULL;
					}
					continue;
				}
				else
				{
					if (getDebugLevel() > 0)
					{
						perror("Error reading from socket");
					}
					setErrorNumber(WCE_SOCKET_READ);
					length = 0;
	//				delete[] data;
					if (readBuffer)
					{
						clearReadBuffer();
					}
					else
					{
						delete[] data;
					}
					return NULL;
				}
			}
			if (bytesRead == 0)
			{
				delete data;
				return NULL;
			}
			totalBytesRead += bytesRead;
			tmpData = new char[length + bytesRead + 1];
			if (data)
			{
				memcpy(tmpData, data, length);
			}
			if (readBuffer)
			{
				clearReadBuffer();
			}
			else
			{
				delete[] data;
			}
			memcpy(tmpData+length, buf, bytesRead);
			tmpData[length + bytesRead] = 0;
			debugPrintf(4, "In buffer:\n%s\n", tmpData);
			data = tmpData;
			length += bytesRead;
			if ((lineEnd = strnstr2(data, "\r\n", length, 1)) != NULL)
			{
				bufferLength = length;
				length = lineEnd + 2 - data;

				bufferLength -= length;
				if (bufferLength)
				{
					readBuffer = data;
					readBufferPosition = lineEnd + 2;
					data = new char[length + 1];
					memcpy(data, readBuffer, length);
					data[length] = 0;
					length++;
					return data;
				}
				else
				{
					length++;
					return data;
				}
			}
		}
		else
		{
			if (data)
			{
				delete data;
			}
			return NULL;
		}
	}
}

void TCWebClient::sendData(int length, const void* data)
{
	int bytesWritten;

	while ((bytesWritten = send(dataSocket, (char *)data, length, 0)) < length)
	{
		if (bytesWritten == -1)
		{
			if (checkBlockingError())
			{
				if (getDebugLevel() > 0)
				{
					perror("Error writing to socket");
				}
				setErrorNumber(WCE_SOCKET_WRITE);
				return;
			}
		}
		if (!waitForWrite())
		{
			return;
		}
	}
}

void TCWebClient::sendCommand(const char* fmt, ...)
{
	va_list argPtr;
	char buf[1024];

	va_start(argPtr, fmt);
	vsprintf(buf, fmt, argPtr);
	va_end(argPtr);
	sendPrintf("%s\r\n", buf);
}

void TCWebClient::sendString(const char* str)
{
	sendData(strlen(str), str);
}

char* TCWebClient::getFilename(void)
{
	if (!filename && url)
	{
		char* spot = strrchr(url, '/');

		if (spot && strlen(spot))
		{
			char* spot2 = strstr(url, "://");

			if (!spot2 || spot2 != spot - 2)
			{
				filename = copyString(spot+1);
			}
		}
		if (!filename)
		{
			filename = copyString("index.html");
		}
	}
	return filename;
}

void TCWebClient::setErrorNumber(int value)
{
	errorNumber = value;
	switch (value)
	{
		case WCE_AUTH:
			setErrorString("Authorization required.");
			break;
		case WCE_BAD_AUTH:
			setErrorString("Authorization failed.");
			break;
		case WCE_THREAD_CREATE:
			setErrorString("Error creating fetch thread", WSAGetLastError());
			break;
		case WCE_NON_BLOCK:
			setErrorString("Error setting non-blocking I/O", WSAGetLastError());
			break;
		case WCE_SELECT:
			setErrorString("Error during select", WSAGetLastError());
			break;
		case WCE_SOCKET:
			setErrorString("Socket error", WSAGetLastError());
			break;
		case WCE_SOCKET_READ:
			setErrorString("Error reading from socket", WSAGetLastError());
			break;
		case WCE_SOCKET_WRITE:
			setErrorString("Error writing to socket", WSAGetLastError());
			break;
		case WCE_READ_TIMEOUT:
			setErrorString("Timeout during read.");
			break;
		case WCE_FILE_NOT_FOUND:
			setErrorString("File not found.");
			break;
		case WCE_DISK_FULL:
			setErrorString("Disk full.");
			break;
		case WCE_FILE_CREATION:
			setErrorString("Error creating file for writing",
				WSAGetLastError());
			break;
		case WCE_DIR_CREATION:
			setErrorString("Error creating directory", WSAGetLastError());
			break;
		case WCE_DIR_STAT:
			setErrorString("Error statting directory", WSAGetLastError());
			break;
		case WCE_NOT_DIR:
			setErrorString("Required directory is not a directory.");
			break;
		case WCE_MAX_RETRIES:
			setErrorString("Max retries reached.");
			break;
		case WCE_URL_MOVED:
			setErrorString("URL Moved.");
			break;
		default:
			TCNetworkClient::setErrorNumber(value);
			break;
	}
}

int TCWebClient::isWebPage(void)
{
	return contentType && strcasecmp(contentType, "text/html") == 0;
}

int TCWebClient::isImage(void)
{
	return contentType && stringHasCaseInsensitivePrefix(contentType, "image/");
}

int TCWebClient::isGif(void)
{
	return contentType && strcasecmp(contentType, "image/gif") == 0;
}

int TCWebClient::isJpeg(void)
{
	return contentType && strcasecmp(contentType, "image/jpeg") == 0;
}

int TCWebClient::isVideo(void)
{
	return contentType && stringHasCaseInsensitivePrefix(contentType, "video/");
}

int TCWebClient::isMpeg(void)
{
	return contentType && strcasecmp(contentType, "video/mpeg") == 0;
}

void TCWebClient::setOwner(TCObject* value)
{
	// Do not retain, since it owns us.
	owner = value;
}

void TCWebClient::setFinishURLMemberFunction(WebClientFinishMemberFunction
											 value)
{
	finishURLMemberFunction = value;
}

void TCWebClient::setFinishHeaderMemberFunction(WebClientFinishMemberFunction
												value)
{
	finishHeaderMemberFunction = value;
}

int TCWebClient::createDirectory(const char* directory)
{
	int errorNumber;
	int retValue = createDirectory(directory, &errorNumber);

	if (!retValue)
	{
		setErrorNumber(errorNumber);
	}
	return retValue;
}

int TCWebClient::createDirectory(const char* directory, int *errorNumber)
{
	struct stat dirStat;
	int result = 1;
	int dummyErrorNumber;

	if (!errorNumber)
	{
		errorNumber = &dummyErrorNumber;
	}
	if (stat(directory, &dirStat) == -1)
	{
		if (errno == ENOENT)
		{
#ifdef WIN32
			if (!CreateDirectory(directory, NULL))
			{
				*errorNumber = WCE_DIR_CREATION;
				result = 0;
			}
#else // WIN32
#ifdef _QT
			if (mkdir(directory, 0) == -1)
			{
				*errorNumber = WCE_DIR_CREATION;
				result = 0;
			}
			else
			{
				chmod(directory, S_IRUSR | S_IWUSR | S_IXUSR);
			}
#endif // _QT
#endif // WIN32
		}
		else
		{
			*errorNumber = WCE_DIR_STAT;
			result = 0;
		}
	}
	else
	{
#ifdef WIN32
		if (!(dirStat.st_mode & _S_IFDIR))
#else // WIN32
#ifdef _QT
		if (!S_ISDIR(dirStat.st_mode))
#endif // _QT
#endif // WIN32
		{
			*errorNumber = WCE_NOT_DIR;
			result = 0;
		}
	}
	return result;
}

int TCWebClient::createDirectories(const char* theDirectory)
{
	char* directory = copyString(theDirectory);
	char* spot = directory;
	int result = 1;

	while (*spot == '/')
	{
		spot++;
	}
	while (result && (spot = strchr(spot, '/')) != NULL)
	{
		*spot = 0;
		if (!createDirectory(directory))
		{
			result = 0;
		}
		*spot = '/';
		while (*spot == '/')
		{
			spot++;
		}
	}
	if (directory[strlen(directory) - 1] != '/')
	{
		if (!createDirectory(directory))
		{
			result = 0;
		}
	}
	delete directory;
	return result;
}

int TCWebClient::openDataFile(void)
{
	char* directory = outputDirectory;

	delete dataFilePath;
	dataFilePath = NULL;
	getFilename();
	if (directory)
	{
		if (!createDirectories(directory))
		{
			return 0;
		}
		dataFilePath = new char[strlen(directory) + strlen(filename) + 2];
		sprintf(dataFilePath, "%s/%s", directory, filename);
	}
	else
	{
		dataFilePath = copyString(filename);
	}
	if ((dataFile = fopen(dataFilePath, "wb")) != NULL)
	{
		return 1;
	}
	else
	{
		delete dataFilePath;
		dataFilePath = NULL;
		return 0;
	}
}

int TCWebClient::writeFile(const char* directory)
{
	int result = 0;

	if (errorNumber)
	{
		debugPrintf("errorNumber: %d\n", errorNumber);
		if (errorString)
		{
			debugPrintf("errorString: %s\n", errorString);
		}
	}
	if (doneFetching && pageData)
	{
		setOutputDirectory(directory);
		if (openDataFile())
		{
			int bytesWritten;

			debugPrintf("writing file: %s\n", dataFilePath);
			if ((bytesWritten = fwrite(pageData, 1, pageLength, dataFile)) <
			 pageLength)
			{
				debugPrintf("Partial file write of %s: %d < %d\n", dataFilePath,
					 bytesWritten, pageLength);
				setErrorNumber(WCE_DISK_FULL);
			}
			else
			{
				result = 1;
			}
			fclose(dataFile);
			dataFile = NULL;
			delete dataFilePath;
			dataFilePath = NULL;
		}
		else
		{
			if (getDebugLevel() > 0)
			{
				perror("Error opening data file for writing");
			}
			setErrorNumber(WCE_FILE_CREATION);
		}
	}
	else
	{
		if (getDebugLevel() > 0)
		{
			if (!doneFetching)
			{
				debugPrintf("Write attempt before fetch done.\n");
			}
		}
	}
	return result;
}

void TCWebClient::setOutputDirectory(const char* value)
{
	if (outputDirectory != value)
	{
		delete outputDirectory;
		outputDirectory = copyString(value);
	}
}

void TCWebClient::setMaxRetries(int value)
{
	maxRetries = value;
}

void TCWebClient::setReferer(char* value)
{
	if (referer != value)
	{
		delete referer;
		referer = copyString(value);
	}
}
