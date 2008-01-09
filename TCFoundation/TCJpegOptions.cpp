#include "TCJpegOptions.h"
#include "TCUserDefaults.h"

int TCJpegOptions::sm_defaultQuality = 85;
TCJpegOptions::SubSampling TCJpegOptions::sm_defaultSubSampling = SS420;
bool TCJpegOptions::sm_defaultProgressive = false;

TCJpegOptions::TCJpegOptions(bool compress /*= true*/):
TCImageOptions(compress),
m_quality(sm_defaultQuality),
m_subSampling(sm_defaultSubSampling),
m_progressive(sm_defaultProgressive)
{
#ifdef _LEAK_DEBUG
	strcpy(className, "TCJpegOptions");
#endif
	m_formatName = "JPG";
	if (m_compress)
	{
		m_quality = TCUserDefaults::longForKey("CJPGQuality", m_quality);
		m_subSampling =
			(SubSampling)TCUserDefaults::longForKey("CJPGSubSampling",
			m_subSampling);
		m_progressive = TCUserDefaults::boolForKey("CJPGProgressive",
			m_progressive);
	}
}

TCJpegOptions::~TCJpegOptions(void)
{
}

void TCJpegOptions::dealloc(void)
{
	TCImageOptions::dealloc();
}

void TCJpegOptions::save(void)
{
	if (m_compress)
	{
		TCUserDefaults::setLongForKey(m_quality, "CJPGQuality");
		TCUserDefaults::setLongForKey(m_subSampling, "CJPGSubSampling");
		TCUserDefaults::setBoolForKey(m_progressive, "CJPGProgressive");
	}
}
