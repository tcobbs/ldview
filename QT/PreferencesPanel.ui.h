/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename slots use Qt Designer which will
** update this file, preserving your code. Create an init() slot in place of
** a constructor, and a destroy() slot in place of a destructor.
*****************************************************************************/


void PreferencesPanel::doApply()
{
    if (preferences)
    {
	 preferences->doApply();
    }
}

void PreferencesPanel::setPreferences(Preferences *value )
{
    preferences = value;
}

void PreferencesPanel::init()
{
    preferences = NULL;
}

void PreferencesPanel::doOK()
{
    doApply();
    close();
}

void PreferencesPanel::doCancel()
{
    if (preferences)
    {
	 preferences->doCancel();
    }
    close();
}


void PreferencesPanel::doResetGeneral()
{
    if (preferences)
    {
        preferences->doResetGeneral();
    }
}

void PreferencesPanel::doResetGeometry()
{
    if (preferences)
    {
        preferences->doResetGeometry();
    }
}

void PreferencesPanel::doResetEffects()
{
    if (preferences)
    {
        preferences->doResetEffects();
    }
}

void PreferencesPanel::doResetPrimitives()
{
    if (preferences)
    {
        preferences->doResetPrimitives();
    }
}

void PreferencesPanel::enableApply()
{
    applyButton->setEnabled(true);
}

void PreferencesPanel::doWireframeCutaway(bool value)
{
    if (preferences)
    {
        preferences->doWireframeCutaway(value);
    }
}

void PreferencesPanel::doLighting(bool value)
{
    if (preferences)
    {
        preferences->doLighting(value);
    }
}

void PreferencesPanel::doStereo(bool value)
{
    if (preferences)
    {
        preferences->doStereo(value);
    }
}

void PreferencesPanel::doWireframe(bool value)
{
    if (preferences)
    {
        preferences->doWireframe(value);
    }
}

void PreferencesPanel::doEdgeLines(bool value)
{
    if (preferences)
    {
        preferences->doEdgeLines(value);
    }
}

void PreferencesPanel::doConditionalShow(bool value)
{
    if (preferences)
    {
        preferences->doConditionalShow(value);
	}
}

void PreferencesPanel::doPrimitiveSubstitution(bool value)
{
    if (preferences)
    {
        preferences->doPrimitiveSubstitution(value);
    }
}

void PreferencesPanel::doTextureStuds(bool value)
{
    if (preferences)
    {
        preferences->doTextureStuds(value);
    }
}
