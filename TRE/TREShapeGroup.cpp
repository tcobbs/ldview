#include "TREShapeGroup.h"
#include "TREVertexArray.h"
#include "TREVertexStore.h"
#include "TREModel.h"
#include <TCFoundation/TCVector.h>
#include <TCFoundation/TCMacros.h>

PFNGLMULTIDRAWELEMENTSEXTPROC TREShapeGroup::glMultiDrawElementsEXT = NULL;

TREShapeGroup::TREShapeGroup(void)
	:m_vertexStore(NULL),
	m_indices(NULL),
	m_stripCounts(NULL),
	m_multiDrawIndices(NULL),
	m_shapesPresent(0)
{
}

TREShapeGroup::TREShapeGroup(const TREShapeGroup &other)
	:m_vertexStore(other.m_vertexStore),
	m_indices(NULL),
//	m_stripCounts(NULL),
	m_stripCounts((TCULongArrayArray *)TCObject::copy(other.m_stripCounts)),
	m_multiDrawIndices(NULL),
	m_shapesPresent(other.m_shapesPresent)
{
	m_vertexStore->retain();
/*
	if (other.m_stripCounts)
	{
		int shapeTypeCount = other.m_stripCounts->getCount();
		int i, j;

		m_stripCounts = new TCULongArrayArray(shapeTypeCount);
		for (i = 0; i < shapeTypeCount; i++)
		{
			TCULongArray *thoseStripCounts = (*other.m_stripCounts)[i];

			if (thoseStripCounts)
			{
				int numStrips = thoseStripCounts->getCount();
				TCULongArray *theseStripCounts = new TCULongArray(numStrips);

				for (j = 0; j < numStrips; j++)
				{
					theseStripCounts->addValue((*thoseStripCounts)[j]);
				}
				m_stripCounts->addObject(theseStripCounts);
				theseStripCounts->release();
			}
			else
			{
				m_stripCounts->addObject(NULL);
			}
		}
	}
*/
	if (other.m_shapesPresent)
	{
		int i, j;
		int shapeTypeCount = other.m_indices->getCount();
		TREVertexArray *vertices = m_vertexStore->getVertices();
		TREVertexArray *normals = m_vertexStore->getNormals();
		TCULongArray *colors = m_vertexStore->getColors();

		m_indices = new TCULongArrayArray(shapeTypeCount);
		for (i = 0; i < shapeTypeCount; i++)
		{
			TCULongArray *thoseIndices = (*other.m_indices)[i];
			int indexCount = thoseIndices->getCount();
			TCULongArray *theseIndices = new TCULongArray(indexCount);

			m_indices->addObject(theseIndices);
			for (j = 0; j < indexCount; j++)
			{
				int index = (*thoseIndices)[j];
				TREVertex vertex = (*vertices)[index];
				TREVertex normal = (*normals)[index];

				theseIndices->addValue(vertices->getCount());
				vertices->addVertex(vertex);
				normals->addVertex(normal);
				if (colors)
				{
					TCULong color = (*colors)[index];

					colors->addValue(color);
				}
			}
			theseIndices->release();
		}
	}
}

TREShapeGroup::~TREShapeGroup(void)
{
}

TCObject *TREShapeGroup::copy(void)
{
	return new TREShapeGroup(*this);
}

void TREShapeGroup::dealloc(void)
{
	// ************************************************************************
	// The deleteMultiDrawIndices call MUST precede the release calls for
	// m_indices and m_stripCounts.
	deleteMultiDrawIndices();
	// ************************************************************************
	TCObject::release(m_vertexStore);
	TCObject::release(m_indices);
	TCObject::release(m_stripCounts);
	TCObject::dealloc();
}

void TREShapeGroup::addShapeType(TREShapeType shapeType, int index)
{
	TCULongArray *newIndexArray = new TCULongArray;

	if (!m_indices)
	{
		m_indices = new TCULongArrayArray;
	}
	m_indices->insertObject(newIndexArray, index);
	newIndexArray->release();
	m_shapesPresent |= shapeType;
	if (!m_stripCounts)
	{
		m_stripCounts = new TCULongArrayArray;
	}
	if (shapeType >= TRESFirstStrip)
	{
		TCULongArray *newCountArray = new TCULongArray;

//		newCountArray->addValue(0);
		m_stripCounts->insertObject(newCountArray, index);
		newCountArray->release();
	}
	else
	{
		m_stripCounts->insertObject(NULL);
	}
}

