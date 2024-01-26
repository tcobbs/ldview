#include "QtWebClientPlugin.h"
#include <TCFoundation/mystring.h>
#include <QUrl>
#include <QEventLoop>
#include <QNetworkReply>
//#include <QDebug>

QtWebClientPlugin* QtWebClientPlugin::plugin = NULL;

bool QtWebClientPlugin::isSupportedURLScheme(const char* scheme)
{
	return strcasecmp(scheme, "https") == 0;
}

TCByte* QtWebClientPlugin::fetchURL(const char* url, int& length, TCWebClient* webClient)
{
	//qInfo() << "URL: " << url;
	QNetworkAccessManager manager;
	if (QUrl(url).scheme().compare("https")!=0) 
	{
		return NULL;
	}
	QNetworkRequest request;
	QNetworkReply *reply;
	request.setUrl(QString(url));
	QEventLoop loop;
	const char* referer = webClient->getReferer();
	const char* lastModifiedString = webClient->getLastModifiedString();
	const char* authorizationString = webClient->getAuthorizationString();
	if (referer != NULL)
	{
		request.setRawHeader("Referer",referer);
	}
	if (lastModifiedString != NULL)
	{
		request.setRawHeader("If-Modified-Since",lastModifiedString);
	}
	if (authorizationString != NULL)
	{
		QString auth="Basic ";
		auth+=authorizationString;
		request.setRawHeader("Authorization",auth.toLocal8Bit());
	}
	//request.setRawHeader("Accept-Encoding","gzip, identity");
	reply = manager.get(request);
	QObject::connect(&manager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
	loop.exec();
	if(reply->error())
	{
		return NULL;
	}
	QList<QByteArray> headerList = reply->rawHeaderList();
	QString head;
	foreach(QByteArray header, headerList)
	{
		head+="\r\n";
		head+=header;
		head+=":";
		head+=reply->rawHeader(header);
	}
	head+="\r\n\r\n";
	webClient->parseHeaderFields(head.toLocal8Bit(),head.length());
	TCByte* result = NULL;
	QByteArray data;
	data=reply->readAll();
	length=data.size();
	result = new TCByte[length];
	memcpy(result,data.data(),length);
	return result;
}

void QtWebClientPlugin::registerPlugin(void)
{
	if (plugin == NULL)
	{
		plugin = new QtWebClientPlugin;
		TCWebClient::setPlugin(plugin, false);
	}
}
