#include "LDLdrExporter.h"
#include <TRE/TREModel.h>
#include <TRE/TRESubModel.h>
#include <TRE/TREVertexStore.h>

#if defined WIN32 && defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif

LDLdrExporter::LDLdrExporter(void)
	: LDExporter("LdrExporter/")
{
	loadSettings();
}

LDLdrExporter::~LDLdrExporter(void)
{
}

void LDLdrExporter::initSettings(void) const
{
}

void LDLdrExporter::dealloc(void)
{
	LDExporter::dealloc();
}

ucstring LDLdrExporter::getTypeDescription(void) const
{
	return ls(_UC("LdrTypeDescription"));
}

void LDLdrExporter::loadSettings(void)
{
	LDExporter::loadSettings();
}

void LDLdrExporter::exportTriangle(
	TREVertexArray *vertices,
	TCULongArray *indices,
	int ix,
	int i0,
	int i1,
	int i2,
	const TCFloat *pMatrix)
{
	int ip[3];
	ip[0]=i0; ip[1]=i1; ip[2]=i2;

	fprintf(m_pFile, "3 16");
	for (int i = 0; i < 3; i++)
	{
		int index = (*indices)[ix + ip[i]];
		const TREVertex &treVertex = (*vertices)[index];
		TCVector vector(treVertex.v[0], treVertex.v[1], treVertex.v[2]);

		vector = vector.transformPoint(pMatrix);
		fprintf(m_pFile, " %s %s %s", ftostr(vector[0], 4).c_str(),
			ftostr(vector[1], 4).c_str(), ftostr(vector[2], 4).c_str());
	}
	fprintf(m_pFile, "\r\n");
}

void LDLdrExporter::exportQuad(
	TREVertexArray *vertices,
	TCULongArray *indices,
	int ix,
	int i0,
	int i1,
	int i2,
	int i3,
	const TCFloat *pMatrix)
{
	int ip[4];
	ip[0]=i0; ip[1]=i1; ip[2]=i2; ip[3] = i3;

	fprintf(m_pFile, "4 16");
	for (int i = 0; i < 4; i++)
	{
		int index = (*indices)[ix + ip[i]];
		const TREVertex &treVertex = (*vertices)[index];
		TCVector vector(treVertex.v[0], treVertex.v[1], treVertex.v[2]);

		vector = vector.transformPoint(pMatrix);
		fprintf(m_pFile, " %s %s %s", ftostr(vector[0], 4).c_str(),
			ftostr(vector[1], 4).c_str(), ftostr(vector[2], 4).c_str());
	}
	fprintf(m_pFile, "\r\n");
}

void LDLdrExporter::exportConditionalLine(
	TREVertexArray *vertices,
	TCULongArray *indices,
	TCULongArray *cpIndices,
	int ix,
	int i0,
	int i1,
	const TCFloat *pMatrix)
{
	int ip[4];
	ip[0]=i0; ip[1]=i1; ip[2]=i0; ip[3]=i1;

	fprintf(m_pFile, "5 24");
	for (int i = 0; i < 4; i++)
	{
		int index;
		if (i < 2)
		{
			index = (*indices)[ix + ip[i]];
		}
		else
		{
			index = (*cpIndices)[ix + ip[i]];
		}

		const TREVertex &treVertex = (*vertices)[index];
		TCVector vector(treVertex.v[0], treVertex.v[1], treVertex.v[2]);

		vector = vector.transformPoint(pMatrix);
		fprintf(m_pFile, " %s %s %s", ftostr(vector[0], 4).c_str(),
			ftostr(vector[1], 4).c_str(), ftostr(vector[2], 4).c_str());
	}
	fprintf(m_pFile, "\r\n");
}

void LDLdrExporter::exportLine(
	TREVertexArray *vertices,
	TCULongArray *indices,
	int ix,
	int i0,
	int i1,
	const TCFloat *pMatrix)
{
	int ip[4];
	ip[0]=i0; ip[1]=i1;

	fprintf(m_pFile, "2 24");
	for (int i = 0; i < 2; i++)
	{
		int index = (*indices)[ix + ip[i]];

		const TREVertex &treVertex = (*vertices)[index];
		TCVector vector(treVertex.v[0], treVertex.v[1], treVertex.v[2]);

		vector = vector.transformPoint(pMatrix);
		fprintf(m_pFile, " %s %s %s", ftostr(vector[0], 4).c_str(),
			ftostr(vector[1], 4).c_str(), ftostr(vector[2], 4).c_str());
	}
	fprintf(m_pFile, "\r\n");
}

