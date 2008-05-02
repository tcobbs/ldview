#ifndef __TCBINARYTREE_H__
#define __TCBINARYTREE_H__

#include <TCFoundation/TCObject.h>

#include <stdio.h>

class TCStringArray;

typedef struct TCBinaryTreeNode
{
	char* string;
	struct TCBinaryTreeNode* left;
	struct TCBinaryTreeNode* right;
}  TCBinaryTreeNode;

class TCExport TCBinaryTree : public TCObject
{
	public:
		TCBinaryTree(void);
		virtual int addString(const char*);
		virtual int stringExists(const char*);
		virtual int writeFile(const char*);
		virtual int readFile(const char*);
		virtual void prettyPrint(void);
		virtual TCObject *copy(void) const;
	protected:
		virtual ~TCBinaryTree(void);
		virtual void dealloc(void);
		virtual void deleteNode(TCBinaryTreeNode*);
		virtual int compare(const char*, const char*);
		virtual TCBinaryTreeNode* newNodeWithString(const char*) const;
		virtual int writeFileForTree(FILE*, TCBinaryTreeNode*);
		virtual void addFromSortedArray(TCStringArray*, int, int);
		virtual void prettyPrintNode(TCBinaryTreeNode*, int);
		virtual void prettyPrintLine(const char*, int);
		virtual void copyNode(TCBinaryTreeNode *dstNode,
			const TCBinaryTreeNode *srcNode) const;

		TCBinaryTreeNode* rootNode;
};

#endif // __TCBINARYTREE_H__
