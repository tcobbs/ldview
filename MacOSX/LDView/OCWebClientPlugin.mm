//
//  OCWebClientPlugin.mm
//  LDView
//
//  Created by Travis Cobbs on 2/5/23.
//

#import <Foundation/Foundation.h>
#import "OCWebClientPlugin.h"

#include <TCFoundation/TCWebClient.h>

class MacWebClientPlugin: public TCWebClient::Plugin
{
	virtual bool isSupportedURLScheme(const char* urlScheme);
	virtual TCByte* fetchURL(const char* url, int& length, TCWebClient* webClient);
};

static MacWebClientPlugin* plugin = NULL;

bool MacWebClientPlugin::isSupportedURLScheme(const char *scheme)
{
	return strcasecmp(scheme, "https") == 0;
}

@interface LDrawFetcher : NSObject <NSURLSessionDataDelegate, NSURLSessionTaskDelegate>
{
	NSString *lastModified;
	NSMutableData *receivedData;
	NSDictionary *headerFields;
	NSInteger statusCode;
	void (^completion)(LDrawFetcher *fetcher, NSData *data, NSError *error);
	bool done;
}
- (instancetype)initWithLastModified:(NSString *)theLastModified;
- (NSDictionary*)headerFields;
- (NSInteger)statusCode;

@end

@implementation LDrawFetcher

- (instancetype)initWithLastModified:(NSString *)theLastModified
{
	if ((self = [super init]) != nil)
	{
		lastModified = [theLastModified retain];
		statusCode = -1;
	}
	return self;
}

- (void)dealloc
{
	[lastModified release];
	[receivedData release];
	[headerFields release];
	[super dealloc];
}

- (NSDictionary*)headerFields
{
	return headerFields;
}

- (NSInteger)statusCode
{
	return statusCode;
}

- (void)URLSession:(NSURLSession *)session
		  dataTask:(NSURLSessionDataTask *)dataTask
didReceiveResponse:(NSURLResponse *)urlResponse
 completionHandler:(void (^)(NSURLSessionResponseDisposition disposition))completionHandler
{
	NSHTTPURLResponse* response = (NSHTTPURLResponse*)urlResponse;
	headerFields = [response.allHeaderFields retain];
	statusCode = response.statusCode;
	if (statusCode / 100 == 2)
	{
		NSString *responseLastModified = [response.allHeaderFields valueForKey:@"Last-Modified"];
		if (responseLastModified != nil && lastModified != nil)
		{
			time_t lastModifiedTime = TCWebClient::scanDateString(lastModified.UTF8String);
			time_t responseLastModifiedTime = TCWebClient::scanDateString(responseLastModified.UTF8String);
			if (lastModifiedTime >= responseLastModifiedTime)
			{
				statusCode = 304;
				completionHandler(NSURLSessionResponseCancel);
				done = true;
				completion(self, nil, nil);
				[session finishTasksAndInvalidate];
				return;
			}
		}
	}
	receivedData = [[NSMutableData alloc] init];
	completionHandler(NSURLSessionResponseAllow);
}

- (void)URLSession:(NSURLSession *)session
		  dataTask:(NSURLSessionDataTask *)dataTask
	didReceiveData:(NSData *)data
{
	[receivedData appendData:data];
}

- (void)URLSession:(NSURLSession *)session task:(NSURLSessionTask *)task
didCompleteWithError:(NSError *)error
{
	if (!done)
	{
		completion(self, receivedData, error);
		[session finishTasksAndInvalidate];
	}
}

- (void)fetchWithRequest:(NSURLRequest *)request completion:(void (^)(LDrawFetcher *fetcher, NSData *data, NSError *error))theCompletion
{
	// The ldraw.org web server was broken, and returned the file when
	// Last-Modified exactly matched the If-Modified-Since we pass it. In order
	// to avoid having to download a file that we already have, we had to use
	// a custom NSURLSession with a custom delegate. The delegate (self) checks
	// to see if the file is unchanged after the header has been fetched and
	// cancels the download if that is the case. While the ldraw.org web server
	// has apparently been fixed to not do this, this code is being left here,
	// since it will catch the problem if it returns in the future, and it
	// doesn't hurt anything.
	NSURLSession *session = [NSURLSession sessionWithConfiguration:[NSURLSessionConfiguration defaultSessionConfiguration] delegate:self delegateQueue:nil];
	completion = theCompletion;
	[[session dataTaskWithRequest:request] resume];
}