void LDLdrExporter::exportStrips(
	TREShapeGroup *shapeGroup,
	int nShapeType,
	const TCFloat *pMatrix)
{
	TREShapeType eShapeType = (TREShapeType)nShapeType;
	TCULongArray *indices = shapeGroup->getIndices(eShapeType, false);
	TCULongArray *stripCounts = shapeGroup->getStripCounts(eShapeType, false);
	TREVertexArray *vertices = shapeGroup->getVertexStore()->getVertices();
	int stripMargin = 2;
	int stripInc = 1;

	if (eShapeType == TRESQuadStrip)
	{
		stripMargin = 3;
		stripInc = 2;
	}
	if (indices != NULL && stripCounts != NULL)
	{
		size_t numStrips = stripCounts->getCount();

		if (numStrips > 0)
		{
			int ofs = 0;

			for (unsigned int j = 0; j < numStrips; j++)
			{
				int stripCount = (*stripCounts)[j];

				for (int k = 0; k < stripCount - stripMargin; k += stripInc)
				{
					switch (eShapeType)
					{
					case TRESTriangleStrip:
						if (k % 2 == 0)
						{
							exportTriangle(vertices, indices,
								ofs + k, 0, 1, 2, pMatrix);
						}
						else
						{
							exportTriangle(vertices, indices,
								ofs + k, 0, 2, 1, pMatrix);
						}
						break;
					case TRESTriangleFan:
						exportTriangle(vertices, indices,
							ofs, 0, k + 1, k + 2, pMatrix);
						break;
					case TRESQuadStrip:
						exportTriangle(vertices, indices,
							ofs + k, 0, 1, 2, pMatrix);
						exportTriangle(vertices, indices,
							ofs + k, 1, 2, 3, pMatrix);
						break;
					default:
						// Get rid of gcc warnings.
						break;
					}
				}
				ofs += stripCount;
			}
		}
	}
}

void LDLdrExporter::exportShapes(
	TREShapeGroup **ppShapes,
	const TCFloat *pMatrix)
{
	for (int i = 0; i <= TREMLast; i++)
	{
		TREShapeGroup *shape = ppShapes[i];

		if (shape != NULL)
		{
			TCULongArray *indices = 
				shape->getIndices(TRESTriangle, false);
			TREVertexStore *vertexStore = shape->getVertexStore();

			if (indices != NULL)
			{
				TREVertexArray *vertices = vertexStore->getVertices();
				size_t count = indices->getCount();

				for (size_t p = 0; p < count; p+=3)
				{
					exportTriangle(vertices, indices, (int)p, 0, 1, 2, pMatrix);
				}
			}
			indices = shape->getIndices(TRESQuad, false);
			if (indices != NULL)
			{
				TREVertexArray *vertices = vertexStore->getVertices();
				size_t count = indices->getCount();

				for (size_t p = 0; p < count; p+=4)
				{
					exportQuad(vertices, indices, (int)p, 0, 1, 2, 3, pMatrix);
				}
			}
			exportStrips(shape, TRESTriangleStrip, pMatrix);
			exportStrips(shape, TRESTriangleFan, pMatrix);
			exportStrips(shape, TRESQuadStrip, pMatrix);
			indices = shape->getIndices(TRESLine, false);
			if (indices != NULL)
			{
				TREVertexArray *vertices = vertexStore->getVertices();
				size_t count = indices->getCount();

				for (size_t p = 0; p < count; p+=2)
				{
					exportLine(vertices, indices, (int)p, 0, 1, pMatrix);
				}
			}
			indices = shape->getIndices(TRESConditionalLine, false);
			if (indices != NULL)
			{
				TREVertexArray *vertices = vertexStore->getVertices();
				size_t count = indices->getCount();

				for (size_t p = 0; p < count; p+=2)
				{
					TCULongArray *cpIndices = shape->getControlPointIndices();

					exportConditionalLine(vertices, indices, cpIndices, (int)p, 0, 1,
						pMatrix);
				}
			}
		}
	}
}

int LDLdrExporter::exportModel(TREModel *pModel, const TCFloat *pMatrix)
{
	exportShapes(pModel->getShapes(), pMatrix);
	exportShapes((TREShapeGroup **)pModel->getColoredShapes(), pMatrix);
	TRESubModelArray *pSubModels = pModel->getSubModels();
	if (pSubModels != NULL)
	{
		for (unsigned int i = 0; i < pSubModels->getCount(); i++)
		{
			TRESubModel *subModel = (*pSubModels)[i];
			TCFloat newMatrix[16];
			
			TCVector::multMatrix(pMatrix, subModel->getMatrix(), newMatrix);
			exportModel(subModel->getEffectiveModel(), newMatrix);
		}
	}
	return 0;
}

int LDLdrExporter::doExport(TREModel *pTopModel)
{
	m_pFile = fopen(m_filename.c_str(), "wb");
	int retVal = 1;

	if (m_pFile != NULL)
	{
		retVal = exportModel(pTopModel, TCVector::getIdentityMatrix());
		fclose(m_pFile);
	}
	return retVal;
}
