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
	close();
}

void PartListPanel::doCancel()
{
	close();
}

void PartListPanel::doUp()
{
	partlist->doUp();
}

void PartListPanel::doDown()
{
	partlist->doDown();
}

