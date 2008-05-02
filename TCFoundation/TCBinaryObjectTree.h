#ifndef __TCBINARYOBJECTTREE_H__
#define __TCBINARYOBJECTTREE_H__

#include <TCFoundation/TCObject.h>

#include <stdio.h>

typedef struct TCBinaryObjectTreeNode
{
	TCBinaryObjectTreeNode(const char *key, TCObject *value);
	~TCBinaryObjectTreeNode(void);
	char* key;
	TCObject* value;
	struct TCBinaryObjectTreeNode* left;
	struct TCBinaryObjectTreeNode* right;
}  TCBinaryObjectTreeNode;

typedef void (*TCTraversalFunc)(const char *key, TCObject *value);

class TCExport TCBinaryObjectTree : public TCObject
{
public:
	TCBinaryObjectTree(bool caseSensitive = 1);
	virtual int setObjectForKey(TCObject *object, const char *key);
	TCObject* objectForKey(const char *key);
	virtual int isCaseSensitve(void) { return caseSensitive; }
	virtual TCObject *copy(void) const;
	virtual void traverseTree(TCTraversalFunc traversalFunc);
	virtual void balance(void);
protected:
	virtual ~TCBinaryObjectTree(void);
	virtual void dealloc(void);
	virtual void deleteNode(TCBinaryObjectTreeNode*);
	virtual int compare(const char*, const char*);
	virtual void copyNode(TCBinaryObjectTreeNode *dstNode,
		const TCBinaryObjectTreeNode *srcNode) const;
	virtual void traverseNode(TCBinaryObjectTreeNode *node,
		TCTraversalFunc traversalFunc);
	virtual int removeObjectForKeyFromNode(TCBinaryObjectTreeNode *&node,
		const char *key);
	virtual void balanceNode(TCBinaryObjectTreeNode *&node);
	virtual TCBinaryObjectTreeNode *findMiddleNode(
		TCBinaryObjectTreeNode *node);
	virtual int countNodes(TCBinaryObjectTreeNode *node);
	virtual TCBinaryObjectTreeNode *findNodeN(TCBinaryObjectTreeNode *node,
		int &n);
	virtual TCBinaryObjectTreeNode *findNodeParent(
		TCBinaryObjectTreeNode *node, TCBinaryObjectTreeNode *child);

	TCBinaryObjectTreeNode* rootNode;
	bool caseSensitive;
	int depth;
	int count;
private:
	// UNTESTED
	virtual int removeObjectForKey(const char *key);
};

#endif // __TCBINARYOBJECTTREE_H__
