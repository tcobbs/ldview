#ifndef __TCSTRINGOBJECTTREE_H__
#define __TCSTRINGOBJECTTREE_H__

#include <TCFoundation/TCObjectTree.h>

class TCExport TCStringObjectTree : public TCObjectTree
{
public:
	explicit TCStringObjectTree(bool caseSensitive = true);
	virtual int setObjectForKey(TCObject *object, const char *key);
	TCObject* objectForKey(const char *key);
protected:
	virtual ~TCStringObjectTree(void);
	virtual void dealloc(void);
	virtual int setObjectForKey(TCObject *object, const void *key);
	TCObject* objectForKey(const void *key);

	bool caseSensitive;
};

#endif // __TCSTRINGOBJECTTREE_H__
