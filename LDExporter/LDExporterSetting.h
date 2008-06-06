#ifndef __LDEXPORTERSETTING_H__
#define __LDEXPORTERSETTING_H__

#include <TCFoundation/TCStlIncludes.h>
#include <TCFoundation/mystring.h>

class LDExporterSetting
{
public:
	enum Type
	{
		TUnknown,
		TBool,
		TLong,
		TFloat,
		TString
	};
	LDExporterSetting(void) : m_type(TUnknown) {}
	LDExporterSetting(CUCSTR name, bool value, const char *key):
		m_name(name), m_key(key), m_type(TBool), m_bool(value), m_hasMin(false),
		m_hasMax(false) {}
	LDExporterSetting(CUCSTR name, long value, const char *key):
		m_name(name), m_key(key), m_type(TLong), m_long(value), m_hasMin(false),
		m_hasMax(false) {}
	LDExporterSetting(CUCSTR name, TCFloat value, const char *key):
		m_name(name), m_key(key), m_type(TFloat), m_float(value),
		m_hasMin(false), m_hasMax(false) {}
	LDExporterSetting(CUCSTR name, CUCSTR value, const char *key):
		m_name(name), m_key(key), m_type(TString), m_string(value),
		m_hasMin(false), m_hasMax(false) {}

	Type getType(void) { return m_type; }

	const ucstring &getName(void) { return m_name; }
	void setName(CUCSTR value);

	void setValue(bool value, bool commit = false);
	void setValue(long value, bool commit = false);
	void setValue(TCFloat value, bool commit = false);
	void setValue(CUCSTR value, bool commit = false);

	bool hasMin(void) { return m_hasMin; }
	bool hasMax(void) { return m_hasMax; }
	bool getBoolValue(void);
	long getLongValue(void);
	long getMinLongValue(void);
	long getMaxLongValue(void);
	void setMinLongValue(long value);
	void setMaxLongValue(long value);
	TCFloat getFloatValue(void);
	TCFloat getMinFloatValue(void);
	TCFloat getMaxFloatValue(void);
	void setMinFloatValue(TCFloat value);
	void setMaxFloatValue(TCFloat value);
	const ucstring &getStringValue(void);
protected:
	ucstring m_name;
	std::string m_key;
	Type m_type;
	bool m_bool;
	long m_long;
	TCFloat m_float;
	ucstring m_string;
	bool m_hasMin;
	bool m_hasMax;
	long m_minLong;
	long m_maxLong;
	TCFloat m_minFloat;
	TCFloat m_maxFloat;
};

#endif // __LDEXPORTERSETTING_H__