TCULongArray *TREShapeGroup::getIndices(TREShapeType shapeType, bool create)
{
	TCULong index = getShapeTypeIndex(shapeType);

	if (!(m_shapesPresent & shapeType))
	{
		if (create)
		{
			addShapeType(shapeType, index);
		}
		else
		{
			return NULL;
		}
	}
	return m_indices->objectAtIndex(index);
}

TCULongArray *TREShapeGroup::getStripCounts(TREShapeType shapeType, bool create)
{
	TCULong index = getShapeTypeIndex(shapeType);

	if (!(m_shapesPresent & shapeType))
	{
		if (create)
		{
			addShapeType(shapeType, index);
		}
		else
		{
			return NULL;
		}
	}
	return m_stripCounts->objectAtIndex(index);
}

TCULong TREShapeGroup::getShapeTypeIndex(TREShapeType shapeType)
{
	int bit;
	TCULong index = 0;

	for (bit = 1; bit != shapeType; bit = bit << 1)
	{
		if (m_shapesPresent & bit)
		{
			index++;
		}
	}
	return index;
}

/*
void TREShapeGroup::addConditionalLine(int index1, int index2, int index3,
									   int index4)
{
	addShape(TRESConditionalLine, index1, index2, index3, index4);
}
*/

void TREShapeGroup::addShapeIndices(TREShapeType shapeType, int firstIndex,
									int count)
{
	TCULongArray *indices = getIndices(shapeType, true);
	int i;

	for (i = 0; i < count; i++)
	{
		indices->addValue(firstIndex + i);
	}
}

void TREShapeGroup::addShapeStripCount(TREShapeType shapeType, int count)
{
	TCULongArray *counts = getStripCounts(shapeType, true);

	counts->addValue(count);
}

GLenum TREShapeGroup::modeForShapeType(TREShapeType shapeType)
{
	switch (shapeType)
	{
	case TRESLine:
		return GL_LINES;
		break;
	case TRESTriangle:
		return GL_TRIANGLES;
		break;
	case TRESQuad:
		return GL_QUADS;
		break;
	case TRESConditionalLine:
		return GL_LINES;
		break;
	case TRESBFCTriangle:
		return GL_TRIANGLES;
		break;
	case TRESBFCQuad:
		return GL_QUADS;
		break;
	case TRESTriangleStrip:
		return GL_TRIANGLE_STRIP;
		break;
	case TRESQuadStrip:
		return GL_QUAD_STRIP;
		break;
	case TRESTriangleFan:
		return GL_TRIANGLE_FAN;
		break;
	default:
		// We shouldn't ever get here.
		return GL_TRIANGLES;
		break;
	}
}

int TREShapeGroup::numPointsForShapeType(TREShapeType shapeType)
{
	switch (shapeType)
	{
	case TRESLine:
		return 2;
		break;
	case TRESTriangle:
		return 3;
		break;
	case TRESQuad:
		return 4;
		break;
	case TRESConditionalLine:
		return 4;
		break;
	case TRESBFCTriangle:
		return 3;
		break;
	case TRESBFCQuad:
		return 4;
		break;
	default:
		// Strips are variable size
		return 0;
		break;
	}
}

/*
static void printULongArray(char *label, TCULongArray *array)
{
	printf("%s", label);
	if (array)
	{
		int i;
		int count = array->getCount();

		for (i = 0; i < count; i++)
		{
			printf("%d ", (*array)[i]);
		}
	}
	printf("\n");
}
*/

