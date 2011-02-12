#include "TCBinaryTree.h"
#include "TCStringArray.h"
#include "mystring.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG

TCBinaryTree::TCBinaryTree(void)
				 :rootNode(NULL)
{
#ifdef _LEAK_DEBUG
	strcpy(className, "TCBinaryTree");
#endif
}

TCBinaryTree::~TCBinaryTree(void)
{
}

void TCBinaryTree::dealloc(void)
{
	deleteNode(rootNode);
	TCObject::dealloc();
}

void TCBinaryTree::deleteNode(TCBinaryTreeNode* node)
{
	if (node)
	{
		deleteNode(node->left);
		deleteNode(node->right);
		if (node->string)
		{
			delete node->string;
		}
		delete node;
	}
}

TCBinaryTreeNode* TCBinaryTree::newNodeWithString(const char* string) const
{
	TCBinaryTreeNode* newNode = new TCBinaryTreeNode;

	newNode->string = copyString(string);
	newNode->left = NULL;
	newNode->right = NULL;
	return newNode;
}

int TCBinaryTree::compare(const char* s1, const char* s2)
{
	return strcmp(s1, s2);
}

int TCBinaryTree::addString(const char* string)
{
	if (rootNode)
	{
		TCBinaryTreeNode* spot = rootNode;

		while (1)
		{
			int result = compare(string, spot->string);

			if (result < 0)
			{
				if (spot->left)
				{
					spot = spot->left;
				}
				else
				{
					spot->left = newNodeWithString(string);
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
					spot->right = newNodeWithString(string);
					break;
				}
			}
			else
			{
				// already exists, so don't insert, and return 0 to indicate this.
				return 0;
			}
		}
	}
	else
	{
		rootNode = newNodeWithString(string);
	}
	return 1;
}

int TCBinaryTree::stringExists(const char* string)
{
	TCBinaryTreeNode* spot = rootNode;

	while (spot)
	{
		int result = compare(string, spot->string);

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
			return 1;
		}
	}
	return 0;
}

int TCBinaryTree::writeFileForTree(FILE* file, TCBinaryTreeNode* node)
{
	if (node)
	{
		if (node->left)
		{
			if (!writeFileForTree(file, node->left))
			{
				return 0;
			}
		}
		fprintf(file, "%s\n", node->string);
		if (node->right)
		{
			if (!writeFileForTree(file, node->right))
			{
				return 0;
			}
		}
		return 1;
	}
	else
	{
		return 1;
	}
}

int TCBinaryTree::writeFile(const char* filename)
{
	char* tmpFilename = new char[strlen(filename) + 16];
	FILE* file;
	
	sprintf(tmpFilename, "%s.ttmp", filename);
	file = fopen(tmpFilename, "w");

	if (file)
	{
		int result = writeFileForTree(file, rootNode);

		fclose(file);
		if (result)
		{
			rename(tmpFilename, filename);
		}
		delete tmpFilename;
		return result;
	}
	else
	{
		delete tmpFilename;
		return 0;
	}
}

int TCBinaryTree::readFile(const char* filename)
{
	TCStringArray* lines = new TCStringArray;

//	fprintf(stderr, "About to Read.\n");
//	sleep(5);
	if (lines->readFile(filename))
	{
//		fprintf(stderr, "Done Reading File.\n");
//		sleep(5);
		addFromSortedArray(lines, 0, lines->getCount());
//		fprintf(stderr, "Done Creating tree.\n");
//		sleep(5);
		lines->release();
//		fprintf(stderr, "Done Freeing memory.\n");
//		sleep(5);
		return 1;
	}
	else
	{
		return 0;
	}
}

void TCBinaryTree::addFromSortedArray(TCStringArray* array, int start, int end)
{
	if (end - start > 0)
	{
		int middle = (start + end) / 2;

		addString((*array)[middle]);
		addFromSortedArray(array, start, middle);
		addFromSortedArray(array, middle + 1, end);
	}
}

void TCBinaryTree::prettyPrintLine(const char* line, int level)
{
	char buf[10240];
	size_t len;

	memset(buf, ' ', level);
	buf[level] = '<';
	len = strlen(line);
	memcpy(buf + level + 1, line, len);
	len += level;
	buf[len + 1] = '>';
	buf[len + 2] = '\n';
	fwrite(buf, len + 3, 1, stdout);
}

void TCBinaryTree::prettyPrintNode(TCBinaryTreeNode* node, int level)
{
	if (node)
	{
		if (node->left)
		{
			prettyPrintNode(node->left, level + 1);
		}
		else if (node->right)
		{
			prettyPrintLine("", level + 1);
		}
		prettyPrintLine(node->string, level);
		if (node->right)
		{
			prettyPrintNode(node->right, level + 1);
		}
		else if (node->left)
		{
			prettyPrintLine("", level + 1);
		}
	}
}

void TCBinaryTree::prettyPrint(void)
{
	prettyPrintNode(rootNode, 0);
}

void TCBinaryTree::copyNode(
	TCBinaryTreeNode *dstNode,
	const TCBinaryTreeNode *srcNode) const
{
	if (srcNode->left)
	{
		dstNode->left = newNodeWithString(srcNode->left->string);
		copyNode(dstNode->left, srcNode->left);
	}
	if (srcNode->right)
	{
		dstNode->right = newNodeWithString(srcNode->right->string);
		copyNode(dstNode->right, srcNode->right);
	}
}

TCObject *TCBinaryTree::copy(void) const
{
	TCBinaryTree *newBinaryTree = new TCBinaryTree;

	if (rootNode)
	{
		newBinaryTree->rootNode = newNodeWithString(rootNode->string);
		copyNode(newBinaryTree->rootNode, rootNode);
	}
	return newBinaryTree;
}
