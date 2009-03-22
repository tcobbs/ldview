#include "LD3dsExporter.h"
#include <LDLoader/LDLMainModel.h>
#include <LDLoader/LDLModelLine.h>
#include <LDLoader/LDLTriangleLine.h>
#include <LDLoader/LDLQuadLine.h>
#include <LDLoader/LDLPalette.h>
#include <lib3ds.h>

#if defined WIN32 && defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif

LD3dsExporter::LD3dsExporter(void):
LDExporter("3dsExporter/")
{
	loadSettings();
}

LD3dsExporter::~LD3dsExporter(void)
{
}

void LD3dsExporter::initSettings(void) const
{
}

void LD3dsExporter::dealloc(void)
{
	LDExporter::dealloc();
}

ucstring LD3dsExporter::getTypeDescription(void) const
{
	return ls(_UC("3dsTypeDescription"));
}

void LD3dsExporter::loadSettings(void)
{
	LDExporter::loadSettings();
}

int LD3dsExporter::getMaterial(int colorNumber)
{
	IntIntMap::iterator it = m_colorNumbers.find(colorNumber);

	if (it == m_colorNumbers.end())
	{
		int material = m_colorNumbers.size();
		Lib3dsMaterial *mat = lib3ds_material_new((std::string("ldraw_") +
			ltostr(colorNumber)).c_str());
        lib3ds_file_insert_material(m_file, mat, -1);
		int r, g, b, a;
		LDLPalette *pPalette = m_topModel->getMainModel()->getPalette();
		pPalette->getRGBA(colorNumber, r, g, b, a);
		LDLColorInfo colorInfo = pPalette->getAnyColorInfo(colorNumber);

        mat->diffuse[0] = r / 255.0f;
        mat->diffuse[1] = g / 255.0f;
        mat->diffuse[2] = b / 255.0f;
		mat->transparency = 1.0f - a / 255.0f;
		if (colorInfo.rubber)
		{
			mat->specular[0] = mat->specular[1] = mat->specular[2] = 0.05f;
		}
		mat->shading = LIB3DS_SHADING_PHONG;
		m_colorNumbers[colorNumber] = material;
		return material;
	}
	else
	{
		return it->second;
	}
}

void LD3dsExporter::writeTriangle(
	Lib3dsMesh *mesh,
	const TCVector *points,
	int i0,
	int i1,
	int i2,
	int colorNumber,
	const TCFloat *matrix)
{
	int ix[3];
	int voffset = mesh->nvertices;
	int foffset = mesh->nfaces;

	ix[0] = i0;
	ix[1] = i1;
	ix[2] = i2;
	lib3ds_mesh_resize_vertices(mesh, voffset + 3, 0, 0);
	lib3ds_mesh_resize_faces(mesh, foffset + 1);
	for (int i = 0; i < 3; i++)
	{
		TCVector vector = points[ix[i]];

		vector = vector.transformPoint(matrix);
		mesh->vertices[voffset + i][0] = vector[0];
		mesh->vertices[voffset + i][1] = vector[1];
		mesh->vertices[voffset + i][2] = vector[2];
		mesh->faces[foffset].index[i] = (unsigned short)(voffset + i);
		mesh->faces[foffset].material = getMaterial(colorNumber);
	}
}

void LD3dsExporter::writeShapeLine(
	Lib3dsMesh *mesh,
	LDLShapeLine *pShapeLine,
	const TCFloat *matrix,
	int colorNumber)
{
	if (pShapeLine->getColorNumber() != 16)
	{
		colorNumber = pShapeLine->getColorNumber();
	}
	writeTriangle(mesh, pShapeLine->getPoints(), 0, 1, 2, colorNumber, matrix);
	if (pShapeLine->getNumPoints() > 3)
	{
		writeTriangle(mesh, pShapeLine->getPoints(), 0, 2, 3, colorNumber,
			matrix);
	}
}

void LD3dsExporter::doExport(
	LDLModel *pModel,
	const TCFloat *matrix,
	int colorNumber)
{
	LDLFileLineArray *pFileLines = pModel->getFileLines();

	if (pFileLines != NULL)
	{
		int count = pModel->getActiveLineCount();
		std::string meshNumString = ltostr(m_numMeshes++);
		Lib3dsMesh *mesh = lib3ds_mesh_new((meshNumString + "m").c_str());
		Lib3dsMeshInstanceNode *pInst;

		lib3ds_file_insert_mesh(m_file, mesh, -1);
		for (int i = 0; i < count; i++)
		{
			LDLFileLine *pFileLine = (*pFileLines)[i];

			if (!pFileLine->isValid())
			{
				continue;
			}
			switch (pFileLine->getLineType())
			{
			case LDLLineTypeTriangle:
			case LDLLineTypeQuad:
				writeShapeLine(mesh, (LDLShapeLine *)pFileLine, matrix,
					colorNumber);
				break;
			case LDLLineTypeModel:
				{
					LDLModelLine *pModelLine = (LDLModelLine *)pFileLine;
					LDLModel *pOtherModel = pModelLine->getModel(true);

					if (pOtherModel != NULL)
					{
						TCFloat newMatrix[16];
						int otherColorNumber = pModelLine->getColorNumber();

						if (otherColorNumber == 16)
						{
							otherColorNumber = colorNumber;
						}
						TCVector::multMatrix(matrix, pModelLine->getMatrix(),
							newMatrix);
						doExport(pOtherModel, newMatrix, otherColorNumber);
					}
				}
				break;
			}
		}
		pInst = lib3ds_node_new_mesh_instance(mesh,
			(meshNumString + "n").c_str(), NULL, NULL, NULL);
		lib3ds_file_append_node(m_file, (Lib3dsNode *)pInst, NULL);
	}
}

int LD3dsExporter::doExport(LDLModel *pTopModel)
{
	int retVal = 1;

	m_topModel = pTopModel;
    m_file = lib3ds_file_new();
	m_numMeshes = 0;
	doExport(pTopModel, TCVector::getIdentityMatrix(), 7);
	if (!lib3ds_file_save(m_file, m_filename.c_str()))
	{
		retVal = 0;
	}
	lib3ds_file_free(m_file);
	return retVal;
}
