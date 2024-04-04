#include "UnMirrorStuds.h"
#include <TCFoundation/TCVector.h>
#include <TCFoundation/mystring.h>
#include <LDLoader/LDLMainModel.h>
#include <LDLoader/LDLModelLine.h>
#include <TCFoundation/TCAlertManager.h>

#ifdef WIN32
#include <direct.h>
#else // WIN32
#include <unistd.h>
#endif // WIN32

UnMirrorStuds::UnMirrorStuds(void)
	:m_delete(false),
	m_overwrite(true),
	m_verbose(false),
	m_abort(false),
	m_optionsDone(false),
	m_fileProcessed(false),
	m_deleteCount(0),
	m_newFileCount(0)
{
}

UnMirrorStuds::~UnMirrorStuds(void)
{
}

void UnMirrorStuds::dealloc(void)
{
	TCObject::dealloc();
}

bool UnMirrorStuds::checkOption(const char *option)
{
	if (m_optionsDone)
	{
		return false;
	}
	else if (stricmp(option, "-s") == 0)
	{
		if (m_fileProcessed)
		{
			printf("Error: -s specified after a filename; ignoring.\n");
		}
		else
		{
			if (m_delete)
			{
				printf("Warning: -s and -d both specified; ignoring -d.\n");
				m_delete = false;
			}
			m_overwrite = false;
		}
	}
	else if (stricmp(option, "-d") == 0)
	{
		if (m_fileProcessed)
		{
			printf("Error: -d specified after a filename; ignoring.\n");
		}
		else
		{
			if (m_overwrite)
			{
				m_delete = true;
			}
			else
			{
				printf("Warning: -s and -d both specified; ignoring -d.\n");
			}
		}
	}
	else if (stricmp(option, "-v") == 0)
	{
		if (m_fileProcessed)
		{
			printf("Error: -v specified after a filename; ignoring.\n");
		}
		else
		{
			m_verbose = true;
		}
	}
	else if (stricmp(option, "--") == 0)
	{
		m_optionsDone = true;
	}
	else if (option[0] == '-')
	{
		printf("Unknown option: %s\n", option);
		return true;
	}
	else
	{
		return false;
	}
	return true;
}

void UnMirrorStuds::ldlErrorCallback(LDLError *error)
{
	if (error && (m_verbose || error->getLevel() == LDLACriticalError))
	{
		char *filename = copyString(error->getFilename());

		printf("%s", error->getTypeName());
		if (error->getLineNumber() >= 0)
		{
			printf(" on line %d", error->getLineNumber());
		}
		printf(" in: ");
		if (filename)
		{
			cleanFilename(filename);
			printf("%s:\n", filename);
		}
		else
		{
			printf("<Unknown file>:\n");
		}
		if (error->getMessage())
		{
			printf("    %s\n", error->getMessage());
		}
		if (error->getFileLine())
		{
			printf("    %s\n", error->getFileLine());
		}
		delete filename;
	}
}

int UnMirrorStuds::run(int argc, char *argv[])
{
	int i;
	int retValue = 0;
	char initialPath[1024];

	if (getcwd(initialPath, sizeof(initialPath)) == NULL)
	{
		printf("Could not determine current directory.\n");
		return 0;
	}
	TCAlertManager::registerHandler(LDLError::alertClass(), this,
		(TCAlertCallback)&UnMirrorStuds::ldlErrorCallback);
	for (i = 1; i < argc; i++)
	{
		char newFilename[1024];
		const char *filename = argv[i];

		if (!checkOption(filename))
		{
			m_fileProcessed = true;
			if (chDirFromFilename(filename, newFilename, sizeof(newFilename)))
			{
				LDLMainModel *mainModel = new LDLMainModel;

				if (mainModel->load(newFilename))
				{
					float identMatrix[] = {
						1.0f, 0.0f, 0.0f, 0.0f,
						0.0f, 1.0f, 0.0f, 0.0f,
						0.0f, 0.0f, 1.0f, 0.0f,
						0.0f, 0.0f, 0.0f, 1.0f,
					};

					if (processModel(mainModel, identMatrix))
					{
						if (m_delete)
						{
							printf("Deleted %d files.\n", m_deleteCount);
							m_deleteCount = 0;
						}
						else
						{
							mirrorFilename(newFilename, filename, identMatrix);
							if (m_newFileCount)
							{
								printf("File created or updated: %s.\n",
									newFilename);
								m_newFileCount = 0;
							}
							else
							{
								printf("No updates for %s due to Safe Mode.\n",
									filename);
							}
						}
					}
					else
					{
						if (m_abort)
						{
							printf("Processing aborted for %s.\n", filename);
							m_abort = false;
						}
						else
						{
							if (m_delete)
							{
								printf("No files to delete for %s.\n",
									filename);
							}
							else
							{
								printf("No changes needed for %s.\n", filename);
							}
						}
					}
				}
				else
				{
					printf("Failed to load %s\n", filename);
					retValue = 1;
				}
				if (chdir(initialPath) != 0)
				{
					printf("Error changing back to original directory.\n");
				}
				mainModel->release();
			}
			else
			{
				retValue = 1;
			}
		}
	}
	if (!m_fileProcessed)
	{
		printUsage();
	}
	return retValue;
}

