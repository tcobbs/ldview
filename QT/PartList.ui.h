void PartListPanel::setPartList( PartList *value)
{
	partlist = value;
}

void PartListPanel::init()
{
}

void PartListPanel::doOk()
{
	partlist->doOk();
	accept();
}

void PartListPanel::doCancel()
{
	reject();
}

void PartListPanel::doUp()
{
	partlist->doUp();
}

void PartListPanel::doDown()
{
	partlist->doDown();
}

void PartListPanel::doHighlighted()
{
	partlist->controlDirectionButtons();
}

