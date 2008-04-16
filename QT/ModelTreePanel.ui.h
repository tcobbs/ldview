/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename slots use Qt Designer which will
** update this file, preserving your code. Create an init() slot in place of
** a constructor, and a destroy() slot in place of a destructor.
*****************************************************************************/

void ModelTreePanel::setModelTree(LDViewModelTree *value)
{
    modelTree = value;
}

void ModelTreePanel::empty()
{
	modelTree->empty();
}

void ModelTreePanel::unknown()
{
	modelTree->unknown();
}

void ModelTreePanel::model()
{
	modelTree->model();
}

void ModelTreePanel::line()
{
	modelTree->line();
}

void ModelTreePanel::triangle()
{
	modelTree->triangle();
}

void ModelTreePanel::conditionalLine()
{
	modelTree->conditionalLine();
}
void ModelTreePanel::quad()
{
	modelTree->quad();
}

void ModelTreePanel::comment()
{
	modelTree->comment();
}

void ModelTreePanel::itemexpanded(QListViewItem *item)
{
	modelTree->expanded(item);
}

