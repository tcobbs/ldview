#include "WinWebClientPlugin.h"
#include <TCFoundation/mystring.h>

#ifndef WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY
#define WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY             4
#endif

WinWebClientPlugin* WinWebClientPlugin::plugin = NULL;

WinWebClientPlugin::WinWebClientPlugin(void):
	hSession(NULL)
{
}

bool WinWebClientPlugin::isSupportedURLScheme(const char* scheme)
{
	return strcasecmp(scheme, "https") == 0;
}

bool WinWebClientPlugin::openSession(TCWebClient* webClient)
{
	if (hSession.isValid())
	{
		return true;
	}
	std::wstring userAgent;
	utf8towstring(userAgent, webClient->getUserAgent());
	hSession.acquire(WinHttpOpen(userAgent.c_str(),
		WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY,
		WINHTTP_NO_PROXY_NAME,
		WINHTTP_NO_PROXY_BYPASS,
		0));
	return hSession.isValid();
}

TCByte* WinWebClientPlugin::fetchURL(const char* url, int& length, TCWebClient* webClient)
{
	if (!openSession(webClient))
	{
		return NULL;
	}
	std::wstring wurl;
	URL_COMPONENTS urlComp;
	memset(&urlComp, 0, sizeof(urlComp));
	urlComp.dwStructSize = sizeof(urlComp);
	urlComp.dwSchemeLength = (DWORD)-1;
	urlComp.dwHostNameLength = (DWORD)-1;
	urlComp.dwUrlPathLength = (DWORD)-1;
	urlComp.dwExtraInfoLength = (DWORD)-1;
	utf8towstring(wurl, url);
	if (!WinHttpCrackUrl(wurl.c_str(), 0, 0, &urlComp))
	{
		return NULL;
	}
	std::wstring scheme(urlComp.lpszScheme, urlComp.dwSchemeLength);
	if (wcscasecmp(scheme.c_str(), L"https") != 0)
	{
		return NULL;
	}
	std::wstring hostName(urlComp.lpszHostName, urlComp.dwHostNameLength);
	std::wstring urlPath(urlComp.lpszUrlPath, urlComp.dwUrlPathLength);
	HttpHandle hConnect = WinHttpConnect(hSession.get(),
		hostName.c_str(),
		INTERNET_DEFAULT_HTTPS_PORT,
		0);
	if (!hConnect.isValid())
	{
		return NULL;
	}
	const char* referer = webClient->getReferer();
	std::wstring wsReferer;
	LPCWSTR wReferer = WINHTTP_NO_REFERER;
	if (referer != NULL)
	{
		utf8towstring(wsReferer, referer);
		wReferer = wsReferer.c_str();
	}
	HttpHandle hRequest = WinHttpOpenRequest(hConnect.get(),
		NULL,
		urlPath.c_str(),
		NULL,
		wReferer,
		NULL,
		WINHTTP_FLAG_REFRESH | WINHTTP_FLAG_SECURE);
	if (!hRequest.isValid())
	{
		return NULL;
	}
	const char* lastModifiedString = webClient->getLastModifiedString();
	const char* authorizationString = webClient->getAuthorizationString();
	WinHttpAddRequestHeaders(hRequest.get(),
		L"Accept-Encoding: gzip, identity",
		(DWORD)-1,
		WINHTTP_ADDREQ_FLAG_ADD);
	if (lastModifiedString != NULL)
	{
		std::wstring value;
		utf8towstring(value, lastModifiedString);
		std::wstring header = L"If-Modified-Since: " + value;
		WinHttpAddRequestHeaders(hRequest.get(),
			header.c_str(),
			header.size(),
			WINHTTP_ADDREQ_FLAG_ADD);
	}
	if (authorizationString != NULL)
	{
		std::wstring value;
		utf8towstring(value, authorizationString);
		std::wstring header = L"Authorization: Basic " + value;
		WinHttpAddRequestHeaders(hRequest.get(),
			header.c_str(),
			header.size(),
			WINHTTP_ADDREQ_FLAG_ADD);
	}
	if (!WinHttpSendRequest(hRequest.get(),
		WINHTTP_NO_ADDITIONAL_HEADERS,
		0,
		WINHTTP_NO_REQUEST_DATA,
		0,
		0,
		0))
	{
		return NULL;
	}
	if (!WinHttpReceiveResponse(hRequest.get(), NULL))
	{
		return NULL;
	}
	DWORD dwStatusCode = 0;
	DWORD dwSize = sizeof(dwStatusCode);
	if (!WinHttpQueryHeaders(hRequest.get(),
		WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
		WINHTTP_HEADER_NAME_BY_INDEX,
		&dwStatusCode, &dwSize, WINHTTP_NO_HEADER_INDEX))
	{
		return NULL;
	}
	if (dwStatusCode != 200)
	{
		webClient->parseErrorResultCode((int)dwStatusCode);
		return NULL;
	}
	dwSize = 0;
	WinHttpQueryHeaders(hRequest.get(),
		WINHTTP_QUERY_RAW_HEADERS_CRLF,
		WINHTTP_HEADER_NAME_BY_INDEX,
		WINHTTP_NO_OUTPUT_BUFFER,
		&dwSize,
		WINHTTP_NO_HEADER_INDEX);
	std::wstring headerData;
	headerData.resize(dwSize / sizeof(wchar_t));
	if (!WinHttpQueryHeaders(hRequest.get(),
		WINHTTP_QUERY_RAW_HEADERS_CRLF,
		WINHTTP_HEADER_NAME_BY_INDEX,
		&headerData[0],
		&dwSize,
		WINHTTP_NO_HEADER_INDEX))
	{
		return NULL;
	}
	std::string utf8HeaderData;
	wstringtoutf8(utf8HeaderData, headerData);
	std::string headers = "\r\n" + utf8HeaderData + "\r\n\r\n";
	webClient->parseHeaderFields(headers.c_str(), (int)headers.size());
	TCByte* result = NULL;
	length = 0;
	do
	{
		dwSize = 0;
		if (!WinHttpQueryDataAvailable(hRequest.get(), &dwSize))
		{
			delete[] result;
			return NULL;
		}
		if (dwSize == 0)
		{
			break;
		}
		DWORD dwBytesRead = 0;
		std::vector<TCByte> buf(dwSize);
		if (!WinHttpReadData(hRequest.get(), &buf[0], dwSize, &dwBytesRead))
		{
			delete[] result;
			return NULL;
		}
		if (length == 0)
		{
			result = new TCByte[dwBytesRead];
			memcpy(result, &buf[0], dwBytesRead);
		}
		else
		{
			TCByte* tmp = result;
			result = new TCByte[(size_t)length + dwBytesRead];
			memcpy(result, tmp, length);
			delete[] tmp;
			memcpy(&result[length], &buf[0], dwBytesRead);
		}
		length += dwBytesRead;
	} while (dwSize > 0);
	return result;
}

void WinWebClientPlugin::registerPlugin(void)
{
	if (plugin == NULL)
	{
		plugin = new WinWebClientPlugin;
		TCWebClient::setPlugin(plugin, false);
	}
}
