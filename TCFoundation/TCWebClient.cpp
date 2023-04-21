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
#include <zlib.h>
#include <functional>

#ifndef USE_CPP11

#ifndef _DEBUG
#ifdef WIN32
#pragma warning(disable: 4710) // "Not inlined" warnings in release mode.
#endif // WIN32
#endif // _DEBUG

#ifdef WIN32
#pragma warning(push, 3)
#endif // WIN32

#ifndef _NO_BOOST
#include <boost/bind.hpp>
#endif // !_NO_BOOST

#ifdef WIN32
#pragma warning(pop)
#endif // WIN32

#endif // !USE_CPP11

#ifdef WIN32
//#define sleep(sec) Sleep((sec) * 1000)
#endif // WIN32

#ifdef WIN32
#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG
#endif // WIN32

#include <TCFoundation/mystring.h>

//#define BUFFER_SIZE 1
#define BUFFER_SIZE 1024
#define GZ_BLOCK_SIZE 1024

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

static void do_sleep(int sec)
{
#ifdef USE_CPP11
	std::this_thread::sleep_for(std::chrono::seconds(sec));
#else
#if defined(_NO_BOOST)
#ifdef WIN32
	Sleep(sec * 1000);
#else // WIN32
	sleep(sec);
#endif // WIN32
#elif defined(_OSMESA)
	sleep(sec);
#else // _OSMESA
	boost::xtime xt;

#if BOOST_VERSION >= 105000
	boost::xtime_get(&xt, boost::TIME_UTC_);
#else
#ifndef TIME_UTC
	boost::xtime_get(&xt, boost::TIME_UTC);
#else
	boost::xtime_get(&xt, TIME_UTC);
#endif
#endif
	xt.sec += sec;
	boost::thread::sleep(xt);
#endif // _OSMESA
#endif
}

TCWebClient::Plugin *TCWebClient::plugin = NULL;
char *TCWebClient::proxyServer = NULL;
int TCWebClient::proxyPort = 80;
TCWebClient::TCWebClientCleanup TCWebClient::webClientCleanup;
char *TCWebClient::userAgent = NULL;

TCWebClient::TCWebClientCleanup::~TCWebClientCleanup(void)
{
	delete[] proxyServer;
	proxyServer = NULL;
	delete[] userAgent;
	userAgent = NULL;
}

TCWebClient::TCWebClient(const char* url)
	:TCNetworkClient(80),
	 socketTimeout(30),
	 urlScheme(NULL),
	 webServer(NULL),
	 serverPath(NULL),
	 url(copyString(url)),
	 referer(NULL),
	 filename(NULL),
	 outputDirectory(NULL),
	 dataFile(NULL),
	 dataFilePath(NULL),
	 contentType(NULL),
	 contentEncoding(NULL),
	 gzipped(false),
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
	 lastModifiedString(NULL),
	 pageData(NULL),
	 username(NULL),
	 password(NULL),
	 authorizationString(NULL),
	 bytesRead(0),
#ifdef USE_CPP11
	 fetchThread(NULL),
	 mutex(new std::mutex),
#else
#ifndef _NO_BOOST
	 fetchThread(NULL),
	 mutex(new boost::mutex),
#endif // _NO_BOOST
#endif
	 totalBytesRead(ZERO64),
	 doneFetching(0),
	 owner(NULL),
	 finishURLMemberFunction(NULL),
	 finishHeaderMemberFunction(NULL),
	 socketSetup(0),
	 retryCount(0),
	 maxRetryCount(10),
	 aborted(false),
	 zStreamInitialized(false),
	 gzHeader(NULL),
	 gzHeaderLen(0)
{
	parseURL();
}

TCWebClient::~TCWebClient(void)
{
}

void TCWebClient::dealloc(void)
{
//	debugPrintf("TCWebClient::dealloc()\n");
	delete[] urlScheme;
	delete[] webServer;
	delete[] serverPath;
	delete[] url;
	delete[] referer;
	delete[] filename;
	delete[] outputDirectory;
	if (dataFile)
	{
		fclose(dataFile);
	}
	delete[] dataFilePath;
	delete[] contentType;
	delete[] contentEncoding;
	delete[] locationField;
	delete[] readBuffer;
	// Do NOT delete readBufferPosition
	delete[] lastModifiedString;
	delete[] pageData;
	delete[] username;
	delete[] password;
	delete[] authorizationString;
#if defined(USE_CPP11) || !defined(_NO_BOOST)
	if (fetchThread)
	{
		delete fetchThread;
		fetchThread = NULL;
	}
	delete mutex;
#endif // USE_CPP11 || !_NO_BOOST
	// Do NOT delete owner
	TCNetworkClient::dealloc();
}

