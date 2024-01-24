#ifndef __WINWEBCLIENTPLUGIN_H__
#define __WINWEBCLIENTPLUGIN_H__

#include <TCFoundation/TCWebClient.h>
#include <winhttp.h>

class WinWebClientPlugin: public TCWebClient::Plugin
{
public:
	WinWebClientPlugin(void);
	virtual bool isSupportedURLScheme(const char* urlScheme);
	virtual TCByte* fetchURL(const char* url, int& length, TCWebClient* webClient);
	static void registerPlugin(void);
private:
	class HttpHandle
	{
	public:
		HttpHandle() : handle(NULL) {}
		HttpHandle(HINTERNET handle) : handle(handle) {}
		~HttpHandle()
		{
			release();
		}
		bool isValid() { return handle != NULL;  }
		HINTERNET get() { return handle; }
		void release()
		{
			if (handle != NULL)
			{
				WinHttpCloseHandle(handle);
				handle = NULL;
			}
		}
		void acquire(HINTERNET value)
		{
			release();
			handle = value;
		}
	private:
		HINTERNET handle;
	};
	bool openSession(TCWebClient* webClient);

	static WinWebClientPlugin* plugin;
	HttpHandle hSession;
};

#endif // __WINWEBCLIENTPLUGIN_H__
