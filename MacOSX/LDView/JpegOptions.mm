#import "JpegOptions.h"
#include <TCFoundation/TCJpegOptions.h>

@implementation JpegOptions

- (id)init
{
	self = [super init];
	if (self != nil)
	{
		options = new TCJpegOptions;
		[NSBundle loadNibNamed:@"JpegOptions.nib" owner:self];
	}
	return self;
}

- (void)dealloc
{
	[panel release];
	TCObject::release(options);
	[super dealloc];
}

- (void)awakeFromNib
{
	int index = 0;
	int state;

	[qualitySlider setIntValue:options->getQuality()];
	[qualityField setIntValue:options->getQuality()];
	switch (options->getSubSampling())
	{
		case TCJpegOptions::SS444:
			index = 0;
			break;
		case TCJpegOptions::SS422:
			index = 1;
			break;
		case TCJpegOptions::SS420:
			index = 2;
			break;
	}
	[subSamplingPopUp selectItemAtIndex:index];
	if (options->getProgressive())
	{
		state = NSOnState;
	}
	else
	{
		state = NSOffState;
	}
	[progressiveButton setState:state];
}

- (IBAction)cancel:(id)sender
{
	[panel orderOut:self];
	[NSApp stopModalWithCode:NSCancelButton];
}

- (IBAction)ok:(id)sender
{
	[panel orderOut:self];
	[NSApp stopModalWithCode:NSOKButton];
}

- (void)runModal
{
	if ([NSApp runModalForWindow:panel] == NSOKButton)
	{
		TCJpegOptions::SubSampling subSampling = TCJpegOptions::SS444;

		switch ([[subSamplingPopUp selectedItem] tag])
		{
			case 0:
				subSampling = TCJpegOptions::SS444;
				break;
			case 1:
				subSampling = TCJpegOptions::SS422;
				break;
			case 2:
				subSampling = TCJpegOptions::SS420;
				break;
		}
		options->setQuality([qualitySlider intValue]);
		options->setSubSampling(subSampling);
		options->setProgressive([progressiveButton state] == NSOnState);
		options->save();
	}
}

@end
