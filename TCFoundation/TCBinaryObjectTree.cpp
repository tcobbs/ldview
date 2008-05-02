#include "TCBinaryObjectTree.h"
#include "TCStringArray.h"
#include "mystring.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG

TCBinaryObjectTree::TCBinaryObjectTree(bool caseSensitive)
	:rootNode(NULL),
	caseSensitive(caseSensitive),
	depth(0),
	count(0)
{
#ifdef _LEAK_DEBUG
	strcpy(className, "TCBinaryObjectTree");
#endif
}

TCBinaryObjectTree::~TCBinaryObjectTree(void)
{
}

TCBinaryObjectTreeNode::TCBinaryObjectTreeNode(const char *key, TCObject *value)
	:key(copyString(key)),
	value(value->retain()),
	left(NULL),
	right(NULL)
{
}

TCBinaryObjectTreeNode::~TCBinaryObjectTreeNode(void)
{
	delete key;
	if (value)
	{
		value->release();
	}
	delete left;
	delete right;
}

void TCBinaryObjectTree::dealloc(void)
{
	deleteNode(rootNode);
	TCObject::dealloc();
}

void TCBinaryObjectTree::deleteNode(TCBinaryObjectTreeNode* node)
{
	delete node;
	node = NULL;
/*
	if (node)
	{
		deleteNode(node->left);
		deleteNode(node->right);
		if (node->string)
		{
			delete node->string;
		}
		if (node->value)
		{
			node->value->release();
		}
		delete node;
	}
*/
}

int TCBinaryObjectTree::compare(const char* s1, const char* s2)
{
	if (caseSensitive)
	{
		return strcmp(s1, s2);
	}
	else
	{
		return strcasecmp(s1, s2);
	}
}

int TCBinaryObjectTree::countNodes(TCBinaryObjectTreeNode *node)
{
	if (node)
	{
		return countNodes(node->left) + countNodes(node->right) + 1;
	}
	else
	{
		return 0;
	}
}

TCBinaryObjectTreeNode *TCBinaryObjectTree::findNodeParent(
		TCBinaryObjectTreeNode *node, TCBinaryObjectTreeNode *child)
{
	TCBinaryObjectTreeNode* spot = node;
	char *key = child->key;

	while (spot)
	{
		int result = compare(key, spot->key);

		if (result < 0)
		{
			if (spot->left == child)
			{
				return spot;
			}
			else
			{
				spot = spot->left;
			}
		}
		else if (result > 0)
		{
			if (spot->right == child)
			{
				return spot;
			}
			else
			{
				spot = spot->right;
			}
		}
		else
		{
			return NULL;
		}
	}
	return NULL;
}

TCBinaryObjectTreeNode *TCBinaryObjectTree::findNodeN(
	TCBinaryObjectTreeNode *node, int &n)
{
	if (n == 0)
	{
		return node;
	}
	else if (node)
	{
		TCBinaryObjectTreeNode *foundNode = findNodeN(node->left, n);

		if (foundNode)
		{
			return foundNode;
		}
		else
		{
			n--;
			if (n == 0)
			{
				return node;
			}
			return findNodeN(node->right, n);
		}
	}
	else
	{
		return NULL;
	}
}

TCBinaryObjectTreeNode *TCBinaryObjectTree::findMiddleNode(
	TCBinaryObjectTreeNode *node)
{
	if (node)
	{
		int middleNodeIndex = (countNodes(node) - 1) / 2;

		return findNodeN(node, middleNodeIndex);
	}
	else
	{
		return NULL;
	}
}

void TCBinaryObjectTree::balanceNode(TCBinaryObjectTreeNode *&node)
{
	if (node)
	{
		TCBinaryObjectTreeNode *middleNode = findMiddleNode(node);

		if (middleNode != node)
		{
			TCBinaryObjectTreeNode *middleNodeParent = findNodeParent(node,
				middleNode);

			if (middleNodeParent)
			{
				TCBinaryObjectTreeNode *spot = middleNode;

				if (middleNode == middleNodeParent->left)
				{
					while (spot->right)
					{
						spot = spot->right;
					}
					spot->right = node;
					node = middleNode;
					middleNodeParent->left = NULL;
				}
				else
				{
				}
			}
		}
		balanceNode(node->left);
		balanceNode(node->right);
	}
}