void UnMirrorStuds::printUsage(void)
{
	printf("UnMirrorStuds v0.1 Copyright (C) 2003 Travis Cobbs\n\n");
	printf("Usage:\n");
	printf("    UnMirrorStuds [options] filename [filename2, filename3, "
		"...]\n\n");
	printf("Options:\n");
	printf("    -d   Delete .ums and .um2 files created by a previous run "
		"of UnMirrorStuds\n");
	printf("         for the specified filename or filenames.  (Cannot be "
		"used with -s.)\n");
	printf("    -s   Safe mode.  Do not overwrite existing .ums and .um2 files."
		"  If you\n");
	printf("         have run the file before, this will probably produce no "
		"new output.\n");
	printf("         (Cannot be used with -d.)\n");
	printf("    -v   Give verbose error and status messages.  Error messages "
		"will include\n");
	printf("         errors that don't effect the functionality of "
		"UnMirrorStuds, but\n");
	printf("         might prevent both the original file and the .ums file "
		"from rendering\n");
	printf("         correctly.\n");
	printf("    --   No more options follow.  Use this as the final option to "
		"process a\n");
	printf("         file with a filename that begins with '-'.\n\n");
	printf("Multiple options must be separated by spaces.\n");
	printf("Filenames cannot include wildcard characters (such as '*' and '?')."
		"\n");
#ifdef WIN32
	printf("If you don't have an ldraw.ini file that specifies your LDraw "
		"directory, set\n");
	printf("the LDRAWDIR environment variable to contain the full path to your "
		"LDraw\n");
	printf("directory.\n");
#else // WIN32
	printf("Set the LDRAWDIR environment variable to contain the full path to "
		"your LDraw\n");
	printf("directory.\n");
#endif // WIN32
}

bool UnMirrorStuds::modelIsStud(LDLModel *model)
{
	return stringHasSuffix(model->getFilename(), "stud.dat") != 0;
}

bool UnMirrorStuds::checkModel(LDLModel *model, float *matrix,
							   LDLFileLineArray *mirroredLines)
{
	bool retValue = false;
	LDLFileLineArray *fileLines = model->getFileLines();
	int i;
	int count = fileLines->getCount();

	for (i = 0; i < count; i++)
	{
		LDLFileLine *fileLine = (*fileLines)[i];

		if (fileLine->getLineType() == LDLLineTypeModel)
		{
			LDLModelLine *modelLine = (LDLModelLine *)fileLine;
			LDLModel *newModel = modelLine->getModel();

			if (newModel)
			{
				float newMatrix[16];
				
				TCVector::multMatrix(matrix, modelLine->getMatrix(), newMatrix);
				if (modelIsStud(newModel))
				{
					if (TCVector::determinant(newMatrix) < 0.0f)
					{
						retValue = true;
					}
				}
				else
				{
					if (processModel(newModel, newMatrix))
					{
						retValue = true;
						mirroredLines->addObject(modelLine);
					}
				}
			}
		}
	}
	return retValue;
}

void UnMirrorStuds::mirrorStud(FILE *file, LDLModelLine *modelLine)
{
	float mirrorMatrix[] = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, -1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
	};
	float newMatrix[16];
	char newLine[1024];

	TCVector::multMatrix(modelLine->getMatrix(), mirrorMatrix, newMatrix);
	strcpy(newLine, modelLine->getLine());
	snprintf(newLine, sizeof(newLine), "1 %d %g %g %g %g %g %g %g %g %g %g %g %g%s",
		modelLine->getColorNumber(), newMatrix[12], newMatrix[13],
		newMatrix[14],
		newMatrix[0], newMatrix[4], newMatrix[8],
		newMatrix[1], newMatrix[5], newMatrix[9],
		newMatrix[2], newMatrix[6], newMatrix[10],
		strrchr(modelLine->getLine(), ' '));
	fprintf(file, "%s\n", newLine);
}

void UnMirrorStuds::mirrorFilename(char *newFilename, const char *oldFilename,
								   float *matrix)
{
	char *extension;

	strcpy(newFilename, oldFilename);
	extension = strrchr(newFilename, '.');
	if (extension)
	{
		if (TCVector::determinant(matrix) < 0)
		{
			strcpy(extension, ".um2");
		}
		else
		{
			strcpy(extension, ".ums");
		}
	}
}

