#include "TCObjectTree.h"
#include "TCStringArray.h"

#ifdef WIN32
#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG
#endif // WIN32

TCObjectTreeNode::TCObjectTreeNode(const void *key, TCObject *value,
											   TCTreeCopyKeyFunc copyKey)
	:key(copyKey(key)),
	value(value->retain()),
	left(NULL),
	right(NULL)
{
}

TCObjectTreeNode::~TCObjectTreeNode(void)
{
	delete (char*)key;
	if (value)
	{
		value->release();
	}
	delete left;
	delete right;
}

TCObjectTree::TCObjectTree(TCTreeCompareKeyFunc compareKey,
						   TCTreeCopyKeyFunc copyKey)
	:rootNode(NULL),
	compareKey(compareKey),
	copyKey(copyKey),
	depth(0),
	count(0)
{
#ifdef _LEAK_DEBUG
	strcpy(className, "TCObjectTree");
#endif
}

TCObjectTree::~TCObjectTree(void)
{
}

void TCObjectTree::dealloc(void)
{
	deleteNode(rootNode);
	TCObject::dealloc();
}

void TCObjectTree::deleteNode(TCObjectTreeNode* node)
{
	delete node;
	node = NULL;
}

int TCObjectTree::setObjectForKey(TCObject *object, const void* key)
{
	if (rootNode)
	{
		TCObjectTreeNode* spot = rootNode;
		int currentDepth = 0;

		while (1)
		{
			int result = compareKey(key, spot->key);

			currentDepth++;
			if (result < 0)
			{
				if (spot->left)
				{
					spot = spot->left;
				}
				else
				{
					spot->left = new TCObjectTreeNode(key, object, copyKey);
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
					spot->right = new TCObjectTreeNode(key, object, copyKey);
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
		rootNode = new TCObjectTreeNode(key, object, copyKey);
		depth = 1;
		count = 1;
	}
	return 1;
}

TCObject *TCObjectTree::objectForKey(const void *key)
{
	TCObjectTreeNode* spot = rootNode;

	while (spot)
	{
		int result = compareKey(key, spot->key);

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

void TCObjectTree::copyNode(
	TCObjectTreeNode *dstNode,
	const TCObjectTreeNode *srcNode) const
{
	if (srcNode->left)
	{
		dstNode->left = new TCObjectTreeNode(srcNode->left->key,
			srcNode->left->value, copyKey);
		copyNode(dstNode->left, srcNode->left);
	}
	if (srcNode->right)
	{
		dstNode->right = new TCObjectTreeNode(srcNode->right->key,
			srcNode->right->value, copyKey);
		copyNode(dstNode->right, srcNode->right);
	}
}

TCObject *TCObjectTree::copy(void) const
{
	TCObjectTree *newBinaryTree = new TCObjectTree(compareKey, copyKey);

	if (rootNode)
	{
		newBinaryTree->rootNode = new TCObjectTreeNode(rootNode->key,
			rootNode->value, copyKey);
		copyNode(newBinaryTree->rootNode, rootNode);
	}
	newBinaryTree->depth = depth;
	newBinaryTree->count = count;
	return newBinaryTree;
}

void TCObjectTree::traverseNode(TCObjectTreeNode *node,
								TCTreeTraversalFunc traversalFunc)
{
	if (node)
	{
		traverseNode(node->left, traversalFunc);
		traversalFunc(node->key, node->value);
		traverseNode(node->right, traversalFunc);
	}
}

void TCObjectTree::traverseTree(TCTreeTraversalFunc traversalFunc)
{
	traverseNode(rootNode, traversalFunc);
}

int TCObjectTree::removeObjectForKeyFromNode(TCObjectTreeNode *&node,
											 const void *key)
{
	if (node)
	{
		int result = compareKey(key, node->key);

		if (result == 0)
		{
			TCObjectTreeNode *oldNode = node;

			if (node->left)
			{
				node = node->left;
				if (node->right)
				{
					TCObjectTreeNode *spot = node->left;

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
int TCObjectTree::removeObjectForKey(const void *key)
{
	return removeObjectForKeyFromNode(rootNode, key);
}