void TCWebClient::abort(void)
{
#ifdef USE_CPP11
	std::unique_lock<std::mutex> lock(*mutex);
#else
#ifndef _NO_BOOST
	boost::mutex::scoped_lock lock(*mutex);
#endif // _NO_BOOST
#endif

	aborted = true;
}

bool TCWebClient::getAborted(void)
{
#ifdef USE_CPP11
	std::unique_lock<std::mutex> lock(*mutex);
#else
#ifndef _NO_BOOST
	boost::mutex::scoped_lock lock(*mutex);
#endif // _NO_BOOST
#endif

	return aborted;
}

void TCWebClient::setProxyServer(const char *value)
{
	if (value != proxyServer)
	{
		delete[] proxyServer;
		proxyServer = copyString(value);
	}
}

void TCWebClient::setUserAgent(const char *value)
{
	if (value != userAgent)
	{
		delete[] userAgent;
		userAgent = copyString(value);
	}
}

int TCWebClient::openConnection(void)
{
	if (TCNetworkClient::openConnection())
	{
		if (setNonBlock())
		{
			setTcpNoDelay();
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

bool TCWebClient::parseURL(void)
{
	char* spot = strstr(url, "://");
	char* hostSpot = NULL;

	delete[] urlScheme;
	urlScheme = NULL;
	delete[] webServer;
	webServer = NULL;
	delete[] serverPath;
	serverPath = NULL;
	if (spot)
	{
		size_t length = spot - url;
		urlScheme = new char[length + 1];
		urlScheme[length] = 0;
		strncpy(urlScheme, url, length);
		hostSpot = spot + 3;
	}
	if (spot == NULL || strcasecmp(urlScheme, "http") != 0)
	{
		setErrorNumber(WCE_NOT_HTTP);
		return false;
	}
	if ((spot = strchr(hostSpot, '/')) != NULL)
	{
		size_t length = spot - hostSpot;

		webServer = new char[length + 1];
		strncpy(webServer, hostSpot, length);
		webServer[length] = 0;
		length = strlen(spot);
		serverPath = new char[length + 1];
		strcpy(serverPath, spot);
	}
	else
	{
		size_t length = strlen(hostSpot);

		webServer = new char[length + 1];
		strcpy(webServer, hostSpot);
		serverPath = new char[2];
		strcpy(serverPath, "/");
	}
	if (proxyServer)
	{
		setServerHost(proxyServer);
		port = proxyPort;
	}
	else
	{
		setServerHost(webServer);
	}
	return true;
}

void TCWebClient::setLocationField(const char* value)
{
	if (value != locationField)
	{
		const char *fieldEnd = strstr(value, "\r\n");

		delete[] locationField;
		if (fieldEnd)
		{
			size_t len = fieldEnd - value;

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

void TCWebClient::setFieldString(char *&field, const char *value)
{
	const char* crSpot;
	const char* lfSpot;
	const char* endSpot = 0;
	int length = -1;

	if (strchr(value, ':'))
	{
		value = strchr(value, ':') + 1;
		while (isspace(value[0]))
		{
			value++;
		}
	}
	while (value[0] == '\r' || value[0] == '\n')
	{
		value++;
	}
	crSpot = strchr(value, '\r');
	lfSpot = strchr(value, '\n');
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
		length = (int)strlen(value);
	}
	delete[] field;
	if (length == -1)
	{
		length = (int)(endSpot - value);
	}
	field = new char[(size_t)length + 1];
	strncpy(field, value, length);
	field[length] = 0;
}

void TCWebClient::setLastModifiedString(const char* value)
{
	if (value != lastModifiedString)
	{
		delete[] lastModifiedString;
		lastModifiedString = copyString(value);
	}
}

void TCWebClient::setLastModifiedStringField(const char* value)
{
	setFieldString(lastModifiedString, value);
}

void TCWebClient::setContentType(const char* value)
{
	setFieldString(contentType, value);
}

void TCWebClient::setContentEncoding(const char* value)
{
	setFieldString(contentEncoding, value);
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
#ifdef USE_CPP11
	char month[128] = { 0 };
#else // USE_CPP11
	char month[128];
#endif // USE_CPP11

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
//			char buf[10241];
			char buf[BUFFER_SIZE + 1];
			int receiveLength;

			receiveLength = (int)recv(dataSocket, buf, sizeof(buf) - 1, 0);
			if (receiveLength == SOCKET_ERROR)
			{
#ifdef WIN32
				int lerrorNumber = WSAGetLastError();
				char errorBuf[1024];

				FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM |
					FORMAT_MESSAGE_IGNORE_INSERTS, NULL, lerrorNumber, 0,
					errorBuf, sizeof(errorBuf), NULL);
				debugPrintf("Socket error: %s\n", errorBuf);
#endif
			}
			else if (receiveLength > 0)
			{
				char *tmpBuffer = new char[(size_t)receiveLength + bufferLength];

				if (readBuffer)
				{
					memcpy(tmpBuffer, readBuffer, bufferLength);
					delete[] readBuffer;
				}
				readBuffer = tmpBuffer;
				memcpy(readBuffer + bufferLength, buf, receiveLength);
				bufferLength += receiveLength;
				if (strnstr(buf, "\r\n\r\n", sizeof(buf) - 1) ||
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

void TCWebClient::parseHeaderFields(const char* header, int headerLength)
{
	char *fieldData;

	fieldData = strncasestr(header, "\r\nContent-Length: ",
		headerLength);
	if (fieldData)
	{
		if (sscanf(fieldData, "\r\nContent-Length: %d", &pageLength) != 1)
		{
			pageLength = 0;
		}
	}
	fieldData = strncasestr(header, "\r\nContent-Type: ", headerLength);
	if (fieldData)
	{
		setContentType(fieldData);
	}
	fieldData = strncasestr(header, "\r\nDate: ", headerLength);
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
	fieldData = strncasestr(header, "\r\nLast-Modified: ",
		headerLength);
	if (fieldData)
	{
		setLastModifiedStringField(fieldData);
		lastModifiedTime = scanDateString(fieldData + 17);
	}
	fieldData = strncasestr(header, "\r\nTransfer-Encoding: ",
		headerLength);
	if (fieldData)
	{
		if (stringHasCaseInsensitivePrefix(fieldData + 21, "chunked"))
		{
			chunked = true;
		}
	}
	fieldData = strncasestr(header, "\r\nLocation: ", headerLength);
	if (fieldData)
	{
		setLocationField(fieldData + 12);
	}
	gzipped = false;
	fieldData = strncasestr(header, "\r\nContent-Encoding: ", headerLength);
	if (fieldData)
	{
		setContentEncoding(fieldData);
		if (strcmp(contentEncoding, "gzip") == 0)
		{
			gzipped = true;
		}
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
		headerLength = (int)(fieldData - readBuffer + 4);
	}
	else
	{
		return false;
	}
	if (sscanf(readBuffer, "%*s %d", &resultCode) != 1)
	{
		return false;
	}
	parseHeaderFields(readBuffer, headerLength);
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
		parseErrorResultCode(resultCode);
	}
	return retValue;
}

void TCWebClient::parseErrorResultCode(int resultCode)
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
	else if (resultCode == 301)
	{
		setErrorNumber(WCE_URL_MOVED);
	}
	else if (resultCode == 302)
	{
		setErrorNumber(WCE_URL_MOVED);
	}
	else if (resultCode == 304)
	{
		setErrorNumber(WCE_NOT_MODIFIED);
	}
}

int TCWebClient::fetchHeader(int recursionCount)
{
//	char* result;
//	int length;

	if (errorNumber != 0)
	{
		return 0;
	}
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
		pageLength = 0;
		if (errorNumber == WCE_URL_MOVED && locationField != NULL &&
			strlen(locationField))
		{
			errorNumber = 0;
			setErrorString(NULL);
			// Switch to the new URL.
			delete[] url;
			url = copyString(locationField);
			if (!parseURL())
			{
				return 0;
			}
			// Reset all the header data
			delete[] contentType;
			contentType = NULL;
			serverTime = 0;
			serverTimeDelta = 0;
			lastModifiedTime = 0;
			chunked = false;
			delete[] locationField;
			locationField = NULL;
			closeConnection();
			delete[] readBuffer;
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

void TCWebClient::appendToOutString(
	char*& outString,
	size_t& outSize,
	char c,
	size_t& outSpot)
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
	size_t inLength = strlen(inString);
	size_t outSize = inLength * 3 / 2;
	char* outString = new char[outSize];
	size_t inSpot = 0;
	size_t outSpot = 0;
	int state = 0;
	int plainText = 0;

	while (inSpot <= inLength)
	{
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
				plainText = 0;
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
					FALLTHROUGH
				case 4:
					appendToOutString(outString, outSize, '=', outSpot);
					FALLTHROUGH
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
		delete[] authorizationString;
		authorizationString = copyString(value);
	}
}

char* TCWebClient::getAuthorizationString(void)
{
	if (!authorizationString && username && password)
	{
		size_t bufLen = strlen(username) + strlen(password) + 2;
		char* buf = new char[bufLen];

		snprintf(buf, bufLen, "%s:%s", username, password);
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
		if (proxyServer)
		{
			sendCommand("GET http://%s/%s HTTP/1.1", webServer, serverPath);
		}
		else
		{
			sendCommand("GET %s HTTP/1.1", serverPath);
		}
		sendCommand("Host: %s", webServer);
		sendCommand("Connection: close");
		sendCommand("Accept-Encoding: gzip, identity");
		if (userAgent)
		{
			sendCommand("User-Agent: %s", userAgent);
		}
		if (lastModifiedString)
		{
			sendCommand("If-Modified-Since: %s", lastModifiedString);
		}
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

bool TCWebClient::skipGZipHeader()
{
	TCByte *originalNextIn = zStream.next_in;
	int originalAvailIn = zStream.avail_in;

	// First, find the gzip header.
	while (zStream.avail_in >= 2 &&
		(zStream.next_in[0] != 0x1f || zStream.next_in[1] != 0x8b))
	{
		zStream.next_in++;
		zStream.avail_in--;
	}
	// Assuming we still 10 have bytes left, we should have the header.  (The
	// header is at least 10 bytes.
	if (zStream.avail_in >= 10 && zStream.next_in[0] == 0x1f &&
		zStream.next_in[1] == 0x8b)
	{
		TCByte flags = zStream.next_in[3];

		zStream.next_in += 10;
		zStream.avail_in -= 10;
		if ((flags & 0x04) != 0)
		{
			// skip the extra field (little endian!?!?!)
			int len = (zStream.next_in[1] << 8) | zStream.next_in[0];

			if ((int)zStream.avail_in >= len)
			{
				zStream.next_in += len;
				zStream.avail_in -= len;
			}
			else
			{
				zStream.avail_in = 0;
			}
		}
		if ((flags & 0x08) != 0)
		{
			// skip original filename
			while (zStream.avail_in > 0 && zStream.next_in[0] != 0)
			{
				zStream.next_in++;
				zStream.avail_in--;
			}
		}
		if ((flags & 0x10) != 0)
		{
			// skip comment
			while (zStream.avail_in > 0 && zStream.next_in[0] != 0)
			{
				zStream.next_in++;
				zStream.avail_in--;
			}
		}
		if ((flags & 0x02) != 0)
		{
			// skip the header CRC
			if (zStream.avail_in >= 2)
			{
				zStream.next_in += 2;
				zStream.avail_in -= 2;
			}
			else
			{
				zStream.avail_in = 0;
			}
		}
	}
	else
	{
		zStream.avail_in = 0;
	}
	if (zStream.avail_in == 0)
	{
		zStream.next_in = originalNextIn;
		zStream.avail_in = originalAvailIn;
		return false;
	}
	else
	{
		return true;
	}
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

				FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM |
					FORMAT_MESSAGE_IGNORE_INSERTS , NULL, errorNumber, 0,
					errorBuf, sizeof(errorBuf), NULL);
				printf("Socket error: %s\n", errorBuf);
			}
			else if (receiveLength > 0)
			{
				char *tmpBuffer = new char[receiveLength + bufferLength];

				if (readBuffer)
				{
					memcpy(tmpBuffer, readBuffer, bufferLength);
					delete[] readBuffer;
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
				int returnCode = 1;

				if (length != pageLength)
				{
					pageLength = length;
				}
				if (gzipped)
				{
					int outputAllocated = 0;
					TCByte *output = NULL;
					bool done = false;

					memset(&zStream, 0, sizeof(zStream));
					zStream.next_in = pageData;
					zStream.avail_in = pageLength;
					if (skipGZipHeader())
					{
						// I have no idea what the below -MAX_WBITS means, but
						// that's what the gzip code in zlib uses, so I'm using
						// it too.
						inflateInit2(&zStream, -MAX_WBITS);
						while (!done)
						{
							TCByte *newOutput;
							int zResult;

							newOutput = new TCByte[(size_t)outputAllocated +
								GZ_BLOCK_SIZE];
							if (output)
							{
								memcpy(newOutput, output, outputAllocated);
								delete[] output;
							}
							output = newOutput;
							zStream.next_out = output + outputAllocated;
							outputAllocated += GZ_BLOCK_SIZE;
							zStream.avail_out = GZ_BLOCK_SIZE;
							zResult = inflate(&zStream, Z_SYNC_FLUSH);
							if (zResult == Z_STREAM_END)
							{
								delete[] pageData;
								pageData = output;
								pageLength = (int)zStream.total_out;
								done = true;
							}
							else if ((int)zStream.total_out != outputAllocated)
							{
								returnCode = 0;
								done = true;
							}
							if (zResult < 0)
							{
								returnCode = 0;
								done = true;
							}
						}
						inflateEnd(&zStream);
					}
					else
					{
						returnCode = 0;
					}
				}
				doneFetching = 1;
				return returnCode;
			}
		}
	}
	else if (errorNumber == WCE_NOT_HTTP)
	{
		if (plugin != NULL && plugin->isSupportedURLScheme(urlScheme))
		{
			doneFetching = 1;
			pageData = plugin->fetchURL(url, pageLength, this);
			if (pageData != NULL)
			{
				errorNumber = 0;
				setErrorString(NULL);
				doneFetching = 1;
				if (outputDirectory != NULL)
				{
					writeFile(outputDirectory);
				}
				return 1;
			}
		}
	}
	doneFetching = 1;
	return 0;
}

#if defined(USE_CPP11) || !defined(_NO_BOOST)

void TCWebClient::backgroundFetchURL(void)
{
	backgroundFetchURLStart();
	backgroundFetchURLFinish();
}

void TCWebClient::backgroundFetchHeader(void)
{
	backgroundFetchHeaderStart();
	backgroundFetchHeaderFinish();
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
	//ThreadHelper threadHelper(this, header);

	if (fetchThread)
	{
		fetchThread->join();
		delete fetchThread;
	}
	try
	{
		if (header)
		{
#ifdef USE_CPP11
			fetchThread = new std::thread(&TCWebClient::backgroundFetchHeader, this);
#else
			fetchThread = new boost::thread(
				boost::bind(&TCWebClient::backgroundFetchHeader, this));
#endif
		}
		else
		{
#ifdef USE_CPP11
			fetchThread = new std::thread(&TCWebClient::backgroundFetchURL, this);
#else
			fetchThread = new boost::thread(
				boost::bind(&TCWebClient::backgroundFetchURL, this));
#endif
		}
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

#endif // USE_CPP11 || !_NO_BOOST

int TCWebClient::waitForActivity(fd_set* readDescs, fd_set* writeDescs)
{
	struct timeval timeout;
	time_t startTime = time(NULL);
	time_t timeLeft;
	fd_set errorDescs;
	fd_set origReadDescs;
	fd_set origWriteDescs;

	if (readDescs)
	{
		origReadDescs = *readDescs;
	}
	else
	{
		FD_ZERO(&origReadDescs);
	}
	if (writeDescs)
	{
		origWriteDescs = *writeDescs;
	}
	else
	{
		FD_ZERO(&origWriteDescs);
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
		int result = select((int)(dataSocket + 1), readDescs, writeDescs, &errorDescs,
			&timeout);

		if (result > 0)
		{
			break;
		}
		if (result == 0 ||
#ifdef WIN32
			WSAGetLastError() == WSAEINTR)
#else // WIN32
#if defined (_QT) || defined (__APPLE__) || defined (_OSMESA)
			WSAGetLastError() == EINTR)
#endif // _QT || __APPLE__ || _OSMESA
#endif // WIN32
		{
			if (getAborted())
			{
				return 0;
			}
			timeLeft = startTime + socketTimeout - time(NULL);
			debugPrintf(3, "Timeout: %td seconds left.\n", (ptrdiff_t)timeLeft);
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
	int returnValue = 1;
	TCByte* newPacket = (TCByte *)packet;

	if (gzipped)
	{
		int outputAllocated = 0;
		TCByte *output = NULL;
		bool done = false;

		if (gzHeader)
		{
			newPacket = new TCByte[(size_t)gzHeaderLen + length];
			memcpy(newPacket, gzHeader, gzHeaderLen);
			memcpy(&newPacket[gzHeaderLen], packet, length);
			length += gzHeaderLen;
			delete[] gzHeader;
			gzHeader = NULL;
			gzHeaderLen = 0;
		}
		zStream.next_in = newPacket;
		zStream.avail_in = length;
		if (!zStreamInitialized)
		{
			if (skipGZipHeader())
			{
				// I have no idea what the below -MAX_WBITS means, but that's
				// what the gzip code in zlib uses, so I'm using it too.
				inflateInit2(&zStream, -MAX_WBITS);
				zStreamInitialized = true;
			}
			else
			{
				gzHeaderLen = zStream.avail_in;
				gzHeader = new TCByte[gzHeaderLen];
				memcpy(gzHeader, zStream.next_in, gzHeaderLen);
				return 1;
			}
		}
		while (!done)
		{
			TCByte *newOutput;
			int zResult;

			newOutput = new TCByte[(size_t)outputAllocated + GZ_BLOCK_SIZE];
			if (output)
			{
				memcpy(newOutput, output, outputAllocated);
				delete[] output;
			}
			output = newOutput;
			zStream.next_out = output + outputAllocated;
			outputAllocated += GZ_BLOCK_SIZE;
			zStream.avail_out = GZ_BLOCK_SIZE;
			zResult = inflate(&zStream, Z_SYNC_FLUSH);
			if (zResult == Z_STREAM_END ||
				zStream.next_out - output != outputAllocated)
			{
				length = (int)(zStream.next_out - output);
				done = true;
			}
			if (zResult < 0)
			{
				returnValue = 0;
				done = true;
			}
		}
		if (newPacket != packet)
		{
			delete[] newPacket;
		}
		newPacket = output;
	}
	if (returnValue &&
		(bytesWritten = (int)fwrite(newPacket, 1, length, dataFile)) < length)
	{
		debugPrintf("Partial file write of %s: %d < %d\n", dataFilePath,
			bytesWritten, length);
		setErrorNumber(WCE_DISK_FULL);
		returnValue = 0;
	}
	if (newPacket != packet)
	{
		delete[] newPacket;
	}
	return returnValue;
}

bool TCWebClient::downloadChunkedData(void)
{
	// In chunked downloads, every even "line" is a hex number indicating the
	// length of the next chunk, followed by CRLF. Every odd "line" is length
	// bytes long, followed by CRLF, and represents the actual data.
	while (1)
	{
		int lineLength;
		char *chunkSizeLine = getLine(lineLength);
		if (chunkSizeLine != NULL)
		{
			int chunkSize;
			bool haveChunkSize = sscanf(chunkSizeLine, "%x", &chunkSize) == 1;
			delete[] chunkSizeLine;
			if (!haveChunkSize)
			{
				return false;
			}
			if (chunkSize == 0)
			{
				// Note that there is a suffix chunk after this, but we don't
				// care about that.
				return true;
			}
			char *chunk = getChunk(chunkSize + 2); // + 2 is for CRLF
			if (chunk[chunkSize] != '\r' || chunk[chunkSize + 1] != '\n')
			{
				delete[] chunk;
				return false;
			}
			if (chunk != NULL)
			{
				bool wrote = writePacket(chunk, chunkSize);
				delete[] chunk;
				if (!wrote)
				{
					return false;
				}
			}
		}
		else
		{
			return false;
		}
	}
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
	memset(&zStream, 0, sizeof(zStream));
	zStreamInitialized = false;
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
			packetBytesRead = (int)recv(dataSocket, readBuffer, readSize, 0);
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
	if (gzipped && zStreamInitialized)
	{
		pageLength = (int)zStream.total_out;
		inflateEnd(&zStream);
	}
	else
	{
		pageLength = bytesRead;
	}
	clearReadBuffer();
	fclose(dataFile);
	dataFile = NULL;
	if (!result && dataFilePath != NULL)
	{
		// The download failed; delete the file if we created it.
		unlink(dataFilePath);
	}
	return result;
}

void TCWebClient::clearReadBuffer(void)
{
	if (readBuffer == NULL)
	{
		return;
	}
	delete[] readBuffer;
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
			delete[] line;
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
					delete[] data;
					data = tmpData;
					dataSize = newDataSize;
				}
				memcpy(data + length, line, lineLength);
				length += lineLength;
				delete[] line;
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
			bool laborted = getAborted();
			if (getDebugLevel() > 0 && !laborted)
			{
				perror("Timeout!");
			}
			delete[] data;
			length = 0;
			clearReadBuffer();
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
		packetBytesRead = (int)recv(dataSocket, (char *)readSpot, readSize, 0);
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
					clearReadBuffer();
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
				clearReadBuffer();
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
#if defined (_QT) || defined (__APPLE__) || defined (_OSMESA)
	if (WSAGetLastError() == EAGAIN)
#endif // _QT || __APPLE__ || _OSMESA
#endif // WIN32
	{
		return true;
	}
	else
	{
		return false;
	}
}

void TCWebClient::advanceReadBuffer(int amount)
{
	readBufferPosition += amount;
	bufferLength -= amount;
	if (bufferLength == 0)
	{
		clearReadBuffer();
	}
}

char* TCWebClient::getChunk(int chunkSize)
{
	char* data = new char[(size_t)chunkSize];
	int progress = 0;
	if (readBufferPosition != NULL)
	{
		if (bufferLength >= chunkSize)
		{
			memcpy(data, readBufferPosition, chunkSize);
			advanceReadBuffer(chunkSize);
			return data;
		}
		else
		{
			memcpy(data, readBufferPosition, bufferLength);
			progress = bufferLength;
			clearReadBuffer();
		}
	}
	while (1)
	{
		if (waitForRead())
		{
			char buf[BUFFER_SIZE];
			int lbytesRead;
			
			if ((lbytesRead = (int)recv(dataSocket, (char *)buf, BUFFER_SIZE, 0)) ==
				-1)
			{
				if (checkBlockingError())
				{
					if (!waitForRead())
					{
						clearReadBuffer();
						delete[] data;
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
					clearReadBuffer();
					delete[] data;
					return NULL;
				}
			}
			if (lbytesRead == 0)
			{
				clearReadBuffer();
				delete[] data;
				return NULL;
			}
			totalBytesRead += lbytesRead;
			if (lbytesRead + progress >= chunkSize)
			{
				memcpy(&data[progress], buf, chunkSize - progress);
				if (lbytesRead + progress > chunkSize)
				{
					bufferLength = lbytesRead + progress - chunkSize;
					readBuffer = new char[bufferLength];
					memcpy(readBuffer, &buf[chunkSize - progress], bufferLength);
					readBufferPosition = readBuffer;
				}
				return data;
			}
			else
			{
				memcpy(&data[progress], buf, lbytesRead);
				progress += lbytesRead;
			}
		}
		else
		{
			delete[] data;
			return NULL;
		}
	}
}

// try to read one line of data.
char* TCWebClient::getLine(int& length)
{
	char buf[BUFFER_SIZE];
	char* data = NULL;
	char* lineEnd;
	int lbytesRead;

	length = 0;
	if (readBufferPosition)
	{
		if ((lineEnd = strnstr2(readBufferPosition, "\r\n", bufferLength, 1)) !=
			NULL)
		{
			length = (int)(lineEnd - readBufferPosition + 2);
			data = new char[(size_t)length+1];
			memcpy(data, readBufferPosition, length);
			data[length] = 0;
			if (length < bufferLength)
			{
				advanceReadBuffer(length);
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

			if ((lbytesRead = (int)recv(dataSocket, (char *)buf, BUFFER_SIZE, 0)) ==
				-1)
			{
				if (checkBlockingError())
				{
					if (!waitForRead())
					{
						clearReadBuffer();
						delete[] data;
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
					clearReadBuffer();
					delete[] data;
					return NULL;
				}
			}
			if (lbytesRead == 0)
			{
				delete[] data;
				return NULL;
			}
			totalBytesRead += lbytesRead;
			tmpData = new char[(size_t)length + lbytesRead + 1];
			if (data)
			{
				memcpy(tmpData, data, length);
			}
			clearReadBuffer();
			delete[] data;
			memcpy(tmpData+length, buf, lbytesRead);
			tmpData[length + lbytesRead] = 0;
			debugPrintf(4, "In buffer:\n%s\n", tmpData);
			data = tmpData;
			length += lbytesRead;
			if ((lineEnd = strnstr2(data, "\r\n", length, 1)) != NULL)
			{
				bufferLength = length;
				length = (int)(lineEnd + 2 - data);

				bufferLength -= length;
				if (bufferLength)
				{
					readBuffer = data;
					readBufferPosition = lineEnd + 2;
					data = new char[(size_t)length + 1];
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
				delete[] data;
			}
			return NULL;
		}
	}
}

void TCWebClient::sendData(int length, const void* data)
{
	int bytesWritten;

	while ((bytesWritten = (int)send(dataSocket, (char *)data, length, 0)) < length)
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
	vsnprintf(buf, sizeof(buf), fmt, argPtr);
	va_end(argPtr);
	sendPrintf("%s\r\n", buf);
}

void TCWebClient::sendString(const char* str)
{
	sendData((int)strlen(str), str);
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
		case WCE_NOT_MODIFIED:
			setErrorString("File not modified.");
			break;
		case WCE_NOT_HTTP:
			setErrorString("URL is not http.");
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
	int lerrorNumber;
	int retValue = createDirectory(directory, &lerrorNumber);

	if (!retValue)
	{
		setErrorNumber(lerrorNumber);
	}
	return retValue;
}

int TCWebClient::createDirectory(const char* directory, int *errorNumber)
{
	struct stat dirStat;
	int result = 1;
	int dummyErrorNumber;
	bool bDrive = false;

	if (!errorNumber)
	{
		errorNumber = &dummyErrorNumber;
	}
#ifdef WIN32
	if (strlen(directory) == 2 && directory[1] == ':')
	{
		bDrive = true;
	}
#endif // WIN32
	if (!bDrive)
	{
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
#if defined (_QT) || defined (__APPLE__) || defined (_OSMESA)
				if (mkdir(directory, 0) == -1)
				{
					*errorNumber = WCE_DIR_CREATION;
					result = 0;
				}
				else
				{
					chmod(directory, S_IRUSR | S_IWUSR | S_IXUSR);
				}
#endif // _QT || __APPLE__ || _OSMESA
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
#if defined (_QT) || defined (__APPLE__) || defined (_OSMESA)
			if (!S_ISDIR(dirStat.st_mode))
#endif // _QT || __APPLE__ || _OSMESA
#endif // WIN32
			{
				*errorNumber = WCE_NOT_DIR;
				result = 0;
			}
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
	if (result && directory[strlen(directory) - 1] != '/')
	{
		if (!createDirectory(directory))
		{
			result = 0;
		}
	}
	delete[] directory;
	return result;
}

int TCWebClient::openDataFile(void)
{
	char* directory = outputDirectory;

	delete[] dataFilePath;
	dataFilePath = NULL;
	getFilename();
	if (directory)
	{
		if (!createDirectories(directory))
		{
			return 0;
		}
		size_t dataFilePathLen = strlen(directory) + strlen(filename) + 2;
		dataFilePath = new char[dataFilePathLen];
		snprintf(dataFilePath, dataFilePathLen, "%s/%s", directory, filename);
	}
	else
	{
		dataFilePath = copyString(filename);
	}
	if ((dataFile = ucfopen(dataFilePath, "wb")) != NULL)
	{
		return 1;
	}
	else
	{
		delete[] dataFilePath;
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
			if ((bytesWritten = (int)fwrite(pageData, 1, pageLength, dataFile))
				< pageLength)
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
			delete[] dataFilePath;
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
		delete[] outputDirectory;
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
		delete[] referer;
		referer = copyString(value);
	}
}

int TCWebClient::setNonBlock(void)
{
#ifdef WIN32
	DWORD input = 1;

	if (ioctlsocket(dataSocket, FIONBIO, &input) == SOCKET_ERROR)
	{
		int lerrorNumber = WSAGetLastError();
		char buf[1024];

		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS, NULL, lerrorNumber, 0, buf, 1024,
			NULL);
		debugPrintf("error: %s\n", buf);
#else // WIN32
#if defined (_QT) || defined (__APPLE__) || defined(_OSMESA)
	if (fcntl(dataSocket, F_SETFL, O_NDELAY) == -1)
	{
#endif // _QT || __APPLE__ || _OSMESA
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
