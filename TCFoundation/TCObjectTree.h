#ifndef __TCOBJECTTREE_H__
#define __TCOBJECTTREE_H__

#include <TCFoundation/TCObject.h>

#include <stdio.h>

typedef void (*TCTreeTraversalFunc)(const void *key, TCObject *value);
typedef int (*TCTreeCompareKeyFunc)(const void *leftKey, const void *rightKey);
typedef void *(*TCTreeCopyKeyFunc)(const void *key);

typedef struct TCObjectTreeNode
{
	TCObjectTreeNode(const void *key, TCObject *value,
		TCTreeCopyKeyFunc copyKey);
	~TCObjectTreeNode(void);

	void* key;
	TCObject* value;
	struct TCObjectTreeNode* left;
	struct TCObjectTreeNode* right;
}  TCBinaryObjectTreeNode;

class TCExport TCObjectTree : public TCObject
{
public:
	TCObjectTree(TCTreeCompareKeyFunc compareKey, TCTreeCopyKeyFunc copyKey);
	virtual int setObjectForKey(TCObject *object, const void *key);
	TCObject* objectForKey(const void *key);
	virtual TCObject *copy(void) const;
	virtual void traverseTree(TCTreeTraversalFunc traversalFunc);
protected:
	virtual ~TCObjectTree(void);
	virtual void dealloc(void);
	virtual void deleteNode(TCObjectTreeNode*);
	virtual void copyNode(TCBinaryObjectTreeNode *dstNode,
		const TCBinaryObjectTreeNode *srcNode) const;
	virtual void traverseNode(TCBinaryObjectTreeNode *node,
		TCTreeTraversalFunc traversalFunc);
	virtual int removeObjectForKeyFromNode(TCBinaryObjectTreeNode *&node,
		const void *key);

	TCBinaryObjectTreeNode *rootNode;
	TCTreeCompareKeyFunc compareKey;
	TCTreeCopyKeyFunc copyKey;
	int depth;
	int count;
private:
	// UNTESTED
	virtual int removeObjectForKey(const void *key);
};

#endif // __TCOBJECTTREE_H__
