void JpegOptionsPanel::setJpegOptions( JpegOptions * value )
{
	jpegoptions = value;
}

void JpegOptionsPanel::init()
{
}

void JpegOptionsPanel::doOk()
{
	jpegoptions->doOk();
	close();
}

void JpegOptionsPanel::doCancel()
{
	jpegoptions->doCancel();
	close();
}

void JpegOptionsPanel::doSliderMoved(int i)
{
	jpegoptions->doSliderMoved(i);
}