void UnMirrorStuds::cleanFilename(char *filename)
{
#ifdef WIN32
	replaceStringCharacter(filename, '/', '\\', 1);
#else
	replaceStringCharacter(filename, '\\', '/', 1);
#endif // WIN32
}

void UnMirrorStuds::mirrorModel(LDLModel *model, float *matrix,
								LDLFileLineArray *mirroredLines)
{
	char mirroredFilename[1024];
	FILE *mirroredFile = NULL;

	mirrorFilename(mirroredFilename, model->getFilename(), matrix);
	cleanFilename(mirroredFilename);
	if (m_delete || !m_overwrite)
	{
		mirroredFile = ucfopen(mirroredFilename, "r");
	}
	if (mirroredFile)
	{
		fclose(mirroredFile);
		if (m_delete)
		{
			if (m_verbose)
			{
				printf("Deleting file %s\n", mirroredFilename);
			}
			ucunlink(mirroredFilename);
			m_deleteCount++;
		}
		else if (m_verbose)
		{
			printf("Already have %s\n", mirroredFilename);
		}
	}
	else if (!m_delete)
	{
		mirroredFile = ucfopen(mirroredFilename, "w");
		if (mirroredFile)
		{
			int i;
			LDLFileLineArray *fileLines = model->getFileLines();
			int count = fileLines->getCount();

			m_newFileCount++;
			for (i = 0; i < count; i++)
			{
				LDLFileLine *fileLine = (*fileLines)[i];
				bool written = false;

				// If the line has an original line, then it is a duplicate of
				// another one.
				if (fileLine->getLineType() == LDLLineTypeModel &&
					!fileLine->getOriginalLine())
				{
					LDLModelLine *modelLine = (LDLModelLine *)fileLine;
					LDLModel *newModel = modelLine->getModel();

					if (newModel)
					{
						int index = mirroredLines->indexOfObject(modelLine);
						float newMatrix[16];

						TCVector::multMatrix(matrix, modelLine->getMatrix(),
							newMatrix);
						if (index >= 0)
						{
							char newLine[1024];

							mirrorFilename(newLine, fileLine->getLine(),
								newMatrix);
							fprintf(mirroredFile, "%s\n", newLine);
							written = true;
						}
						else
						{
							if (modelIsStud(newModel))
							{
								if (TCVector::determinant(newMatrix) < 0)
								{
									mirrorStud(mirroredFile, modelLine);
									written = true;
								}
							}
						}
					}
				}
				if (!written && !fileLine->getOriginalLine())
				{
					fprintf(mirroredFile, "%s\n", fileLine->getLine());
				}
			}
		}
	}
}

bool UnMirrorStuds::processModel(LDLModel *model, float *matrix)
{
	bool retValue = false;
	LDLFileLineArray *mirroredLines = new LDLFileLineArray;

	if (model->isMPD())
	{
		char *filename = copyString(model->getFilename());

		cleanFilename(filename);
		printf("%s is an MPD file.  MPD files are not supported.\n",
			filename);
		delete filename;
		m_abort = true;
	}
	if (!m_abort && checkModel(model, matrix, mirroredLines))
	{
		retValue = true;
		mirrorModel(model, matrix, mirroredLines);
	}
	mirroredLines->release();
	return retValue;
}

bool UnMirrorStuds::chDirFromFilename(const char *filename, char* outFilename,
									  unsigned maxLength)
{
	char *newFilename = copyString(filename);
	char *fileSpot;
	bool retValue = false;

	replaceStringCharacter(newFilename, '\\', '/');
	fileSpot = strrchr(newFilename, '/');
	if (fileSpot)
	{
		int len = fileSpot - newFilename;
		char *path = new char[(size_t)len + 1];

		strncpy(path, newFilename, len);
		path[len] = 0;
		if (chdir(path) == 0)
		{
			if (getcwd(outFilename, maxLength) == NULL)
			{
				delete[] path;
				delete[] newFilename;
				return false;
			}
			if (strlen(outFilename) + strlen(fileSpot) < maxLength)
			{
				retValue = true;
				strcat(outFilename, fileSpot);
			}
		}
		delete[] path;
	}
	else
	{
		// Filename doesn't contain a path
		if (strlen(newFilename) < maxLength)
		{
			strcpy(outFilename, newFilename);
			retValue = true;
		}
	}
	delete[] newFilename;
	if (!retValue)
	{
		printf("Couldn't find directory for file %s\n", filename);
	}
	return retValue;
}