void TREShapeGroup::drawShapeType(TREShapeType shapeType)
{
	TCULongArray *indexArray = getIndices(shapeType);

	if (indexArray)
	{
//		int count = indexArray->getCount();
//		TCULong *values = indexArray->getValues();
//		int i;

/*
		printf("TREShapeGroup::drawShapeType\n");
		for (i = 0; i < count; i++)
		{
			TCULong index = values[i];
			TREVertex vertex = (*m_vertexStore->getVertices())[index];
			TREVertex normal = (*m_vertexStore->getNormals())[index];

			printf("%10f %10f %10f      ", vertex.v[0], vertex.v[1], vertex.v[2]);
			printf("%10f %10f %10f\n", normal.v[0], normal.v[1], normal.v[2]);
		}
		if (!_CrtIsValidPointer(values, count * 4, TRUE))
		{
			printf("Bad Pointer!\n");
		}
*/
/*
		printf(" count: %d\n", count);
		printf("values: 0x%08X\n", values);
		printf("retain: %d\n", indexArray->getRetainCount());
		printf("    vc: %d\n", m_vertexStore->getVertices()->getCount());
		printULongArray("", indexArray);
*/
		glDrawElements(modeForShapeType(shapeType), indexArray->getCount(),
			GL_UNSIGNED_INT, indexArray->getValues());
	}
}

// This is really ugly, but I really need to use dynamic arrays while filling
// in the data.  Since that can't be passed to glMultiDrawElements, I have to
// create a new copy of the data and copy it here.
void TREShapeGroup::initMultiDrawIndices(void)
{
	if (!m_multiDrawIndices)
	{
		int shapeTypeCount = m_indices->getCount();
		int i, j;

		m_multiDrawIndices = new TCULong**[shapeTypeCount];
		for (i = 0; i < shapeTypeCount; i++)
		{
			TCULongArray *stripCounts = (*m_stripCounts)[i];

			if (stripCounts)
			{
				TCULongArray *indices = (*m_indices)[i];
				int numStrips = stripCounts->getCount();

				if (numStrips)
				{
					int indexOffset = 0;

					m_multiDrawIndices[i] = new TCULong*[numStrips];
					for (j = 0; j < numStrips; j++)
					{
						int stripCount = (*stripCounts)[j];

						if (stripCount)
						{
							m_multiDrawIndices[i][j] = new TCULong[stripCount];
							memcpy(m_multiDrawIndices[i][j],
								indices->getValues() + indexOffset,
								stripCount * sizeof(TCULong));
						}
						else
						{
							m_multiDrawIndices[i][j] = NULL;
						}
						indexOffset += stripCount;
					}
				}
				else
				{
					m_multiDrawIndices[i] = NULL;
				}
			}
			else
			{
				m_multiDrawIndices[i] = NULL;
			}
		}
	}
}

// The m_multiDrawIndices array doesn't have any internal tracking of the size
// of the various nesting levels, so we just need to get that info from the
// other variables the same way we did when we originally created it.
void TREShapeGroup::deleteMultiDrawIndices(void)
{
	if (m_multiDrawIndices)
	{
		int shapeTypeCount = m_indices->getCount();
		int i, j;

		for (i = 0; i < shapeTypeCount; i++)
		{
			TCULongArray *stripCounts = (*m_stripCounts)[i];

			if (stripCounts)
			{
				int numStrips = stripCounts->getCount();

				if (numStrips)
				{
					for (j = 0; j < numStrips; j++)
					{
						if (m_multiDrawIndices[i][j])
						{
							delete m_multiDrawIndices[i][j];
						}
					}
				}
			}
			if (m_multiDrawIndices[i])
			{
				delete m_multiDrawIndices[i];
			}
		}
		delete m_multiDrawIndices;
	}
}

