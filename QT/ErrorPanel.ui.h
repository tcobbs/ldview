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

void ErrorPanel::generalError()
{
    errors->doErrorClick(generalErrorButton, LDLEGeneral);
}

void ErrorPanel::parseError()
{
    errors->doErrorClick(parseErrorButton, LDLEParse);
}

void ErrorPanel::fileNotFound()
{
    errors->doErrorClick(fileNotFoundButton, LDLEFileNotFound);
}

void ErrorPanel::singularMatrix()
{
    errors->doErrorClick(singularMatrixButton, LDLEMatrix);
}
//void ErrorPanel::colorError()
//{
	// TC TODO: Get rid of this error
//    errors->doErrorClick(colorErrorButton, LDLEGeneral/*LDLEColor*/);
//}

void ErrorPanel::partDeterminant()
{
    errors->doErrorClick(partDeterminantButton, LDLEPartDeterminant);
}

void ErrorPanel::concaveQuad()
{
    errors->doErrorClick(concaveQuadButton, LDLEConcaveQuad);
}
void ErrorPanel::badVertexOrder()
{
    errors->doErrorClick(badVertexOrderButton, LDLEVertexOrder);
}

//void ErrorPanel::concaveQuadSplit()
//{
	// TC TODO: Get rid of this error
//    errors->doErrorClick(concaveQuadSplitButton, LDLEGeneral/*LDLEConcaveQuadSplit*/);
//}

void ErrorPanel::colinearPoints()
{
    errors->doErrorClick(colinearPointsButton, LDLEColinear);
}

//void ErrorPanel::openGLError()
//{
	// TC TODO: Get rid of this error
//    errors->doErrorClick(openGLErrorButton, LDLEGeneral/*LDLEOpenGL*/);
//}

void ErrorPanel::showWarnings()
{
    errors->doShowWarnings();
}

void ErrorPanel::BFCWarning()
{
    errors->doErrorClick(BFCWarningButton, LDLEBFCWarning);
}

void ErrorPanel::BFCError()
{
    errors->doErrorClick(BFCErrorButton, LDLEBFCError);
}

void ErrorPanel::nonFlatQuad()
{
    errors->doErrorClick(nonFlatQuadButton, LDLENonFlatQuad);
}

void ErrorPanel::MPDError()
{
    errors->doErrorClick(MPDErrorButton, LDLEMPDError);
}

void ErrorPanel::whitespace()
{
	errors->doErrorClick(whitespaceButton, LDLEWhitespace);
}

void ErrorPanel::identicalVertices()
{
    errors->doErrorClick(identicalVerticesButton,LDLEMatchingPoints);
}

void ErrorPanel::showAllError()
{
    errors->doShowAllError();
}

void ErrorPanel::showNoneError()
{
    errors->doShowNoneError();
}

