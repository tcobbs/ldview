#ifndef __LDLPALETTE_H__
#define __LDLPALETTE_H__

#include <TCFoundation/TCObject.h>
#include <TCFoundation/TCTypedObjectArray.h>

typedef struct
{
	TCByte r, g, b, a;
} LDLColor;

typedef struct
{
	LDLColor color;
	LDLColor ditherColor;
	int edgeColorNumber;
	float specular[4];
	float shininess;
} LDLColorInfo;

class LDLPalette : public TCObject
{
public:
	LDLPalette(void);
	LDLPalette(const LDLPalette &other);
	void reset(void);
	void getRGBA(int colorNumber, int &r, int &g, int &b, int &a);
	int getEdgeColorNumber(int colorNumber);
	bool isColorComment(const char *comment);
	void parseColorComment(const char *comment);
	LDLColorInfo &getColorInfo(int index) { return m_colors[index]; }
	virtual int getColorNumberForRGB(TCByte r, TCByte g, TCByte b);
protected:
	virtual ~LDLPalette(void);
	virtual void dealloc(void);
	void init(void);
	void initStandardColors(void);
	void initDitherColors(void);
	void initSpecular(int index, float sr, float sg, float sb, float sa,
		float shininess);
	void initOtherColors(void);
	void initOtherColor(int index, TCByte r, TCByte g, TCByte b,
		TCByte a = 255);
	void parseLDLiteColorComment(const char *comment);
	void initSpecularAndShininess(LDLColorInfo &color);
	void getRGBA(const LDLColorInfo &colorInfo, int &r, int &g, int &b, int &a);
	bool getCustomColorRGBA(int colorNumber, int &r, int &g, int &b, int &a);
	int getBlendedColorComponent(TCULong c1, TCULong c2, TCULong a1,
		TCULong a2);
	virtual bool isColorNumberRGB(int colorNumber, TCByte r, TCByte g,
		TCByte b);

	class CustomColor : public TCObject
	{
	public:
		int colorNumber;
		LDLColorInfo colorInfo;
	};

	LDLColorInfo m_colors[512];
	TCTypedObjectArray<CustomColor> *customColors;
};

#endif __LDLPALETTE_H__
