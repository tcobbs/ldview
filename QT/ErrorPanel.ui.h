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
    errors->doErrorClick(parseErrorButton, LDMEParse);
}

void ErrorPanel::fileNotFound()
{
    errors->doErrorClick(fileNotFoundButton, LDMEFileNotFound);
}


void ErrorPanel::colorError()
{
    errors->doErrorClick(colorErrorButton, LDMEColor);
}

void ErrorPanel::partDeterminant()
{
    errors->doErrorClick(partDeterminantButton, LDMEPartDeterminant);
}

void ErrorPanel::concaveQuad()
{
    errors->doErrorClick(concaveQuadButton, LDMEConcaveQuad);
}

void ErrorPanel::concaveQuadSplit()
{
    errors->doErrorClick(concaveQuadSplitButton, LDMEConcaveQuadSplit);
}

void ErrorPanel::colinearPoints()
{
    errors->doErrorClick(colinearPointsButton, LDMEColinear);
}

void ErrorPanel::openGLError()
{
    errors->doErrorClick(openGLErrorButton, LDMEOpenGL);
}
