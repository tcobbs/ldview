void ExtraDirPanel::doCancel()
{
	close();
}
void ExtraDirPanel::doOk()
{
	extradir->doOk();
	close();
}

void ExtraDirPanel::setExtraDir(ExtraDir *value)
{
	extradir=value;
}

void ExtraDirPanel::doAddExtraDir()
{
	extradir->doAddExtraDir();
}

void ExtraDirPanel::doDelExtraDir()
{
	extradir->doDelExtraDir();
}

void ExtraDirPanel::doUpExtraDir()
{
	extradir->doUpExtraDir();
}

void ExtraDirPanel::doDownExtraDir()
{
	extradir->doDownExtraDir();
}

void ExtraDirPanel::doExtraDirSelected()
{
	extradir->doExtraDirSelected();
}