void TREShapeGroup::drawStripShapeType(TREShapeType shapeType)
{
	if (m_shapesPresent & shapeType)
	{
		TCULongArray *indexArray = getIndices(shapeType);
		TCULongArray *countArray = getStripCounts(shapeType);

		if (indexArray && countArray)
		{
			int numStrips = countArray->getCount();

			if (numStrips)
			{
				GLenum glMode = modeForShapeType(shapeType);

				if (glMultiDrawElementsEXT)
				{
					int shapeTypeIndex = getShapeTypeIndex(shapeType);

					if (!m_multiDrawIndices)
					{
						initMultiDrawIndices();
					}
//					printf("TREShapeGroup::drawStripShapeType:multi\n");
					glMultiDrawElementsEXT(glMode,
						(GLsizei *)countArray->getValues(), GL_UNSIGNED_INT,
						(const void **)m_multiDrawIndices[shapeTypeIndex],
						numStrips);
				}
				else
				{
					TCULong *indices = indexArray->getValues();
					int indexOffset = 0;
					int i;
/*
					int count = indexArray->getCount();

					printf("TREShapeGroup::drawStripShapeType:nonmulti\n");
					if (!_CrtIsValidPointer(indices, count * 4, TRUE))
					{
						printf("Bad Pointer!\n");
					}
					for (i = 0; i < count; i++)
					{
						TCULong index = indices[i];
						TREVertex vertex = (*m_vertexStore->getVertices())[index];
						TREVertex normal = (*m_vertexStore->getNormals())[index];

						printf("%10f %10f %10f      ", vertex.v[0], vertex.v[1], vertex.v[2]);
						printf("%10f %10f %10f\n", normal.v[0], normal.v[1], normal.v[2]);
					}
*/
					for (i = 0; i < numStrips; i++)
					{
						int stripCount = (*countArray)[i];

						glDrawElements(glMode, stripCount, GL_UNSIGNED_INT,
							indices + indexOffset);
						indexOffset += stripCount;
					}
				}
			}
		}
	}
}

void TREShapeGroup::draw(void)
{
	drawNonBFC();
//	drawBFC();
}

void TREShapeGroup::drawBFC(void)
{
	if (m_vertexStore && (m_shapesPresent & (TRESBFCTriangle | TRESBFCQuad)))
	{
		// Note that GL_BACK is the default face to cull, and GL_CCW is the
		// default polygon winding.
		glEnable(GL_CULL_FACE);
		if (m_vertexStore->getTwoSidedLightingFlag())
		{
			glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);
		}
		drawShapeType(TRESBFCTriangle);
		drawShapeType(TRESBFCQuad);
		if (m_vertexStore->getTwoSidedLightingFlag())
		{
			glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);
		}
		glDisable(GL_CULL_FACE);
	}
}

void TREShapeGroup::drawNonBFC(void)
{
	if (m_vertexStore)
	{
		drawShapeType(TRESTriangle);
		drawShapeType(TRESQuad);
		drawStripShapeType(TRESTriangleStrip);
		drawStripShapeType(TRESQuadStrip);
		drawStripShapeType(TRESTriangleFan);
	}
}

void TREShapeGroup::drawLines(void)
{
	if (m_vertexStore)
	{
		drawShapeType(TRESLine);
	}
}

int TREShapeGroup::addShape(TREShapeType shapeType, TCVector *vertices,
							int count)
{
	return addShape(shapeType, vertices, NULL, count);
}

int TREShapeGroup::addShape(TREShapeType shapeType, TCVector *vertices,
							TCVector *normals, int count)
{
	int index;

	m_vertexStore->setup();
	if (normals)
	{
		index = m_vertexStore->addVertices(vertices, normals, count);
	}
	else
	{
		index = m_vertexStore->addVertices(vertices, count);
	}
	addShapeIndices(shapeType, index, count);
	return index;
}

int TREShapeGroup::addLine(TCVector *vertices)
{
	return addShape(TRESLine, vertices, 2);
}

int TREShapeGroup::addTriangle(TCVector *vertices)
{
	return addShape(TRESTriangle, vertices, 3);
}

int TREShapeGroup::addTriangle(TCVector *vertices, TCVector *normals)
{
	return addShape(TRESTriangle, vertices, normals, 3);
}

int TREShapeGroup::addQuad(TCVector *vertices)
{
	return addShape(TRESQuad, vertices, 4);
}

int TREShapeGroup::addQuad(TCVector *vertices, TCVector *normals)
{
	return addShape(TRESQuad, vertices, normals, 4);
}

int TREShapeGroup::addBFCTriangle(TCVector *vertices)
{
	return addShape(TRESBFCTriangle, vertices, 3);
}

int TREShapeGroup::addBFCTriangle(TCVector *vertices, TCVector *normals)
{
	return addShape(TRESBFCTriangle, vertices, normals, 3);
}