void TCBinaryObjectTree::balance(void)
{
	balanceNode(rootNode);
}

int TCBinaryObjectTree::setObjectForKey(TCObject *object, const char* key)
{
	if (rootNode)
	{
		TCBinaryObjectTreeNode* spot = rootNode;
		int currentDepth = 0;

		while (1)
		{
			int result = compare(key, spot->key);

			currentDepth++;
			if (result < 0)
			{
				if (spot->left)
				{
					spot = spot->left;
				}
				else
				{
					spot->left = new TCBinaryObjectTreeNode(key, object);
					count++;
					break;
				}
			}
			else if (result > 0)
			{
				if (spot->right)
				{
					spot = spot->right;
				}
				else
				{
					spot->right = new TCBinaryObjectTreeNode(key, object);
					count++;
					break;
				}
			}
			else
			{
				TCObject *oldObject = spot->value;

				// already exists, so replace, and return 0 to indicate this.
				spot->value = object->retain();
				oldObject->release();
				return 0;
			}
		}
		if (currentDepth > depth)
		{
			depth = currentDepth;
		}
	}
	else
	{
		rootNode = new TCBinaryObjectTreeNode(key, object);
		depth = 1;
		count = 1;
	}
	return 1;
}

TCObject *TCBinaryObjectTree::objectForKey(const char *key)
{
	TCBinaryObjectTreeNode* spot = rootNode;

	while (spot)
	{
		int result = compare(key, spot->key);

		if (result < 0)
		{
			spot = spot->left;
		}
		else if (result > 0)
		{
			spot = spot->right;
		}
		else
		{
			return spot->value;
		}
	}
	return NULL;
}

void TCBinaryObjectTree::copyNode(
	TCBinaryObjectTreeNode *dstNode,
	const TCBinaryObjectTreeNode *srcNode) const
{
	if (srcNode->left)
	{
		dstNode->left = new TCBinaryObjectTreeNode(srcNode->left->key,
			srcNode->left->value);
		copyNode(dstNode->left, srcNode->left);
	}
	if (srcNode->right)
	{
		dstNode->right = new TCBinaryObjectTreeNode(srcNode->right->key,
			srcNode->right->value);
		copyNode(dstNode->right, srcNode->right);
	}
}

TCObject *TCBinaryObjectTree::copy(void) const
{
	TCBinaryObjectTree *newBinaryTree = new TCBinaryObjectTree;

	if (rootNode)
	{
		newBinaryTree->rootNode = new TCBinaryObjectTreeNode(rootNode->key,
			rootNode->value);
		copyNode(newBinaryTree->rootNode, rootNode);
	}
	newBinaryTree->depth = depth;
	newBinaryTree->count = count;
	newBinaryTree->caseSensitive = caseSensitive;
	return newBinaryTree;
}

void TCBinaryObjectTree::traverseNode(TCBinaryObjectTreeNode *node,
									  TCTraversalFunc traversalFunc)
{
	if (node)
	{
		traverseNode(node->left, traversalFunc);
		traversalFunc(node->key, node->value);
		traverseNode(node->right, traversalFunc);
	}
}

void TCBinaryObjectTree::traverseTree(TCTraversalFunc traversalFunc)
{
	traverseNode(rootNode, traversalFunc);
}

int TCBinaryObjectTree::removeObjectForKeyFromNode(TCBinaryObjectTreeNode *&node,
												   const char *key)
{
	if (node)
	{
		int result = compare(key, node->key);

		if (result == 0)
		{
			TCBinaryObjectTreeNode *oldNode = node;

			if (node->left)
			{
				node = node->left;
				if (node->right)
				{
					TCBinaryObjectTreeNode *spot = node->left;

					while (spot->right)
					{
						spot = spot->right;
					}
					spot->right = oldNode->right;
				}
			}
			else
			{
				node = node->right;
			}
			oldNode->left = NULL;
			oldNode->right = NULL;
			delete oldNode;
			return 1;
		}
		else if (result < 0)
		{
			return removeObjectForKeyFromNode(node->left, key);
		}
		else
		{
			return removeObjectForKeyFromNode(node->right, key);
		}
	}
	else
	{
		return 0;
	}
}

// UNTESTED!!!
int TCBinaryObjectTree::removeObjectForKey(const char *key)
{
	return removeObjectForKeyFromNode(rootNode, key);
}
