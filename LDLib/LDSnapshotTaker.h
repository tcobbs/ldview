#ifndef __LDSNAPSHOTTAKER_H__
#define __LDSNAPSHOTTAKER_H__

#include <TCFoundation/TCObject.h>
#include <TCFoundation/TCStlIncludes.h>
#include <TCFoundation/TCImage.h>

class LDrawModelViewer;

class LDSnapshotTaker : public TCObject
{
public:
	enum ImageType
	{
		ITPng = 1,
		ITBmp = 2
	};
	LDSnapshotTaker(void);
	LDSnapshotTaker(LDrawModelViewer *modelViewer);
	LDrawModelViewer *getModelViewer(void) { return m_modelViewer; }
	void setImageType(ImageType value) { m_imageType = value; }
	ImageType getImageType(void) const { return m_imageType; }
	void setTrySaveAlpha(bool value) { m_trySaveAlpha = value; }
	bool getTrySaveAlpha(void) const { return m_trySaveAlpha; }
	void setAutoCrop(bool value) { m_autoCrop = value; }
	bool getAutoCrop(void) const { return m_autoCrop; }
	void setProductVersion(const std::string &value)
	{
		m_productVersion = value;
	}
	const std::string &getProductVersion(void) const
	{
		return m_productVersion;
	}
	void calcTiling(int desiredWidth, int desiredHeight, int &bitmapWidth,
		int &bitmapHeight, int &numXTiles, int &numYTiles);

	bool saveImage(const char *filename, int imageWidth, int imageHeight,
		bool zoomToFit);
	bool saveImage(void);
	static bool doCommandLine(void);

	static const char *alertClass(void) { return "LDSnapshotTaker"; }
protected:
	virtual ~LDSnapshotTaker(void);
	virtual void dealloc(void);
	bool writeBmp(const char *filename, int width, int height, TCByte *buffer);
	bool writePng(const char *filename, int width, int height, TCByte *buffer,
		bool saveAlpha);
	bool writeImage(const char *filename, int width, int height, TCByte *buffer,
		char *formatName, bool saveAlpha);
	TCByte *grabImage(int &imageWidth, int &imageHeight, bool zoomToFit,
		TCByte *buffer, bool *saveAlpha);
	bool canSaveAlpha(void);
	void renderOffscreenImage(void);
	bool imageProgressCallback(CUCSTR message, float progress);
	bool shouldZoomToFit(bool zoomToFit);

	static bool staticImageProgressCallback(CUCSTR message, float progress,
		void* userData);

	LDrawModelViewer *m_modelViewer;
	ImageType m_imageType;
	std::string m_productVersion;
	bool m_trySaveAlpha;
	bool m_autoCrop;
	bool m_fromCommandLine;
};

#endif // __LDSNAPSHOTTAKER_H__
