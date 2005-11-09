#ifndef __WEBCLIENT_H__
#define __WEBCLIENT_H__

#include <TCFoundation/TCNetworkClient.h>
#include <TCFoundation/TCThread.h>

#include <stdio.h>

#ifdef WIN32
#include <time.h>

typedef __int64 int64;
#define ZERO64 0i64
#else // WIN32
#ifdef _QT
#include <sys/time.h>
#include <unistd.h>

typedef long long int64;
#define ZERO64 0ll
#endif // _QT
#endif // WIN32

class TCThreadManager;
class TCThread;

#define WCE_AUTH TCNC_MAX_ERROR + 1
#define WCE_BAD_AUTH TCNC_MAX_ERROR + 2
#define WCE_THREAD_CREATE TCNC_MAX_ERROR + 3
#define WCE_NON_BLOCK TCNC_MAX_ERROR + 4
#define WCE_SELECT TCNC_MAX_ERROR + 5
#define WCE_SOCKET TCNC_MAX_ERROR + 6
#define WCE_SOCKET_READ TCNC_MAX_ERROR + 7
#define WCE_SOCKET_WRITE TCNC_MAX_ERROR + 8
#define WCE_READ_TIMEOUT TCNC_MAX_ERROR + 9
#define WCE_FILE_NOT_FOUND TCNC_MAX_ERROR + 10
#define WCE_DISK_FULL TCNC_MAX_ERROR + 11
#define WCE_FILE_CREATION TCNC_MAX_ERROR + 12
#define WCE_DIR_CREATION TCNC_MAX_ERROR + 13
#define WCE_DIR_STAT TCNC_MAX_ERROR + 14
#define WCE_NOT_DIR TCNC_MAX_ERROR + 15
#define WCE_MAX_RETRIES TCNC_MAX_ERROR + 16
#define WCE_URL_MOVED TCNC_MAX_ERROR + 17

class TCWebClient;

typedef void (TCObject::*WebClientFinishMemberFunction)(TCWebClient*);

class TCWebClient : public TCNetworkClient
{
	public:
		TCWebClient(const char* url);
		virtual ~TCWebClient(void);
		virtual int setNonBlock(void);

		virtual int openConnection(void);
		virtual void sendData(int, const void*);
		virtual int downloadData(void);
		virtual void sendCommand(const char* fmt, ...);
		virtual void sendString(const char*);
		int64 getTotalBytesRead(void) { return totalBytesRead; };
		void resetTotalBytesRead(void) { totalBytesRead = ZERO64; };
		char* getWebServer(void) { return webServer; }
		char* getServerPath(void) { return serverPath; }
		char* getURL(void) { return url; }
		virtual int fetchURL(void);
		virtual int fetchURLInBackground(void);
		virtual int fetchHeader(int recursionCount = 0);
		virtual int fetchHeaderInBackground(void);
		virtual int retryFetchHeaderInBackground(void);
		TCByte* getPageData(void) { return pageData; }
		int getPageLength(void) { return pageLength; }
		virtual void setUsername(const char* value);
		char* getUsername(void) { return username; }
		virtual void setPassword(const char* value);
		char* getPassword(void) { return password; }
		int getBytesRead(void) { return bytesRead; }
		int isDoneFetching(void) { return doneFetching != 0; }
		char* getFilename(void);
		virtual void setAuthorizationString(const char*);
		virtual char* getAuthorizationString(void);
		virtual void setContentType(const char*);
		virtual void setLocationField(const char* value);
		char* getContentType(void) { return contentType; }
		char* getLocationField(void) { return locationField; }
		const time_t& getServerTime(void) { return serverTime; }
		const time_t& getServerTimeDelta(void) { return serverTimeDelta; }
		const time_t& getLastModifiedTime(void) { return lastModifiedTime; }
		int isHeaderFetched(void) { return headerFetched != 0; }
		virtual int isWebPage(void);
		virtual int isImage(void);
		virtual int isGif(void);
		virtual int isJpeg(void);
		virtual int isVideo(void);
		virtual int isMpeg(void);
		TCThread* getFetchThread(void) { return fetchThread; }
		virtual void setOwner(TCObject*);
		TCObject* getOwner(void) { return owner; }
		virtual void setFinishURLMemberFunction(WebClientFinishMemberFunction);
		WebClientFinishMemberFunction getFinishURLMemberFunction(void)
		{
			return finishURLMemberFunction;
		}
		virtual void setFinishHeaderMemberFunction(WebClientFinishMemberFunction);
		WebClientFinishMemberFunction getFinishHeaderMemberFunction(void)
		{
			return finishHeaderMemberFunction;
		}
		virtual int writeFile(const char* = NULL);
		virtual void closeConnection(void);
		virtual void setOutputDirectory(const char*);
		virtual void setMaxRetries(int);
		int getMaxRetries(void) { return maxRetries; }
		virtual void setReferer(char*);
		char* getReferer(void) { return referer; }

		static int createDirectory(const char*, int *);
	protected:
		virtual TCByte* getData(int& length);
		virtual char* getLine(int&);
		virtual int parseURL(void);
		virtual void appendToOutString(char*&, int&, char, int&);
		virtual char* base64EncodedString(const char*);
		int waitForActivity(fd_set*, fd_set*);
		int waitForRead(void);
		int waitForWrite(void);
		virtual void dealloc(void);
		virtual THREAD_RET_TYPE backgroundFetchURLFunction(TCThread*);
		virtual THREAD_RET_TYPE backgroundFetchHeaderFunction(TCThread*);
		virtual void backgroundFetchURLFinish(TCThread*);
		virtual void backgroundFetchHeaderFinish(TCThread*);
		virtual void setErrorNumber(int);
		virtual time_t scanDateString(const char*);
		virtual int createDirectory(const char*);
		virtual int createDirectories(const char*);
		virtual int sendFetchCommands(void);
		virtual int setupSocket(void);
		virtual int openDataFile(void);
		virtual int writePacket(const void*, int);
		bool checkBlockingError(void);
		bool receiveHeader(void);
		bool parseHeader(void);
		void parseHeaderFields(int headerLength);
		TCByte *getChunkedData(int &length);
		bool downloadChunkedData(void);
		void clearReadBuffer(void);

		int socketTimeout;
		char* webServer;
		char* serverPath;
		char* url;
		char* referer;
		char* filename;
		char* outputDirectory;
		FILE* dataFile;
		char* dataFilePath;
		char* contentType;
		char* locationField;
		bool chunked;
		int bufferLength;
		char* readBuffer;
		char* readBufferPosition;
		int maxRetries;
		int pageLength;
		int headerFetched;
		time_t serverTime;
		time_t serverTimeDelta;
		time_t lastModifiedTime;
		TCByte* pageData;
		char* username;
		char* password;
		char* authorizationString;
		int bytesRead;
		TCThreadManager* threadManager;
		TCThread* fetchThread;
		int64 totalBytesRead;
		int doneFetching;
		TCObject* owner;
		WebClientFinishMemberFunction finishURLMemberFunction;
		WebClientFinishMemberFunction finishHeaderMemberFunction;
		int socketSetup;
		int retryCount;
		int maxRetryCount;
};

#endif
