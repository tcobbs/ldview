/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename slots use Qt Designer which will
** update this file, preserving your code. Create an init() slot in place of
** a constructor, and a destroy() slot in place of a destructor.
*****************************************************************************/

void ErrorPanel::setErrors(LDViewErrors *value)
{
    errors = value;
}

void ErrorPanel::parseError()
{
    errors->doErrorClick(parseErrorButton, LDLEParse);
}

void ErrorPanel::fileNotFound()
{
    errors->doErrorClick(fileNotFoundButton, LDLEFileNotFound);
}


void ErrorPanel::colorError()
{
	// TC TODO: Get rid of this error
    errors->doErrorClick(colorErrorButton, LDLEGeneral/*LDLEColor*/);
}

void ErrorPanel::partDeterminant()
{
    errors->doErrorClick(partDeterminantButton, LDLEPartDeterminant);
}

void ErrorPanel::concaveQuad()
{
    errors->doErrorClick(concaveQuadButton, LDLEConcaveQuad);
}

void ErrorPanel::concaveQuadSplit()
{
	// TC TODO: Get rid of this error
    errors->doErrorClick(concaveQuadSplitButton, LDLEGeneral/*LDLEConcaveQuadSplit*/);
}

void ErrorPanel::colinearPoints()
{
    errors->doErrorClick(colinearPointsButton, LDLEColinear);
}

void ErrorPanel::openGLError()
{
	// TC TODO: Get rid of this error
    errors->doErrorClick(openGLErrorButton, LDLEGeneral/*LDLEOpenGL*/);
}
