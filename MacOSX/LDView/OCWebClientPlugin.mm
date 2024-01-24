//
//  OCWebClientPlugin.mm
//  LDView
//
//  Created by Travis Cobbs on 2/5/23.
//

#import <Foundation/Foundation.h>
#import "OCWebClientPlugin.h"

#include <TCFoundation/TCWebClient.h>

class WebClientPlugin: public TCWebClient::Plugin
{
	virtual bool isSupportedURLScheme(const char* urlScheme);
	virtual TCByte* fetchURL(const char* url, int& length, TCWebClient* webClient);
};

static WebClientPlugin* plugin = NULL;

bool WebClientPlugin::isSupportedURLScheme(const char *scheme)
{
	return strcasecmp(scheme, "https") == 0;
}

TCByte* WebClientPlugin::fetchURL(const char *url, int& length, TCWebClient* webClient)
{
	__block TCByte* result = NULL;
	length = 0;
	dispatch_semaphore_t sema = dispatch_semaphore_create(0);
	NSURLRequest* request = [NSURLRequest requestWithURL:[NSURL URLWithString:[NSString stringWithCString:url encoding:NSUTF8StringEncoding]]];
	NSMutableURLRequest* mutableRequest = [request mutableCopy];
	const char* userAgent = webClient->getUserAgent();
	const char* lastModifiedString = webClient->getLastModifiedString();
	const char* referer = webClient->getReferer();
	const char* authorizationString = webClient->getAuthorizationString();
	// If we let NSURLRequest cache, it will simply return the original data if the file
	// has not been modified, and we won't have any way to know that. Disabling the cache
	// causes a 304 return value, which we then deal with correctly.
	mutableRequest.cachePolicy = NSURLRequestReloadIgnoringLocalCacheData;
	if (userAgent)
	{
		[mutableRequest setValue:[NSString stringWithUTF8String:userAgent] forHTTPHeaderField:@"User-Agent"];
	}
	if (lastModifiedString)
	{
		[mutableRequest setValue:[NSString stringWithUTF8String:lastModifiedString] forHTTPHeaderField:@"If-Modified-Since"];
	}
	if (referer)
	{
		[mutableRequest setValue:[NSString stringWithUTF8String:referer] forHTTPHeaderField:@"Referer"];
	}
	if (authorizationString)
	{
		[mutableRequest setValue:[NSString stringWithFormat: @"Basic %@", [NSString stringWithUTF8String:authorizationString]] forHTTPHeaderField:@"Authorization"];
	}
	[mutableRequest setValue:@"gzip, identity" forHTTPHeaderField:@"Accept-Encoding"];
	[[[NSURLSession sharedSession] dataTaskWithRequest:mutableRequest completionHandler:^(NSData *data, NSURLResponse *urlResponse, NSError *error){
		if (error != nil)
		{
			webClient->setErrorNumber(WCE_FILE_NOT_FOUND);
			dispatch_semaphore_signal(sema);
			return;
		}
		NSHTTPURLResponse* response = (NSHTTPURLResponse*)urlResponse;
		std::string header;
		NSDictionary* headerFields = response.allHeaderFields;
		for (id key in response.allHeaderFields)
		{
			header += "\r\n";
			header += [key UTF8String];
			header += ": ";
			header += [[headerFields valueForKey:key] UTF8String];
		}
		header += "\r\n\r\n";
		webClient->parseHeaderFields(header.c_str(), (int)header.size());
		if (response.statusCode == 200)
		{
			if (data.length > 0)
			{
				result = new TCByte[data.length];
				memcpy(result, data.bytes, data.length);
				length = (int)data.length;
			}
			dispatch_semaphore_signal(sema);
			return;
		}
		else
		{
			webClient->parseErrorResultCode((int)response.statusCode);
		}
		[mutableRequest release];
		dispatch_semaphore_signal(sema);
	}] resume];
	if (![NSThread isMainThread]) {
		dispatch_semaphore_wait(sema, DISPATCH_TIME_FOREVER);
	} else {
		while (dispatch_semaphore_wait(sema, DISPATCH_TIME_NOW)) {
			[[NSRunLoop currentRunLoop] runMode:NSDefaultRunLoopMode beforeDate:[NSDate dateWithTimeIntervalSinceNow:0]];
		}
	}
	return result;
}

void registerWebClientPlugin(void)
{
	if (plugin == NULL)
	{
		plugin = new WebClientPlugin;
		TCWebClient::setPlugin(plugin);
	}
}
