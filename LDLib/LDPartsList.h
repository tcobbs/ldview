#ifndef __LDPARTSLIST_H__
#define __LDPARTSLIST_H__

#include <TCFoundation/TCObject.h>
#include <TCFoundation/TCStlIncludes.h>
#include <string.h>
#include <LDLib/LDPartCount.h>

typedef std::map<std::string, LDPartCount> LDPartCountMap;
typedef std::vector<LDPartCount> LDPartCountVector;

class LDLModel;

class LDPartsList : public TCObject
{
public:
	LDPartsList(void);
	virtual void scanModel(LDLModel *model);
	virtual const LDPartCountVector &getPartCounts(void)
	{
		return m_partCounts;
	}
protected:
	virtual ~LDPartsList(void);
	virtual void dealloc(void);
	virtual void scanSubModel(LDLModel *model);

	LDPartCountMap m_partCountMap;
	LDPartCountVector m_partCounts;
};

#endif // __LDPARTSLIST_H__