int TREShapeGroup::addBFCQuad(TCVector *vertices)
{
	return addShape(TRESBFCQuad, vertices, 4);
}

int TREShapeGroup::addBFCQuad(TCVector *vertices, TCVector *normals)
{
	return addShape(TRESBFCQuad, vertices, normals, 4);
}

int TREShapeGroup::addQuadStrip(TCVector *vertices, TCVector *normals, int count)
{
	return addStrip(TRESQuadStrip, vertices, normals, count);
}

int TREShapeGroup::addTriangleFan(TCVector *vertices, TCVector *normals, int count)
{
	return addStrip(TRESTriangleFan, vertices, normals, count);
}

int TREShapeGroup::addStrip(TREShapeType shapeType, TCVector *vertices,
							TCVector *normals, int count)
{
	int index;

	m_vertexStore->setup();
	index = m_vertexStore->addVertices(vertices, normals, count);
	addShapeStripCount(shapeType, count);
	addShapeIndices(shapeType, index, count);
	return index;
}

void TREShapeGroup::setVertexStore(TREVertexStore *vertexStore)
{
	vertexStore->retain();
	TCObject::release(m_vertexStore);
	m_vertexStore = vertexStore;
}

void TREShapeGroup::scanPoints(TCObject *scanner,
							   TREScanPointCallback scanPointCallback,
							   float* matrix)
{
	int bit;

	for (bit = 1; (TREShapeType)bit < TRESFirstStrip; bit = bit << 1)
	{
		scanPoints(getIndices((TREShapeType)bit), scanner, scanPointCallback,
			matrix);
	}
	for (; (TREShapeType)bit <= TRESLast; bit = bit << 1)
	{
		scanStripPoints(getIndices((TREShapeType)bit),
			getStripCounts((TREShapeType)bit), scanner, scanPointCallback,
			matrix);
	}
}

void TREShapeGroup::scanPoints(TCULongArray *indices, TCObject *scanner,
							   TREScanPointCallback scanPointCallback,
							   float* matrix)
{
	if (indices)
	{
		int i;
		int count = indices->getCount();

		for (i = 0; i < count; i++)
		{
			scanPoints((*indices)[i], scanner, scanPointCallback, matrix);
		}
	}
}

void TREShapeGroup::scanStripPoints(TCULongArray *indices,
									TCULongArray *stripCounts,
									TCObject *scanner,
									TREScanPointCallback scanPointCallback,
									float* matrix)
{
	if (indices && stripCounts)
	{
		int i, j;
		int numStrips = stripCounts->getCount();
		int indexOffset = 0;

		for (i = 0; i < numStrips; i++)
		{
			int stripCount = (*stripCounts)[i];
			for (j = 0; j < stripCount; j++)
			{
				scanPoints((*indices)[j + indexOffset], scanner,
					scanPointCallback, matrix);
			}
			indexOffset += stripCount;
		}
	}
}

void TREShapeGroup::scanPoints(const TREVertex &vertex, TCObject *scanner,
							   TREScanPointCallback scanPointCallback)
{
	TCVector point = TCVector(vertex.v[0], vertex.v[1], vertex.v[2]);
	((*scanner).*scanPointCallback)(point);
}

void TREShapeGroup::scanPoints(TCULong index, TCObject *scanner,
							   TREScanPointCallback scanPointCallback,
							   float* matrix)
{
	TREVertex vertex = (*m_vertexStore->getVertices())[index];

	TREModel::transformVertex(vertex, matrix);
	scanPoints(vertex, scanner, scanPointCallback);
}

void TREShapeGroup::unshrinkNormals(float *matrix, float *unshrinkMatrix)
{
	int bit;

	for (bit = 1; (TREShapeType)bit < TRESFirstStrip; bit = bit << 1)
	{
		unshrinkNormals(getIndices((TREShapeType)bit), matrix, unshrinkMatrix);
	}
	for (; (TREShapeType)bit <= TRESLast; bit = bit << 1)
	{
		unshrinkStripNormals(getIndices((TREShapeType)bit),
			getStripCounts((TREShapeType)bit), matrix, unshrinkMatrix);
	}
}

