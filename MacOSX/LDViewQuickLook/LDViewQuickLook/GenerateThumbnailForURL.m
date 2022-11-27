#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>
#include <QuickLook/QuickLook.h>
#include <Foundation/Foundation.h>
#include <AppKit/AppKit.h>
#include <stdlib.h>

OSStatus GenerateThumbnailForURL(void *thisInterface, QLThumbnailRequestRef thumbnail, CFURLRef url, CFStringRef contentTypeUTI, CFDictionaryRef options, CGSize maxSize);
void CancelThumbnailGeneration(void *thisInterface, QLThumbnailRequestRef thumbnail);

/* -----------------------------------------------------------------------------
    Generate a thumbnail for file

   This function's job is to create thumbnail for designated file as fast as possible
   ----------------------------------------------------------------------------- */

OSStatus GenerateThumbnailForURL(void *thisInterface, QLThumbnailRequestRef thumbnail, CFURLRef url, CFStringRef contentTypeUTI, CFDictionaryRef options, CGSize maxSize)
{
	NSUserDefaults *sud = [NSUserDefaults standardUserDefaults];
	NSDictionary<NSString *, id>* udDomain = [sud persistentDomainForName:@"com.cobbsville.LDViewQuickLook"];
	BOOL disableThumbnails = [[udDomain objectForKey:@"DisableThumbnails"] boolValue];
	if (disableThumbnails)
		return noErr;
	NSBundle *qlBundle = [NSBundle bundleWithIdentifier:@"com.cobbsville.LDViewQuickLook"];
	NSString *executable = nil;
	if (qlBundle)
	{
		NSString *bundlePath = qlBundle.bundlePath;
		executable = [[[[[bundlePath stringByDeletingLastPathComponent] stringByDeletingLastPathComponent] stringByDeletingLastPathComponent] stringByAppendingPathComponent:@"MacOS"] stringByAppendingPathComponent:@"LDView"];
		if (![[NSFileManager defaultManager] isExecutableFileAtPath:executable])
		{
			executable = nil;
		}
	}
	if (executable == nil)
	{
		// Users really shouldn't get here; it's for debugging the built version
		// before it has been wrapped in LDView.app. So the fact that it requires
		// macOS 10.10 is no big deal.
		if (@available(macOS 10.10, *))
		{
			NSURL *defaultAppUrl = (NSURL*)CFBridgingRelease(LSCopyDefaultApplicationURLForURL(url, kLSRolesViewer, NULL));
			if (defaultAppUrl == nil)
			{
				return -1;
			}
			NSURL *executableUrl = [defaultAppUrl URLByAppendingPathComponent:@"Contents/MacOS/LDView" isDirectory:NO];
			if (executableUrl == nil)
			{
				return -1;
			}
			executable = [NSString stringWithUTF8String:executableUrl.fileSystemRepresentation];
		}
	}
	if (executable.length == 0)
	{
		return -1;
	}
	NSString *template = [NSString stringWithFormat:@"%@%@", NSTemporaryDirectory(), @"LDViewThumb.XXXXXX"];
	const char *templateCString = template.UTF8String;
	if (templateCString != NULL)
	{
		char *tempFilenameBuf = (char*)malloc(strlen(templateCString) + 1);
		strcpy(tempFilenameBuf, templateCString);
		int fileDescriptor = mkstemp(tempFilenameBuf);
		NSString *tempFilename = [NSString stringWithUTF8String:tempFilenameBuf];
		close(fileDescriptor);
		NSURL *fileUrl = (__bridge NSURL*)url;
		NSDictionary *nsOptions = (__bridge NSDictionary *)options;
		double scaleFactor = [[nsOptions objectForKey:(__bridge NSString *) kQLThumbnailOptionScaleFactorKey] doubleValue];	// can be >1 on Retina displays
		if (scaleFactor == 0)
		{
			scaleFactor = 1.0;
		}
		int width = (int)(maxSize.width * scaleFactor);
		int height = (int)(maxSize.width * scaleFactor);
		const char *ldrFilename;
		if (@available(macOS 10.9, *))
		{
			ldrFilename = fileUrl.fileSystemRepresentation;
		}
		else
		{
			ldrFilename = fileUrl.path.UTF8String;
		}
		NSString *commandLine = [NSString stringWithFormat:@"\"%@\" \"%s\" "
			"-SaveSnapshot= \"%@\" -SaveActualSize=0 -SaveAlpha=1 "
			"-SaveWidth=%d -SaveHeight=%d -CheckPartTracker=0 -SaveZoomToFit=1 "
			"-PreferenceSet=Thumbnails -SnapshotSuffix=.png",
			executable, ldrFilename, tempFilename, width, height];
//		NSLog(@"commandLine: %@", commandLine);
		system(commandLine.UTF8String);
		FILE *thumbnailFile = fopen(tempFilenameBuf, "rb");
		if (thumbnailFile != NULL)
		{
			fseek(thumbnailFile, 0, SEEK_END);
			long fileSize = ftell(thumbnailFile);
			fclose(thumbnailFile);
			if (fileSize > 0)
			{
				free(tempFilenameBuf);
				CFURLRef thumbnailUrl = CFURLCreateWithFileSystemPath(NULL, (__bridge CFStringRef)tempFilename, kCFURLPOSIXPathStyle, false);
				QLThumbnailRequestSetImageAtURL(thumbnail, thumbnailUrl, NULL);
				CFRelease(thumbnailUrl);
				return noErr;
			}
			else
			{
				unlink(tempFilenameBuf);
			}
		}
		free(tempFilenameBuf);
	}
	return -1;
}

void CancelThumbnailGeneration(void *thisInterface, QLThumbnailRequestRef thumbnail)
{
	// Implement only if supported
}
