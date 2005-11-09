#include "TCStringObjectTree.h"
#include "mystring.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *copyStringFunc(const char *string)
{
	return copyString(string);
}

TCStringObjectTree::TCStringObjectTree(bool caseSensitive)
	:TCObjectTree(NULL, (TCTreeCopyKeyFunc)copyStringFunc),
	caseSensitive(caseSensitive)
{
#ifdef _LEAK_DEBUG
	strcpy(className, "TCStringObjectTree");
#endif
	if (caseSensitive)
	{
		compareKey = (TCTreeCompareKeyFunc)strcmp;
	}
	else
	{
		compareKey = (TCTreeCompareKeyFunc)strcasecmp;
	}
}

TCStringObjectTree::~TCStringObjectTree(void)
{
}

void TCStringObjectTree::dealloc(void)
{
	TCObjectTree::dealloc();
}

int TCStringObjectTree::setObjectForKey(TCObject *object, const char* key)
{
	return TCObjectTree::setObjectForKey(object, key);
}

int TCStringObjectTree::setObjectForKey(TCObject *object, const void* key)
{
	return TCObjectTree::setObjectForKey(object, key);
}

TCObject *TCStringObjectTree::objectForKey(const void *key)
{
	return TCObjectTree::objectForKey(key);
}

TCObject *TCStringObjectTree::objectForKey(const char *key)
{
	return TCObjectTree::objectForKey(key);
}
