#import "Updater.h"
#import "OCLocalStrings.h"
#import "LDViewCategories.h"
#import "UpdaterAlertHandler.h"

#include <TCFoundation/TCProgressAlert.h>
#include <TCFoundation/TCStringArray.h>
#include <LDLib/LDLibraryUpdater.h>
#include <LDLib/LDUserDefaultsKeys.h>

@implementation Updater

- (id) init
{
	self = [super init];
	if (self != nil)
	{
		[NSBundle loadNibNamed:@"Updater.nib" owner:self];
	}
	return self;
}

- (void)dealloc
{
	TCObject::release(alertHandler);
	[super dealloc];
}

- (void)awakeFromNib
{
	alertHandler = new UpdaterAlertHandler(self);
}

- (bool)run:(NSString *)targetDir full:(bool)fullDownload
{
	NSString *ldrawDir = [targetDir stringByAppendingPathComponent:@"ldraw"];
	NSFileManager *fileManger = [NSFileManager defaultManager];
	BOOL isDir;

	if (![fileManger fileExistsAtPath:ldrawDir isDirectory:&isDir])
	{
		if (![fileManger createDirectoryAtPath:ldrawDir attributes:nil])
		{
			NSRunCriticalAlertPanel([OCLocalStrings get:@"ErrorCreatingLDrawFolder"], [OCLocalStrings get:@"EnsureParentFolderWriteAccess"], [OCLocalStrings get:@"OK"], nil, nil);
			return NSCancelButton;
		}
	}
	if (!isDir)
	{
		NSRunCriticalAlertPanel([OCLocalStrings get:@"FileExists"], [NSString stringWithFormat:[OCLocalStrings get:@"FileExistsFormat"], ldrawDir], [OCLocalStrings get:@"OK"], nil, nil);
		return NSCancelButton;
	}
	canceled = NO;
	done = NO;
	error = NO;
	TCObject::release(updater);
	updater = new LDLibraryUpdater;
	[progress setDoubleValue:0.0];
	[textField setStringValue:[OCLocalStrings get:@"CheckingForLibraryUpdates"]];
	updater->setLibraryUpdateKey(LAST_LIBRARY_UPDATE_KEY);
	updater->setLdrawDir([ldrawDir asciiCString]);
	if (fullDownload)
	{
		updater->installLDraw();
	}
	else
	{
		updater->checkForUpdates();
	}
	[NSApp runModalForWindow:panel];
	[panel orderOut:self];
	TCObject::release(updater);
	return done && !error && !canceled;
}

- (bool)checkForUpdates:(NSString *)targetDir
{
	return [self run:targetDir full:false];
}

- (bool)downloadLDraw:(NSString *)targetDir
{
	return [self run:targetDir full:true];
}

- (void)setProgressValue:(NSNumber *)amount
{
	[progress setDoubleValue:[amount doubleValue]];
	[amount release];
}

- (void)setMessageText:(NSString *)message
{
	[textField setStringValue:message];
	[message release];
}

- (void)switchToDone
{
	[cancelButton setEnabled:NO];
	[okButton setEnabled:YES];
}

- (void)noUpdates
{
	[textField setStringValue:[OCLocalStrings get:@"LibraryUpdateUnnecessary"]];
	[self switchToDone];
}

- (void)updateFinished
{
	[textField setStringValue:[OCLocalStrings get:@"LibraryUpdateComplete"]];
	[self switchToDone];
}

- (void)updateCanceled
{
	[textField setStringValue:[OCLocalStrings get:@"LibraryUpdateCanceled"]];
	[self switchToDone];
}

- (void)updateError
{
	[textField setStringValue:[NSString stringWithFormat:@"%@:\n%@", [OCLocalStrings get:@"LibraryUpdateError"], [NSString stringWithUCString:updater->getError()]]];
	[self switchToDone];
}

- (void)cancel
{
	[NSApp stopModalWithCode:NSOKButton];
}

- (void)progressCallback:(TCProgressAlert *)alert
{
	// NOTE: this gets called from a background thread.  That background thread
	// has not NSAutorleasePool in place, so don't do anything that would
	// trigger an autorelease (like [NSNumber numberWithDouble:].
	// Also, don't update the UI directly.  Instead, use
	// performSelectorOnMainThread:withObject:waitUntilDone:.
	if (alert && strcmp(alert->getSource(), LD_LIBRARY_UPDATER) == 0)
	{
		[self performSelectorOnMainThread:@selector(setProgressValue:) withObject:[[NSNumber alloc] initWithDouble:alert->getProgress()] waitUntilDone:NO];
		[self performSelectorOnMainThread:@selector(setMessageText:) withObject:[[NSString alloc] initWithUCString:alert->getMessageUC()] waitUntilDone:NO];
		if (alert->getProgress() == 1.0f)
		{
			if (alert->getExtraInfo())
			{
				if (strcmp((*(alert->getExtraInfo()))[0], "None") == 0)
				{
					// No updates available.
					[self performSelectorOnMainThread:@selector(noUpdates) withObject:nil waitUntilDone:YES];
				}
				else
				{
					// Success
					[self performSelectorOnMainThread:@selector(updateFinished) withObject:nil waitUntilDone:YES];
				}
			}
			else
			{
				[self performSelectorOnMainThread:@selector(updateCanceled) withObject:nil waitUntilDone:YES];
			}
			done = true;
		}
		else if (alert->getProgress() == 2.0f)
		{
			[self performSelectorOnMainThread:@selector(updateError) withObject:nil waitUntilDone:YES];
			error = YES;
		}
		if (canceled)
		{
			alert->abort();
			[self performSelectorOnMainThread:@selector(cancel) withObject:nil waitUntilDone:NO];
		}
	}
}

- (IBAction)ok:(id)sender
{
	[NSApp stopModalWithCode:NSOKButton];
}

- (IBAction)cancel:(id)sender
{
	canceled = true;
}

@end
