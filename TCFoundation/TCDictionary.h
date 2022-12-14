#ifndef __TCDICTIONARY_H__
#define __TCDICTIONARY_H__

#include <TCFoundation/TCObject.h>

class TCObjectArray;
class TCSortedStringArray;

class TCExport TCDictionary: public TCObject
{
	public:
		explicit TCDictionary(bool = true);
		virtual void setObjectForKey(TCObject*, const char*);
		/*virtual*/ TCObject* objectForKey(const char*);
		TCObjectArray *allObjects(void) { return objects; }
		TCSortedStringArray *allKeys(void) { return keys; }
		virtual bool removeObjectForKey(const char*);
		virtual void removeAll(void);
		virtual bool isCaseSensitve(void);
		virtual TCObject *copy(void) const;
	protected:
		virtual ~TCDictionary(void);
		virtual void dealloc(void);
		virtual ptrdiff_t indexOfKey(const char*);

		TCObjectArray* objects;
		TCSortedStringArray* keys;
	private:
		TCDictionary(int): objects(NULL), keys(NULL) {}
};

#endif // __TCDICTIONARY_H__
