#ifndef __QTWEBCLIENTPLUGIN_H__
#define __QTWEBCLIENTPLUGIN_H__

#include <TCFoundation/TCWebClient.h>
#include <QNetworkAccessManager>
class QtWebClientPlugin: public TCWebClient::Plugin
{
public:
	virtual bool isSupportedURLScheme(const char* urlScheme);
	virtual TCByte* fetchURL(const char* url, int& length, TCWebClient* webClient);
	static void registerPlugin(void);
private:
	static QtWebClientPlugin* plugin;
};

#endif // __QTWEBCLIENTPLUGIN_H__