@end

TCByte* MacWebClientPlugin::fetchURL(const char *url, int& length, TCWebClient* webClient)
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	__block TCByte* result = NULL;
	length = 0;
	dispatch_semaphore_t sema = dispatch_semaphore_create(0);
	NSURLRequest* request = [NSURLRequest requestWithURL:[NSURL URLWithString:[NSString stringWithCString:url encoding:NSUTF8StringEncoding]]];
	NSMutableURLRequest* mutableRequest = [request mutableCopy];
	const char* userAgent = webClient->getUserAgent();
	const char* lastModifiedString = webClient->getLastModifiedString();
	NSString* lastModifiedNSString = nil;
	const char* referer = webClient->getReferer();
	const char* authorizationString = webClient->getAuthorizationString();
	// If we let NSURLRequest cache, it will simply return the original data if the file
	// has not been modified, and we won't have any way to know that. Disabling the cache
	// causes a 304 return value, which we then deal with correctly.
	mutableRequest.cachePolicy = NSURLRequestReloadIgnoringLocalCacheData;
	if (userAgent != NULL)
	{
		[mutableRequest setValue:[NSString stringWithUTF8String:userAgent] forHTTPHeaderField:@"User-Agent"];
	}
	if (lastModifiedString != NULL)
	{
		lastModifiedNSString = [NSString stringWithUTF8String:lastModifiedString];
		[mutableRequest setValue:lastModifiedNSString forHTTPHeaderField:@"If-Modified-Since"];
	}
	if (referer != NULL)
	{
		[mutableRequest setValue:[NSString stringWithUTF8String:referer] forHTTPHeaderField:@"Referer"];
	}
	if (authorizationString != NULL)
	{
		[mutableRequest setValue:[NSString stringWithFormat: @"Basic %@", [NSString stringWithUTF8String:authorizationString]] forHTTPHeaderField:@"Authorization"];
	}
	[mutableRequest setValue:@"gzip, identity" forHTTPHeaderField:@"Accept-Encoding"];
	LDrawFetcher* fetcher = [[LDrawFetcher alloc] initWithLastModified:lastModifiedNSString];
	[fetcher fetchWithRequest:mutableRequest completion:^(LDrawFetcher *fetcher, NSData *data, NSError *error){
		if (error != nil)
		{
			webClient->setErrorNumber(WCE_FILE_NOT_FOUND);
			dispatch_semaphore_signal(sema);
			return;
		}
		std::string header;
		NSDictionary* headerFields = [fetcher headerFields];
		for (id key in headerFields)
		{
			header += "\r\n";
			header += [key UTF8String];
			header += ": ";
			header += [[headerFields valueForKey:key] UTF8String];
		}
		header += "\r\n\r\n";
		webClient->parseHeaderFields(header.c_str(), (int)header.size());
		NSInteger statusCode = [fetcher statusCode];
		if (statusCode == 200)
		{
			if (data.length > 0)
			{
				result = new TCByte[data.length];
				memcpy(result, data.bytes, data.length);
				length = (int)data.length;
			}
		}
		else
		{
			webClient->parseErrorResultCode((int)statusCode);
		}
		dispatch_semaphore_signal(sema);
	}];
	if (![NSThread isMainThread]) {
		dispatch_semaphore_wait(sema, DISPATCH_TIME_FOREVER);
	} else {
		while (dispatch_semaphore_wait(sema, DISPATCH_TIME_NOW)) {
			[[NSRunLoop currentRunLoop] runMode:NSDefaultRunLoopMode beforeDate:[NSDate dateWithTimeIntervalSinceNow:0]];
		}
	}
	dispatch_release(sema);
	[pool release];
	[mutableRequest release];
	[fetcher release];
	return result;
}

void registerWebClientPlugin(void)
{
	if (plugin == NULL)
	{
		plugin = new MacWebClientPlugin;
		TCWebClient::setPlugin(plugin);
	}
}
