void SnapshotSettingsPanel::setSnapshotSettings( SnapshotSettings * value )
{
	snapshotsettings = value;
	doEnabledSeries();
	doEnabledSize();
}


void SnapshotSettingsPanel::init()
{

}


void SnapshotSettingsPanel::doOk()
{
	snapshotsettings->doOk();
	close();
}


void SnapshotSettingsPanel::doCancel()
{
	snapshotsettings->doCancel();
	close();
}


void SnapshotSettingsPanel::doEnabledSeries()
{
	digitBox->setEnabled(seriesEnabledButton->isChecked());
	digitLabel->setEnabled(seriesEnabledButton->isChecked());
}


void SnapshotSettingsPanel::doEnabledSize()
{
	widthBox->setEnabled(sizeEnabledButton->isChecked());
	heightBox->setEnabled(sizeEnabledButton->isChecked());
    widthLabel->setEnabled(sizeEnabledButton->isChecked());
    heightLabel->setEnabled(sizeEnabledButton->isChecked());
}
