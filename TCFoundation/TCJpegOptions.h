#ifndef __TCJPEGOPTIONS_H__
#define __TCJPEGOPTIONS_H__

#include <TCFoundation/TCImageOptions.h>

class TCExport TCJpegOptions : public TCImageOptions
{
public:
	enum SubSampling
	{
		SS420,
		SS422,
		SS444,
	};

	TCJpegOptions(bool compress = true);

	void setQuality(int value) { m_quality = value; }
	int getQuality(void) const { return m_quality; }
	static void setDefaultQuality(int value) { sm_defaultQuality = value; }

	void setSubSampling(SubSampling value) { m_subSampling = value; }
	SubSampling getSubSampling(void) const { return m_subSampling; }
	static void setDefaultSubSampling(SubSampling value)
	{
		sm_defaultSubSampling = value;
	}

	void setProgressive(bool value) { m_progressive = value; }
	bool getProgressive(void) const { return m_progressive; }
	static void setDefaultProgressive(bool value)
	{
		sm_defaultProgressive = value;
	}

	virtual void save(void);

protected:
	virtual ~TCJpegOptions(void);
	virtual void dealloc(void);

	int m_quality;
	SubSampling m_subSampling;
	bool m_progressive;

	static int sm_defaultQuality;
	static SubSampling sm_defaultSubSampling;
	static bool sm_defaultProgressive;
};

#endif // __TCJPEGOPTIONS_H__