void TREShapeGroup::unshrinkNormals(TCULongArray *indices, float *matrix,
									float *unshrinkMatrix)
{
	if (indices)
	{
		int i;
		int count = indices->getCount();

		for (i = 0; i < count; i++)
		{
			unshrinkNormal((*indices)[i], matrix, unshrinkMatrix);
		}
	}
}

void TREShapeGroup::unshrinkStripNormals(TCULongArray *indices,
										 TCULongArray *stripCounts,
										 float *matrix, float *unshrinkMatrix)
{
	if (indices && stripCounts)
	{
		int i, j;
		int numStrips = stripCounts->getCount();
		int indexOffset = 0;

		for (i = 0; i < numStrips; i++)
		{
			int stripCount = (*stripCounts)[i];
			for (j = 0; j < stripCount; j++)
			{
				unshrinkNormal((*indices)[j + indexOffset], matrix,
					unshrinkMatrix);
			}
			indexOffset += stripCount;
		}
	}
}

void TREShapeGroup::unshrinkNormal(TCULong index, float *matrix,
								   float *unshrinkMatrix)
{
	TREVertexArray *normals = m_vertexStore->getNormals();
	TREVertex normal = (*normals)[index];
	TCVector newNormal = TCVector(normal.v[0], normal.v[1], normal.v[2]);
	float adjust;

	if (!fEq(newNormal.length(), 1.0f))
	{
		printf("Huh?\n");
	}
	newNormal = newNormal.transformNormal(matrix);
	newNormal = newNormal.transformNormal(unshrinkMatrix, false);
	adjust = 1.0f / newNormal.length();
	if (adjust > 1.0f)
	{
		printf("adjust: %f\n", adjust);
	}
	normal.v[0] *= adjust;
	normal.v[1] *= adjust;
	normal.v[2] *= adjust;
	normals->replaceVertex(normal, index);
}

static void invertULongArray(TCULongArray *array, int start = 0, int end = -1)
{
	int i;
	int count = array->getCount();

	if (end == -1)
	{
		end = count;
	}
	for (i = start; i < (end - start) / 2 + start; i++)
	{
		TCULong temp1 = (*array)[i];
		TCULong temp2 = (*array)[end - i - 1 + start];

		array->replaceValue(temp2, i);
		array->replaceValue(temp1, end - i - 1 + start);
	}
}

int TREShapeGroup::flipNormal(int index)
{
	TREVertexArray *vertices = m_vertexStore->getVertices();
	TREVertexArray *normals = m_vertexStore->getNormals();
	TCULongArray *colors = m_vertexStore->getColors();
	TREVertex vertex = (*vertices)[index];
	TREVertex normal = (*normals)[index];

	normal.v[0] = -normal.v[0];
	normal.v[1] = -normal.v[1];
	normal.v[2] = -normal.v[2];
	vertices->addVertex(vertex);
	normals->addVertex(normal);
	if (colors)
	{
		colors->addValue((*colors)[index]);
	}
	return vertices->getCount() - 1;
}

void TREShapeGroup::invertShapes(TCULongArray *oldIndices,
								 TCULongArray *newIndices)
{
	int i;
	int indexCount = oldIndices->getCount();

	for (i = indexCount - 1; i >= 0; i--)
	{
		int index = (*oldIndices)[i];

		newIndices->addValue(flipNormal(index));
	}
}

void TREShapeGroup::invert(void)
{
	if (m_indices)
	{
		int i, j;
		int shapeTypeCount = m_indices->getCount();
		int firstStripIndex = getShapeTypeIndex(TRESFirstStrip);
		int triangleStripIndex = getShapeTypeIndex(TRESTriangleStrip);
		int quadStripIndex = getShapeTypeIndex(TRESQuadStrip);
		int triangleFanIndex = getShapeTypeIndex(TRESTriangleFan);

		for (i = 0; i < shapeTypeCount; i++)
		{
			TCULongArray *theseIndices = (*m_indices)[i];
			TCULongArray *newIndices =
				new TCULongArray(theseIndices->getCount());
			TCULongArray *theseStripCounts = (*m_stripCounts)[i];
			int indexCount = theseIndices->getCount();

			if (i < firstStripIndex)
			{
				invertShapes(theseIndices, newIndices);
//				printULongArray("in: ", theseIndices);
//				printULongArray("in: ", theseIndices);
			}
			else
			{
				if ((m_shapesPresent & TRESTriangleStrip) &&
					i == triangleStripIndex)
				{
					// WRONG!
					//invertShapes(theseIndices, newIndices);
				}
				else if ((m_shapesPresent & TRESQuadStrip) &&
					i == quadStripIndex)
				{
//					printULongArray("qs: ", theseIndices);
					for (j = 0; j < indexCount; j += 2)
					{
						TCULong index1 = (*theseIndices)[j];
						TCULong index2 = (*theseIndices)[j + 1];

						newIndices->addValue(flipNormal(index2));
						newIndices->addValue(flipNormal(index1));
					}
//					printULongArray("qs: ", theseIndices);
				}
				else if ((m_shapesPresent & TRESTriangleFan) &&
					i == triangleFanIndex)
				{
					int numStrips = theseStripCounts->getCount();
					int indexOffset = 0;

//					printULongArray("tf: ", theseIndices);
					for (j = 0; j < numStrips; j++)
					{
						int stripCount = (*theseStripCounts)[j];

						invertULongArray(theseIndices, indexOffset + 1,
							indexOffset + stripCount);
						indexOffset += stripCount;
					}
					for (j = 0; j < indexCount; j++)
					{
						newIndices->addValue(flipNormal((*theseIndices)[j]));
					}
//					printULongArray("tf: ", theseIndices);
				}
			}
			m_indices->replaceObject(newIndices, i);
			newIndices->release();
		}
	}
}

void TREShapeGroup::unMirror(void)
{
	if (m_indices)
	{
		int i, j;
		int shapeTypeCount = m_indices->getCount();
		int firstStripIndex = getShapeTypeIndex(TRESFirstStrip);
		int triangleStripIndex = getShapeTypeIndex(TRESTriangleStrip);
		int quadStripIndex = getShapeTypeIndex(TRESQuadStrip);
		int triangleFanIndex = getShapeTypeIndex(TRESTriangleFan);

		for (i = 0; i < shapeTypeCount; i++)
		{
			TCULongArray *theseIndices = (*m_indices)[i];
			TCULongArray *theseStripCounts = (*m_stripCounts)[i];
			int indexCount = theseIndices->getCount();

			if (i < firstStripIndex)
			{
//				printULongArray("in: ", theseIndices);
				invertULongArray(theseIndices);
//				printULongArray("in: ", theseIndices);
			}
			else
			{
				if ((m_shapesPresent & TRESTriangleStrip) &&
					i == triangleStripIndex)
				{
					if ((*theseStripCounts)[i] % 2)
					{
//						printULongArray("sc: ", theseStripCounts);
						invertULongArray(theseStripCounts);
//						printULongArray("sc: ", theseStripCounts);
					}
					else
					{
						printf("Cannot invert tri strip with even number of "
							"points.\n");
					}
				}
				else if ((m_shapesPresent & TRESQuadStrip) &&
					i == quadStripIndex)
				{
//					printULongArray("qs: ", theseIndices);
					for (j = 0; j < indexCount; j += 2)
					{
						TCULong temp1 = (*theseIndices)[j];
						TCULong temp2 = (*theseIndices)[j + 1];

						theseIndices->replaceValue(temp2, j);
						theseIndices->replaceValue(temp1, j + 1);
					}
//					printULongArray("qs: ", theseIndices);
				}
				else if ((m_shapesPresent & TRESTriangleFan) &&
					i == triangleFanIndex)
				{
					int numStrips = theseStripCounts->getCount();
					int indexOffset = 0;

//					printULongArray("tf: ", theseIndices);
					for (j = 0; j < numStrips; j++)
					{
						int stripCount = (*theseStripCounts)[j];

						invertULongArray(theseIndices, indexOffset + 1,
							indexOffset + stripCount);
						indexOffset += stripCount;
					}
//					printULongArray("tf: ", theseIndices);
				}
			}
		}
	}
}
