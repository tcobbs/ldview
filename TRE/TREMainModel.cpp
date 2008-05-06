#include "TREMainModel.h"
#include "TREVertexStore.h"
#include "TRETransShapeGroup.h"
#include "TREGL.h"
#include "TREGLExtensions.h"
#include "TRESubModel.h"
#include <math.h>
#include <string.h>
#include <gl2ps/gl2ps.h>

#include <TCFoundation/TCDictionary.h>
#include <TCFoundation/TCProgressAlert.h>
#include <TCFoundation/TCLocalStrings.h>

#ifdef WIN32
#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG
#endif // WIN32

#ifndef _NO_TRE_THREADS
#include <boost/thread.hpp>
#include <boost/bind.hpp>
//#define ANTI_DEADLOCK_HACK
#ifdef ANTI_DEADLOCK_HACK
#include <boost/thread/xtime.hpp>
#endif // ANTI_DEADLOCK_HACK
typedef boost::MutexType::scoped_lock ScopedLock;

#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif // __APPLE__
#endif // !_NO_TRE_THREADS

//const GLfloat POLYGON_OFFSET_FACTOR = 0.85f;
//const GLfloat POLYGON_OFFSET_UNITS = 0.0f;
const GLfloat POLYGON_OFFSET_FACTOR = 1.0f;
const GLfloat POLYGON_OFFSET_UNITS = 1.0f;

TCImageArray *TREMainModel::sm_studTextures = NULL;
GLuint TREMainModel::sm_studTextureID = 0;
TREMainModel::TREMainModelCleanup TREMainModel::sm_mainModelCleanup;

TREMainModel::TREMainModelCleanup::~TREMainModelCleanup(void)
{
	TCObject::release(TREMainModel::sm_studTextures);
	TREMainModel::sm_studTextures = NULL;
	// We can't deletet the texture from GL, since there's not likely to be an
	// OpenGL context active at this point in the app shutdown process.
	//if (TREMainModel::sm_studTextureID)
	//{
	//	glDeleteTextures(1, &TREMainModel::sm_studTextureID);
	//}
}

//TREMainModel::TREMainModel(const TREMainModel &other)
//	:TREModel(other),
//	m_loadedModels((TCDictionary *)TCObject::copy(other.m_loadedModels)),
//	m_loadedBFCModels((TCDictionary *)TCObject::copy(other.m_loadedBFCModels)),
//	m_vertexStore((TREVertexStore *)TCObject::copy(other.m_vertexStore)),
//	m_studVertexStore((TREVertexStore *)TCObject::copy(
//		other.m_studVertexStore)),
//	m_coloredVertexStore((TREVertexStore *)TCObject::copy(
//		other.m_coloredVertexStore)),
//	m_coloredStudVertexStore((TREVertexStore *)TCObject::copy(
//		other.m_coloredStudVertexStore)),
//	m_transVertexStore((TREVertexStore *)TCObject::copy(
//		other.m_transVertexStore)),
//	m_color(other.m_color),
//	m_edgeColor(other.m_edgeColor),
//	m_maxRadiusSquared(other.m_maxRadiusSquared),
//	m_edgeLineWidth(other.m_edgeLineWidth),
//	m_studAnisoLevel(other.m_studAnisoLevel),
//	m_abort(false),
//	m_studTextureFilter(other.m_studTextureFilter),
//	m_mainFlags(other.m_mainFlags)
//{
//#ifdef _LEAK_DEBUG
//	strcpy(className, "TREMainModel");
//#endif // _LEAK_DEBUG
//	m_mainModel = this;
//}

TREMainModel::TREMainModel(void)
	:m_alertSender(NULL),
	m_loadedModels(NULL),
	m_loadedBFCModels(NULL),
	m_vertexStore(new TREVertexStore),
	m_studVertexStore(new TREVertexStore),
	m_coloredVertexStore(new TREVertexStore),
	m_coloredStudVertexStore(new TREVertexStore),
	m_transVertexStore(new TREVertexStore),
	m_color(htonl(0x999999FF)),
	m_edgeColor(htonl(0x666658FF)),
	m_maxRadiusSquared(0.0f),
	m_edgeLineWidth(1.0f),
	m_studAnisoLevel(1.0f),
	m_abort(false),
	m_studTextureFilter(GL_LINEAR_MIPMAP_LINEAR),
	m_step(-1)
#ifndef _NO_TRE_THREADS
	, m_threadGroup(NULL)
	, m_workerMutex(NULL)
	, m_workerCondition(NULL)
	, m_sortCondition(NULL)
	, m_conditionalsCondition(NULL)
	, m_exiting(false)
#endif // !_NO_TRE_THREADS
{
#ifdef _LEAK_DEBUG
	strcpy(className, "TREMainModel");
#endif // _LEAK_DEBUG
	m_mainModel = this;
	m_mainFlags.compiled = false;
	m_mainFlags.compiling = false;
	m_mainFlags.removingHiddenLines = false;
	m_mainFlags.cutawayDraw = false;
	m_mainFlags.activeLineJoins = false;
	m_mainFlags.frameSorted = false;
	m_mainFlags.frameSortStarted = false;
	m_mainFlags.frameStarted = false;

	m_mainFlags.compileParts = false;
	m_mainFlags.compileAll = false;
	m_mainFlags.edgeLines = false;
	m_mainFlags.edgesOnly = false;
	m_mainFlags.twoSidedLighting = false;
	m_mainFlags.lighting = false;
	m_mainFlags.useStrips = true;
	m_mainFlags.useFlatStrips = false;
	m_mainFlags.bfc = false;
	m_mainFlags.aaLines = false;
	m_mainFlags.sortTransparent = false;
	m_mainFlags.stipple = false;
	m_mainFlags.wireframe = false;
	m_mainFlags.conditionalLines = false;
	m_mainFlags.smoothCurves = true;
	m_mainFlags.showAllConditional = false;
	m_mainFlags.conditionalControlPoints = false;
	m_mainFlags.studLogo = true;
	m_mainFlags.redBackFaces = false;
	m_mainFlags.greenFrontFaces = false;
	m_mainFlags.lineJoins = false;	// Doesn't work right
	m_mainFlags.drawNormals = false;
	m_mainFlags.stencilConditionals = false;
	m_mainFlags.vertexArrayEdgeFlags = false;
	m_mainFlags.multiThreaded = true;
	m_mainFlags.gl2ps = false;

	m_conditionalsDone = 0;
	m_conditionalsStep = 0;
	memset(m_activeConditionals, 0, sizeof(m_activeConditionals));
	memset(m_activeColorConditionals, 0, sizeof(m_activeColorConditionals));
}

TREMainModel::~TREMainModel(void)
{
}

void TREMainModel::dealloc(void)
{
#ifndef _NO_TRE_THREADS
	if (m_threadGroup)
	{
		ScopedLock lock(*m_workerMutex);

		m_exiting = true;
		m_workerCondition->notify_all();
		lock.unlock();
		m_threadGroup->join_all();
		delete m_threadGroup;
		m_threadGroup = NULL;
		delete m_workerMutex;
		m_workerMutex = NULL;
		delete m_workerCondition;
		m_workerCondition = NULL;
		delete m_sortCondition;
		m_sortCondition = NULL;
		delete m_conditionalsCondition;
		m_conditionalsCondition = NULL;
	}
#endif // !_NO_TRE_THREADS
	uncompile();
	TCObject::release(m_loadedModels);
	TCObject::release(m_loadedBFCModels);
	TCObject::release(m_vertexStore);
	TCObject::release(m_studVertexStore);
	TCObject::release(m_coloredVertexStore);
	TCObject::release(m_coloredStudVertexStore);
	TCObject::release(m_transVertexStore);
	TREModel::dealloc();
}

TCObject *TREMainModel::copy(void) const
{
	return new TREMainModel(*this);
}

TCDictionary *TREMainModel::getLoadedModels(bool bfc)
{
	if (bfc)
	{
		if (!m_loadedBFCModels)
		{
			m_loadedBFCModels = new TCDictionary(0);
		}
		return m_loadedBFCModels;
	}
	else
	{
		if (!m_loadedModels)
		{
			m_loadedModels = new TCDictionary(0);
		}
		return m_loadedModels;
	}
}

void TREMainModel::activateBFC(void)
{
	if (getRedBackFacesFlag() || getGreenFrontFacesFlag())
	{
		bool needColorMaterial = true;

		if (getRedBackFacesFlag() && getGreenFrontFacesFlag())
		{
			needColorMaterial = false;
			glDisable(GL_COLOR_MATERIAL);
		}
		if (getRedBackFacesFlag())
		{
			GLfloat mRed[] = {1.0f, 0.0f, 0.0f, 1.0f};

			if (needColorMaterial)
			{
				glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
			}
			glMaterialfv(GL_BACK, GL_AMBIENT, mRed);
			glMaterialfv(GL_BACK, GL_DIFFUSE, mRed);
		}
		if (getGreenFrontFacesFlag())
		{
			GLfloat mGreen[] = {0.0f, 1.0f, 0.0f, 1.0f};

			if (needColorMaterial)
			{
				glColorMaterial(GL_BACK, GL_AMBIENT_AND_DIFFUSE);
			}
			glMaterialfv(GL_FRONT, GL_AMBIENT, mGreen);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, mGreen);
		}
	}
	else
	{
		// Note that GL_BACK is the default face to cull, and GL_CCW is the
		// default polygon winding.
		glEnable(GL_CULL_FACE);
		if (getTwoSidedLightingFlag() && getLightingFlag())
		{
			glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);
		}
	}
}

void TREMainModel::deactivateBFC(void)
{
	if (getRedBackFacesFlag() && getGreenFrontFacesFlag())
	{
		glEnable(GL_COLOR_MATERIAL);
	}
	else if (getRedBackFacesFlag() || getGreenFrontFacesFlag())
	{
		glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	}
	else
	{
		if (getTwoSidedLightingFlag() && getLightingFlag())
		{
			glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);
		}
		glDisable(GL_CULL_FACE);
	}
}

/*
void TREMainModel::compileTransparent(void)
{
	if (!m_coloredListIDs[TREMTransparent] &&
		m_coloredShapes[TREMTransparent])
	{
		int listID = glGenLists(1);

		glNewList(listID, GL_COMPILE);
		((TRETransShapeGroup *)m_coloredShapes[TREMTransparent])->
			draw(false);
		glEndList();
		m_coloredListIDs[TREMTransparent] = listID;
	}
}
*/

bool TREMainModel::getStencilConditionalsFlag(void)
{
	// Save alpha requires the stencil buffer for another purpose; don't allow
	// stencil conditionals when it's set.
	return m_mainFlags.stencilConditionals != false &&
		!getSaveAlphaFlag();
}

bool TREMainModel::shouldCompileSection(TREMSection section)
{
	if (getStencilConditionalsFlag())
	{
		return section != TREMTransparent;
	}
	else
	{
		return section != TREMConditionalLines && section != TREMTransparent;
	}
}

void TREMainModel::compile(void)
{
	if (!m_mainFlags.compiled)
	{
		int i;
		float numSections = (float)(TREMLast - TREMFirst + 1);

//		TCProgressAlert::send("TREMainModel",
//			TCLocalStrings::get("TREMainModelCompiling"), 0.0f, &m_abort);
		if (!m_abort)
		{
			m_mainFlags.compiling = true;
			for (i = TREMFirst; i <= TREMLast && !m_abort; i++)
			{
				TREMSection section = (TREMSection)i;

				if (shouldCompileSection(section))
				{
					float progress = (float)i / (numSections * 2.0f);

					TCProgressAlert::send("TREMainModel",
						TCLocalStrings::get(_UC("TREMainModelCompiling")),
						progress, &m_abort, this);
					if (!m_abort && isSectionPresent(section, false))
					{
						if (isStudSection(section))
						{
							m_studVertexStore->activate(true);
						}
						else
						{
							m_vertexStore->activate(true);
						}
						if (section == TREMEdgeLines)
						{
							if (getLineJoinsFlag() && m_edgeLineWidth > 1.0f)
							{
//								pointSize(m_edgeLineWidth);
								m_mainFlags.activeLineJoins = true;
//								glEnable(GL_POINT_SMOOTH);
//								glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
							}
						}
						TREModel::compile(section, false);
						m_mainFlags.activeLineJoins = false;
					}
				}
			}
/*
			TREModel::compile(TREMStandard, false);
			TCProgressAlert::send("TREMainModel",
				TCLocalStrings::get(L"TREMainModelCompiling"), 0.2f, &m_abort);
*/
		}
/*
		if (!m_abort)
		{
			if (getBFCFlag())
			{
				TREModel::compile(TREMBFC, false);
			}
			TCProgressAlert::send("TREMainModel",
				TCLocalStrings::get(L"TREMainModelCompiling"), 0.3f, &m_abort);
		}
		if (!m_abort)
		{
			TREModel::compile(TREMLines, false);
			TCProgressAlert::send("TREMainModel",
				TCLocalStrings::get(L"TREMainModelCompiling"), 0.35f, &m_abort);
		}
		if (!m_abort)
		{
			TREModel::compile(TREMEdgeLines, false);
			TCProgressAlert::send("TREMainModel",
				TCLocalStrings::get(L"TREMainModelCompiling"), 0.45f, &m_abort);
		}
*/
		if (!m_abort)
		{
			for (i = TREMFirst; i <= TREMLast && !m_abort; i++)
			{
				TREMSection section = (TREMSection)i;

				if (shouldCompileSection(section))
				{
					float progress = (float)i / (numSections * 2.0f) + 0.5f;

					TCProgressAlert::send("TREMainModel",
						TCLocalStrings::get(_UC("TREMainModelCompiling")),
						progress, &m_abort, this);
					if (!m_abort && isSectionPresent(section, true))
					{
						if (isStudSection(section))
						{
							m_coloredStudVertexStore->activate(true);
						}
						else
						{
							m_coloredVertexStore->activate(true);
						}
						TREModel::compile(section, true);
					}
				}
			}
/*
			TREModel::compile(TREMStandard, true);
			TCProgressAlert::send("TREMainModel",
				TCLocalStrings::get(L"TREMainModelCompiling"), 0.55f, &m_abort);
*/
		}
/*
		if (!m_abort)
		{
			if (getBFCFlag())
			{
				TREModel::compile(TREMBFC, true);
			}
			TCProgressAlert::send("TREMainModel",
				TCLocalStrings::get(L"TREMainModelCompiling"), 0.65f, &m_abort);
		}
		if (!m_abort)
		{
			TREModel::compile(TREMBFC, true);
			TCProgressAlert::send("TREMainModel",
				TCLocalStrings::get(L"TREMainModelCompiling"), 0.7f, &m_abort);
		}
		if (!m_abort)
		{
			TREModel::compile(TREMEdgeLines, true);
			TCProgressAlert::send("TREMainModel",
				TCLocalStrings::get(L"TREMainModelCompiling"), 0.8f, &m_abort);
		}
		if (!m_abort)
		{
			if (m_transVertexStore)
			{
				m_transVertexStore->activate(true);
			}
			if (!getSortTransparentFlag())
			{
				TREModel::compile(TREMTransparent, true);
			}
			TCProgressAlert::send("TREMainModel",
				TCLocalStrings::get(L"TREMainModelCompiling"), 0.9f, &m_abort);
		}
*/
		if (!m_abort)
		{
			m_mainFlags.compiled = true;
			m_mainFlags.compiling = false;
			TCProgressAlert::send("TREMainModel",
				TCLocalStrings::get(_UC("TREMainModelCompiling")), 1.0f,
				&m_abort, this);
//			TCProgressAlert::send("LDrawModelViewer", "Done.", 2.0f);
		}
	}
}

void TREMainModel::recompile(void)
{
	if (m_mainFlags.compiled)
	{
		uncompile();
		m_mainFlags.compiled = false;
	}
	compile();
}

/*
void checkNormals(TREVertexStore *vertexStore)
{
	if (vertexStore)
	{
		TREVertexArray *normals = vertexStore->getNormals();

		if (normals)
		{
			int i;
			int count = normals->getCount();

			for (i = 0; i < count; i++)
			{
				TCVector normal = (*normals)[i].v;

				if (!fEq(normal.lengthSquared(), 1.0))
				{
					MessageBox(NULL, "Bad Normal", "LDView", MB_OK);
				}
			}
		}
	}
}
*/

void TREMainModel::passOnePrep(void)
{
	glPushAttrib(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ENABLE_BIT | GL_STENCIL_BUFFER_BIT);
	// First pass: draw just the color of all the transparent polygons
	// that are just in front of the background.  Don't draw anywhere
	// that has already been drawn to.  This makes sure that subsequent
	// color blend operations don't include the background color.  Also
	// set the stencil to 1 for every pixel we update the alpha on.
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_EQUAL, 0, 0xFFFFFFFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
	glStencilMask(0xFFFFFFFF);
	disable(GL_BLEND);
}

void TREMainModel::passTwoPrep(void)
{
	// Second pass: update just the alpha channel on all transparent
	// polygons that have the background visible through them (which
	// is all the ones we set the stencil to 1 in on pass 1 above).
	// Note that depth testing is disabled, and the blend function
	// is set to allow the alpha channel to build up as transparent
	// polygons overlap.
	glStencilFunc(GL_EQUAL, 1, 0xFFFFFFFF);
	glDisable(GL_DEPTH_TEST);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
}

void TREMainModel::passThreePrep(void)
{
	// Third pass (actual drawing is outside this if statement):
	// Now draw the color component of all the transparent polygons
	// except for the ones drawn in step 1.  (Depth test is set to
	// GL_LESS, so that those don't ge redrawn.)  Other than that
	// depth test setting, everything is configure the same as for
	// standard one-pass transparency.
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
	glDisable(GL_STENCIL_TEST);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
}

int TREMainModel::getNumBackgroundTasks(void)
{
	int numTasks = 0;

	if (backgroundSortNeeded())
	{
		numTasks++;
	}
	if (backgroundConditionalsNeeded())
	{
		numTasks += 32;
	}
	return numTasks;
}

int TREMainModel::getNumWorkerThreads(void)
{
#ifndef _NO_TRE_THREADS
	if (getMultiThreadedFlag())
	{
		int numProcessors = 1;

#if defined(WIN32)
		DWORD processAffinityMask;
		DWORD systemAffinityMask;
		
		if (GetProcessAffinityMask(GetCurrentProcess(), &processAffinityMask,
			&systemAffinityMask))
		{
			numProcessors = 0;

			while (processAffinityMask)
			{
				if (processAffinityMask & 0x01)
				{
					numProcessors++;
				}
				processAffinityMask >>= 1;
			}
		}
#elif defined(__APPLE__)
		numProcessors = (int)MPProcessorsScheduled();
#elif defined(_QT)
		int result = -1;
#ifdef _SC_NPROCESSORS_CONF
		result = (int)sysconf(_SC_NPROCESSORS_CONF);
		if (result > 0)
		{
			numProcessors = result;
		}
#endif // _SC_NPROCESSORS_CONF
#ifdef _SC_NPROCESSORS_ONLN
		result = (int)sysconf(_SC_NPROCESSORS_ONLN);
		if (result > 0)
		{
			numProcessors = result;
		}
#endif // _SC_NPROCESSORS_ONLN
#endif // _QT
		if (numProcessors > 1)
		{
			return std::min(numProcessors - 1, getNumBackgroundTasks());
		}
	}
#endif // !_NO_TRE_THREADS
	return 0;
}

bool TREMainModel::backgroundSortNeeded(void)
{
	TRETransShapeGroup* transShapeGroup =
		(TRETransShapeGroup*)m_coloredShapes[TREMTransparent];

	return (transShapeGroup && getSortTransparentFlag() &&
		!getCutawayDrawFlag());
}

bool TREMainModel::backgroundConditionalsNeeded(void)
{
	return getFlattenConditionalsFlag() && getConditionalLinesFlag();
}

bool TREMainModel::doingBackgroundConditionals(void)
{
	return backgroundConditionalsNeeded() && getMultiThreadedFlag() &&
		getNumBackgroundTasks() > 0 && getNumWorkerThreads() > 0;
}

TCULongArray *TREMainModel::backgroundConditionals(
	TREShapeGroup *shapes,
	int step)
{
	if (shapes)
	{
		TCULongArray *indices = shapes->getIndices(TRESConditionalLine);

		if (indices)
		{
			int subCount = shapes->getIndexCount(TRESConditionalLine) / 2;
			int stepSize = subCount / 32 * 2;
			int stepCount = stepSize;

			if (step == 31)
			{
				stepCount += (subCount % 32) * 2;
			}
			return shapes->getActiveConditionalIndices(indices,
				TCVector::getIdentityMatrix(), stepSize * step, stepCount);
		}
	}
	return NULL;
}

void TREMainModel::backgroundConditionals(int step)
{
	m_activeConditionals[step] =
		backgroundConditionals(m_shapes[TREMConditionalLines], step);
	m_activeColorConditionals[step] = 
		backgroundConditionals(m_coloredShapes[TREMConditionalLines], step);
}

#ifndef _NO_TRE_THREADS

template <class _ScopedLock>
void TREMainModel::nextConditionalsStep(_ScopedLock &lock)
{
	// lock is always locked here.
	int step = m_conditionalsStep;

	m_conditionalsStep++;
	lock.unlock();
	backgroundConditionals(step);
	lock.lock();
	m_conditionalsDone |= 1 << step;
}

template <class _ScopedLock>
bool TREMainModel::workerThreadDoWork(_ScopedLock &lock)
{
	// lock is always locked here.
	if (!m_mainFlags.frameStarted)
	{
		return false;
	}
	if (backgroundConditionalsNeeded() && (m_conditionalsStep < 32))
	{
		nextConditionalsStep(lock);
		if (m_conditionalsDone == 0xFFFFFFFF)
		{
			m_conditionalsCondition->notify_all();
		}
		return true;
	}
	if (!m_mainFlags.frameSortStarted && backgroundSortNeeded())
	{
		TRETransShapeGroup* transShapeGroup =
			(TRETransShapeGroup*)m_coloredShapes[TREMTransparent];

		m_mainFlags.frameSortStarted = true;
		lock.unlock();
		transShapeGroup->backgroundSort();
		lock.lock();
		m_mainFlags.frameSorted = true;
		m_sortCondition->notify_all();
		return true;
	}
	return false;
}

void TREMainModel::workerThreadProc(void)
{
	ScopedLock lock(*m_workerMutex);

	if (!m_mainFlags.frameStarted)
	{
		m_workerCondition->wait(lock);
	}
	while (1)
	{
		if (m_exiting)
		{
			break;
		}
		if (!workerThreadDoWork(lock))
		{
			if (!m_exiting)
			{
#ifdef ANTI_DEADLOCK_HACK
				boost::xtime xt;

				boost::xtime_get(&xt, boost::TIME_UTC);
				// 100,000,000 nsec == 100 msec
				xt.nsec += 100 * 1000000;
				// HACK: If any deadlocks are encountered during testing,
				// ANTI_DEADLOCK_HACK can be defined, and hopefully they'll go
				// away.
				m_workerCondition->timed_wait(lock, xt);
#else // ANTI_DEADLOCK_HACK
				m_workerCondition->wait(lock);
#endif // ANTI_DEADLOCK_HACK
			}
		}
	}
}
#endif // !_NO_TRE_THREADS

void TREMainModel::launchWorkerThreads()
{
#ifndef _NO_TRE_THREADS
	if (m_threadGroup == NULL)
	{
		int workerThreadCount = getNumWorkerThreads();

		if (workerThreadCount > 0)
		{
			m_mainFlags.frameStarted = false;
			m_threadGroup = new boost::thread_group;
			m_workerMutex = new boost::MutexType;
			m_workerCondition = new boost::condition;
			m_sortCondition = new boost::condition;
			m_conditionalsCondition = new boost::condition;
			for (int i = 0; i < workerThreadCount; i++)
			{
				m_threadGroup->create_thread(
					boost::bind(&TREMainModel::workerThreadProc, this));
			}
		}
	}
#endif // !_NO_TRE_THREADS
}

void TREMainModel::triggerWorkerThreads(void)
{
#ifndef _NO_TRE_THREADS
	if (m_workerMutex)
	{
		ScopedLock lock(*m_workerMutex);
		m_mainFlags.frameSorted = false;
		m_mainFlags.frameSortStarted = false;
		m_mainFlags.frameStarted = true;
		m_conditionalsDone = 0;
		m_conditionalsStep = 0;
		m_workerCondition->notify_all();
		memset(m_activeConditionals, 0, sizeof(m_activeConditionals));
		memset(m_activeColorConditionals, 0, sizeof(m_activeColorConditionals));
	}
#endif // !_NO_TRE_THREADS
}

bool TREMainModel::hasWorkerThreads(void)
{
#ifndef _NO_TRE_THREADS
	if (m_workerMutex)
	{
		ScopedLock lock(*m_workerMutex);
		return m_threadGroup != NULL;
	}
#endif // !_NO_TRE_THREADS
	return false;
}

void TREMainModel::waitForSort(void)
{
#ifndef _NO_TRE_THREADS
	if (m_workerMutex)
	{
		ScopedLock lock(*m_workerMutex);
		if (!m_mainFlags.frameSorted)
		{
			m_sortCondition->wait(lock);
		}
	}
#endif // !_NO_TRE_THREADS
}

#ifdef _NO_TRE_THREADS
void TREMainModel::waitForConditionals(int /*step*/)
#else // _NO_TRE_THREADS
void TREMainModel::waitForConditionals(int step)
#endif // !_NO_TRE_THREADS
{
#ifndef _NO_TRE_THREADS
	if (m_workerMutex)
	{
		ScopedLock lock(*m_workerMutex);

		while ((m_conditionalsDone & (1 << step)) == 0)
		{
			if (m_conditionalsStep < 32)
			{
				nextConditionalsStep(lock);
			}
			else
			{
				m_conditionalsCondition->wait(lock);
			}
		}
	}
#endif // !_NO_TRE_THREADS
}

void TREMainModel::draw(void)
{
	GLfloat normalSpecular[4];
	bool multiPass = false;

	treGlGetFloatv(GL_MODELVIEW_MATRIX, m_currentModelViewMatrix);
	treGlGetFloatv(GL_PROJECTION_MATRIX, m_currentProjectionMatrix);
	if (getSaveAlphaFlag() && (!getStippleFlag() || getAALinesFlag()))
	{
		GLint stencilBits;

		multiPass = true;
		glGetIntegerv(GL_STENCIL_BITS, &stencilBits);
		glPushAttrib(GL_DEPTH_BUFFER_BIT | GL_ENABLE_BIT |
			GL_STENCIL_BUFFER_BIT);
		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_ALWAYS, 2, 0xFFFFFFFF);
		glStencilMask(0xFFFFFFFF);
		glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
	}
	glGetLightfv(GL_LIGHT0, GL_SPECULAR, normalSpecular);
	if (m_mainFlags.compileParts || m_mainFlags.compileAll)
	{
		compile();
	}
	launchWorkerThreads();
	triggerWorkerThreads();
	if (getEdgeLinesFlag() && !getWireframeFlag() && getPolygonOffsetFlag())
	{
		glPolygonOffset(POLYGON_OFFSET_FACTOR, POLYGON_OFFSET_UNITS);
		enable(GL_POLYGON_OFFSET_FILL);
	}
	else
	{
		disable(GL_POLYGON_OFFSET_FILL);
	}
	if (getWireframeFlag())
	{
		enableLineSmooth();
	}
	if (getEdgesOnlyFlag())
	{
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	}
	if (!getEdgesOnlyFlag() || !getWireframeFlag())
	{
		drawSolid();
	}
	if (getEdgesOnlyFlag())
	{
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	}
	// Next, disable lighting and draw lines.  First draw default colored lines,
	// which probably don't exist, since color number 16 doesn't often get used
	// for lines.
	if (getSaveAlphaFlag())
	{
		passOnePrep();
		drawLines(1);
		passTwoPrep();
		drawLines(2);
		passThreePrep();
		drawLines(3);
		glPopAttrib();
	}
	else
	{
		drawLines();
	}
	if (getAALinesFlag() && getWireframeFlag())
	{
		// We use glPushAttrib() when we enable line smoothing.
		glPopAttrib();
	}
	if (!getEdgesOnlyFlag() && !getRemovingHiddenLines())
	{
		if (getSaveAlphaFlag() && !getStippleFlag())
		{
			passOnePrep();
			drawTransparent(1);
			passTwoPrep();
			drawTransparent(2);
			passThreePrep();
			drawTransparent(3);
			glPopAttrib();
		}
		else
		{
			drawTransparent();
		}
	}
	if (multiPass)
	{
		glPopAttrib();
	}
	if (backgroundConditionalsNeeded())
	{
		for (int i = 0; i < 32; i++)
		{
			TCObject::release(m_activeConditionals[i]);
			TCObject::release(m_activeColorConditionals[i]);
		}
	}
	m_mainFlags.frameStarted = false;
//	checkNormals(m_vertexStore);
//	checkNormals(m_coloredVertexStore);
}

void TREMainModel::enable(GLenum cap)
{
	if (getGl2psFlag())
	{
		GLint mode = GL2PS_BLEND;

		if (cap == GL_POLYGON_OFFSET_FILL)
		{
			mode = GL2PS_POLYGON_OFFSET_FILL;
		}
		gl2psEnable(mode);
	}
	glEnable(cap);
}

void TREMainModel::disable(GLenum cap)
{
	if (getGl2psFlag())
	{
		GLint mode = GL2PS_BLEND;

		if (cap == GL_POLYGON_OFFSET_FILL)
		{
			mode = GL2PS_POLYGON_OFFSET_FILL;
		}
		gl2psDisable(mode);
	}
	glDisable(cap);
}

void TREMainModel::blendFunc(GLenum sfactor, GLenum dfactor)
{
	if (getGl2psFlag())
	{
		gl2psBlendFunc(sfactor, dfactor);
	}
	glBlendFunc(sfactor, dfactor);
}

void TREMainModel::lineWidth(GLfloat width)
{
	if (getGl2psFlag())
	{
		gl2psLineWidth(width);
	}
	glLineWidth(width);
}

void TREMainModel::pointSize(GLfloat size)
{
	if (getGl2psFlag())
	{
		gl2psPointSize(size);
	}
	glPointSize(size);
}

void TREMainModel::enableLineSmooth(int pass /*= -1*/)
{
	if (getAALinesFlag())
	{
		glPushAttrib(GL_ENABLE_BIT);
		glEnable(GL_LINE_SMOOTH);
		if (pass != 1)
		{
			enable(GL_BLEND);
			if (pass == 2)
			{
				blendFunc(GL_SRC_ALPHA_SATURATE, GL_ONE_MINUS_SRC_ALPHA);
			}
			else
			{
				blendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			}
		}
	}
}

void TREMainModel::drawSolid(void)
{
	bool subModelsOnly = false;

	if (TREShapeGroup::isTransparent(m_color, true))
	{
		subModelsOnly = true;
	}
	// I admit, this is a mess.  But I'm not sure how to make it less of a mess.
	// The various things do need to be drawn separately, and they have to get
	// drawn in a specific order.
	//
	// First, draw all opaque triangles and quads that are color number 16 (the
	// default color inherited from above.  Note that the actual drawing color
	// will generally be changed before each part, since you don't usually use
	// color number 16 when you use a part in your model.

	glColor4ubv((GLubyte*)&m_color);
	m_vertexStore->activate(m_mainFlags.compileAll || m_mainFlags.compileParts);
	TREModel::draw(TREMStandard, false, subModelsOnly);
	if (getStudLogoFlag())
	{
		glEnable(GL_TEXTURE_2D);
		bindStudTexture();
		configureStudTexture();
		m_studVertexStore->activate(m_mainFlags.compileAll ||
			m_mainFlags.compileParts);
		TREModel::draw(TREMStud, false, subModelsOnly);
		glDisable(GL_TEXTURE_2D);
	}
	if (getBFCFlag())
	{
		activateBFC();
		m_vertexStore->activate(m_mainFlags.compileAll ||
			m_mainFlags.compileParts);
		TREModel::draw(TREMBFC, false, subModelsOnly);
		if (getStudLogoFlag())
		{
			glEnable(GL_TEXTURE_2D);
			m_studVertexStore->activate(m_mainFlags.compileAll ||
				m_mainFlags.compileParts);
			TREModel::draw(TREMStudBFC, false, subModelsOnly);
			glDisable(GL_TEXTURE_2D);
		}
		deactivateBFC();
	}
	// Next draw all opaque triangles and quads that were specified with a color
	// number other than 16.  Note that the colored vertex store includes color
	// information for every vertex.
	m_coloredVertexStore->activate(m_mainFlags.compileAll ||
		m_mainFlags.compileParts);
	drawColored(TREMStandard);
	if (getStudLogoFlag())
	{
		glEnable(GL_TEXTURE_2D);
		m_coloredStudVertexStore->activate(m_mainFlags.compileAll ||
			m_mainFlags.compileParts);
		drawColored(TREMStud);
		glDisable(GL_TEXTURE_2D);
	}
	if (getBFCFlag())
	{
		activateBFC();
		m_coloredVertexStore->activate(m_mainFlags.compileAll ||
			m_mainFlags.compileParts);
		drawColored(TREMBFC);
		if (getStudLogoFlag())
		{
			glEnable(GL_TEXTURE_2D);
			m_coloredStudVertexStore->activate(m_mainFlags.compileAll ||
				m_mainFlags.compileParts);
			drawColored(TREMStudBFC);
			glDisable(GL_TEXTURE_2D);
		}
		deactivateBFC();
	}
}

void TREMainModel::drawLines(int pass /*= -1*/)
{
	if (getLightingFlag())
	{
		glDisable(GL_LIGHTING);
	}
	if (getAALinesFlag() && !getWireframeFlag())
	{
		// Note that if we're in wireframe mode, smoothing is already enabled.
		enableLineSmooth(pass);
		// Smooth lines produce odd effects on the edge of transparent surfaces
		// when depth writing is enabled, so disable.
		glDepthMask(GL_FALSE);
	}
	glColor4ubv((GLubyte*)&m_color);
	m_vertexStore->activate(m_mainFlags.compileAll || m_mainFlags.compileParts);
	TREModel::draw(TREMLines);
	// Next, switch to the default edge color, and draw the edge lines.  By
	// definition, edge lines in the original files use the default edge color.
	// However, if parts are flattened, they can contain sub-models of a
	// different color, which can lead to non-default colored edge lines.
	glColor4ubv((GLubyte*)&m_edgeColor);
	lineWidth(m_edgeLineWidth);
	if (getLineJoinsFlag() && m_edgeLineWidth > 1.0f)
	{
		pointSize(m_edgeLineWidth);
		m_mainFlags.activeLineJoins = true;
		glEnable(GL_POINT_SMOOTH);
		glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	}
	TREModel::draw(TREMEdgeLines);
	if (!getStencilConditionalsFlag())
	{
		m_vertexStore->deactivate();
		m_vertexStore->activate(false);
	}
	if (getConditionalLinesFlag())
	{
		TREModel::draw(TREMConditionalLines);
	}
	if (getLineJoinsFlag() && m_edgeLineWidth > 1.0f)
	{
		pointSize(1.0f);
		m_mainFlags.activeLineJoins = false;
	}
	// Next, draw the specific colored lines.  As with the specific colored
	// triangles and quads, every point in the vertex store specifies a color.
	m_coloredVertexStore->activate(m_mainFlags.compileAll ||
		m_mainFlags.compileParts);
	drawColored(TREMLines);
	// Next draw the specific colored edge lines.  Note that if it weren't for
	// the fact that edge lines can be turned off, these could simply be added
	// to the colored lines list.
	drawColored(TREMEdgeLines);
	if (!getStencilConditionalsFlag())
	{
		m_coloredVertexStore->deactivate();
		m_coloredVertexStore->activate(false);
	}
	if (getConditionalLinesFlag())
	{
		drawColored(TREMConditionalLines);
	}
	if (getAALinesFlag() && !getWireframeFlag())
	{
		// Note that if we're in wireframe mode, smoothing was enabled
		// elsewhere, and will therefore be disabled eleswhere.
		glDepthMask(GL_TRUE);
		// We use glPushAttrib() when we enable line smoothing.
		glPopAttrib();
	}
	if (getLightingFlag())
	{
		glEnable(GL_LIGHTING);
	}
}

TREModel *TREMainModel::modelNamed(const char *name, bool bfc)
{
	return (TREMainModel *)getLoadedModels(bfc)->objectForKey(name);
}

void TREMainModel::registerModel(TREModel *model, bool bfc)
{
	getLoadedModels(bfc)->setObjectForKey(model, model->getName());
}

void TREMainModel::setColor(TCULong color, TCULong edgeColor)
{
	m_color = htonl(color);
	m_edgeColor = htonl(edgeColor);
}

TCULong TREMainModel::getColor(void)
{
	return htonl(m_color);
}

TCULong TREMainModel::getEdgeColor(void)
{
	return htonl(m_edgeColor);
}

void TREMainModel::setLightingFlag(bool value)
{
	m_mainFlags.lighting = value;
	m_vertexStore->setLightingFlag(value);
	m_studVertexStore->setLightingFlag(value);
	m_coloredVertexStore->setLightingFlag(value);
	m_coloredStudVertexStore->setLightingFlag(value);
	m_transVertexStore->setLightingFlag(value);
}

void TREMainModel::setTwoSidedLightingFlag(bool value)
{
	m_mainFlags.twoSidedLighting = value;
	m_vertexStore->setTwoSidedLightingFlag(value);
	m_studVertexStore->setTwoSidedLightingFlag(value);
	m_coloredVertexStore->setTwoSidedLightingFlag(value);
	m_coloredStudVertexStore->setTwoSidedLightingFlag(value);
	m_transVertexStore->setTwoSidedLightingFlag(value);
}

void TREMainModel::setShowAllConditionalFlag(bool value)
{
	m_mainFlags.showAllConditional = value;
	m_vertexStore->setShowAllConditionalFlag(value);
	m_studVertexStore->setShowAllConditionalFlag(value);
	m_coloredVertexStore->setShowAllConditionalFlag(value);
	m_coloredStudVertexStore->setShowAllConditionalFlag(value);
	m_transVertexStore->setShowAllConditionalFlag(value);
}

void TREMainModel::setConditionalControlPointsFlag(bool value)
{
	m_mainFlags.conditionalControlPoints = value;
	m_vertexStore->setConditionalControlPointsFlag(value);
	m_studVertexStore->setConditionalControlPointsFlag(value);
	m_coloredVertexStore->setConditionalControlPointsFlag(value);
	m_coloredStudVertexStore->setConditionalControlPointsFlag(value);
	m_transVertexStore->setConditionalControlPointsFlag(value);
}

TCFloat TREMainModel::getMaxRadiusSquared(const TCVector &center)
{
	if (!m_maxRadiusSquared)
	{
		m_center = center;
		scanPoints(this,
			(TREScanPointCallback)&TREMainModel::scanMaxRadiusSquaredPoint,
			TCVector::getIdentityMatrix());
	}
	return m_maxRadiusSquared;
}

void TREMainModel::scanMaxRadiusSquaredPoint(const TCVector &point)
{
	TCFloat rSquared = (point - m_center).lengthSquared();

	if (rSquared > m_maxRadiusSquared)
	{
		m_maxRadiusSquared = rSquared;
	}
}

// By asking for the maximum radius squared, and then returning the square root
// of that, we only have to do one square root for the whole radius calculation.
// Otherwise, we would have to do one for every point.
TCFloat TREMainModel::getMaxRadius(const TCVector &center)
{
	return (TCFloat)sqrt(getMaxRadiusSquared(center));
}

bool TREMainModel::postProcess(void)
{
	int i;
	float numSections = (float)(TREMTransparent - TREMStandard);

	TCProgressAlert::send("TREMainModel",
		TCLocalStrings::get(_UC("TREMainModelProcessing")), 0.0f, &m_abort,
		this);
	if (m_abort)
	{
		return false;
	}
	transferTransparent();
	// Note: I DON'T want to check if the transparent section is present.
	// That's why I'm using < below, instead of <=.
	for (i = TREMFirst; i < TREMTransparent && !m_abort; i++)
	{
		float progress = (float)i / numSections * 0.8f + 0.2f;

		TCProgressAlert::send("TREMainModel",
			TCLocalStrings::get(_UC("TREMainModelProcessing")), progress,
			&m_abort, this);
		if (!m_abort)
		{
			checkSectionPresent((TREMSection)i);
			checkSectionPresent((TREMSection)i, true);
		}
	}
	if (m_abort)
	{
		return false;
	}
/*
	checkDefaultColorPresent();
	TCProgressAlert::send("TREMainModel",
		TCLocalStrings::get(L"TREMainModelProcessing"), 0.3f, &m_abort);
	if (m_abort)
	{
		return false;
	}
	checkStudsPresent();
	TCProgressAlert::send("TREMainModel",
		TCLocalStrings::get(L"TREMainModelProcessing"), 0.3f, &m_abort);
	if (m_abort)
	{
		return false;
	}
	checkBFCPresent();
	TCProgressAlert::send("TREMainModel",
		TCLocalStrings::get(L"TREMainModelProcessing"), 0.4f, &m_abort);
	if (m_abort)
	{
		return false;
	}
	checkDefaultColorLinesPresent();
	TCProgressAlert::send("TREMainModel",
		TCLocalStrings::get(L"TREMainModelProcessing"), 0.45f, &m_abort);
	if (m_abort)
	{
		return false;
	}
	checkEdgeLinesPresent();
	TCProgressAlert::send("TREMainModel",
		TCLocalStrings::get(L"TREMainModelProcessing"), 0.55f, &m_abort);
	if (m_abort)
	{
		return false;
	}
	checkConditionalLinesPresent();
	TCProgressAlert::send("TREMainModel",
		TCLocalStrings::get(L"TREMainModelProcessing"), 0.65f, &m_abort);
	if (m_abort)
	{
		return false;
	}
	checkColoredPresent();
	TCProgressAlert::send("TREMainModel",
		TCLocalStrings::get(L"TREMainModelProcessing"), 0.7f, &m_abort);
	if (m_abort)
	{
		return false;
	}
	checkColoredBFCPresent();
	TCProgressAlert::send("TREMainModel",
		TCLocalStrings::get(L"TREMainModelProcessing"), 0.75f, &m_abort);
	if (m_abort)
	{
		return false;
	}
	checkColoredLinesPresent();
	TCProgressAlert::send("TREMainModel",
		TCLocalStrings::get(L"TREMainModelProcessing"), 0.8f, &m_abort);
	if (m_abort)
	{
		return false;
	}
	checkColoredEdgeLinesPresent();
	TCProgressAlert::send("TREMainModel",
		TCLocalStrings::get(L"TREMainModelProcessing"), 0.9f, &m_abort);
	if (m_abort)
	{
		return false;
	}
	checkColoredConditionalLinesPresent();
*/
	TCProgressAlert::send("TREMainModel",
		TCLocalStrings::get(_UC("TREMainModelProcessing")), 1.0f, &m_abort,
		this);
	if (m_abort)
	{
		return false;
	}

	if (getCompilePartsFlag() || getCompileAllFlag())
	{
		compile();
	}

	return !m_abort;
}

// We have to remove all the transparent objects from the whole tree after we've
// completely built the tree.  Before it is completely built, we don't know
// which items are transparent.  The actual removal is complicated.
//
// First, any geometry that has a specific color specified (and thus is in a
// TREColoredShapeGroup at the point this method is called) is simply removed
// from its parent model completely, and then added to the transparent triangle
// list (after being transformed into its coordinate system).  Strips, fans,
// and quads are all split into triangles.
//
// Next, any "default color" geometry that will in the end by transparent will
// be transferred to the triangle list (with coordinate transformation and
// strip/quad splitting).  When the non-transparent portions are drawn, they
// will completely skip default colored geomtry when the default color is
// transparent.
void TREMainModel::transferTransparent(void)
{
	SectionList sectionList;

	sectionList.push_back(TREMStandard);
	sectionList.push_back(TREMStud);
	if (getBFCFlag())
	{
		sectionList.push_back(TREMBFC);
		sectionList.push_back(TREMStudBFC);
	}
	transferPrep();
	transferTransparent(sectionList);
	for (size_t i = 1; i < m_transStepCounts.size(); i++)
	{
		m_transStepCounts[i] += m_transStepCounts[i - 1];
	}
	TRETransShapeGroup *transShapes =
		(TRETransShapeGroup *)m_coloredShapes[TREMTransparent];

	if (transShapes)
	{
		transShapes->setStepCounts(m_transStepCounts);
	}
}

void TREMainModel::transferTransparent(const SectionList &sectionList)
{
	const TCFloat *matrix = TCVector::getIdentityMatrix();
	SectionList::const_iterator it;

	for (it = sectionList.begin(); it != sectionList.end(); it++)
	{
		TREShapeGroup *shapeGroup = m_shapes[*it];
		TREColoredShapeGroup *coloredShapeGroup = m_coloredShapes[*it];

		if (shapeGroup)
		{
			shapeGroup->transferTransparent(m_color, matrix);
		}
		if (coloredShapeGroup)
		{
			coloredShapeGroup->transferColoredTransparent(matrix);
		}
	}
	if (m_subModels)
	{
		int i;
		int count = m_subModels->getCount();

		for (i = 0; i < count; i++)
		{
			updateModelTransferStep(i);
			for (SectionList::const_iterator it = sectionList.begin();
				it != sectionList.end(); it++)
			{
				TRESubModel *subModel = (*m_subModels)[i];

				subModel->transferTransparent(m_color, *it, matrix);
				subModel->transferColoredTransparent(*it, matrix);
			}
		}
	}
	for (it = sectionList.begin(); it != sectionList.end(); it++)
	{
		TREModel::cleanupTransparent(*it);
	}
}

// This should really be modified to work in the sub-models, so that if sorting
// isn't enabled, transparent bits get drawn as just another standard pass,
// instead of all being thrown into the main model.
void TREMainModel::addTransparentTriangle(TCULong color,
										  const TCVector vertices[],
										  const TCVector normals[],
										  const TCVector *textureCoords)
{
	if (!m_coloredShapes[TREMTransparent])
	{
		m_coloredShapes[TREMTransparent] = new TRETransShapeGroup;
		m_coloredShapes[TREMTransparent]->setMainModel(this);
		m_coloredShapes[TREMTransparent]->setVertexStore(m_transVertexStore);
	}
	if (getStudLogoFlag() && getStudTextures())
	{
		if (textureCoords)
		{
			m_coloredShapes[TREMTransparent]->addTriangle(color, vertices,
				normals, textureCoords);
		}
		else
		{
			static TCVector zeroTextureCoords[3] =
			{
				TCVector(0.0, 0.0, 0.0),
				TCVector(0.0, 0.0, 0.0),
				TCVector(0.0, 0.0, 0.0)
			};

			m_coloredShapes[TREMTransparent]->addTriangle(color, vertices,
				normals, zeroTextureCoords);
		}
	}
	else
	{
		m_coloredShapes[TREMTransparent]->addTriangle(color, vertices, normals);
	}
	if (m_transStepCounts.size() <= (size_t)m_transferStep)
	{
		m_transStepCounts.resize(m_transferStep + 1);
	}
	m_transStepCounts[m_transferStep] += 3;
}

bool TREMainModel::onLastStep(void)
{
	return m_step == -1 || m_step == m_numSteps - 1;
}

void TREMainModel::drawTransparent(int pass /*= -1*/)
{
	if (m_coloredShapes[TREMTransparent])
	{
		GLfloat specular[] = {0.75f, 0.75f, 0.75f, 1.0f};
		GLfloat oldSpecular[4];
		GLfloat oldShininess;

		if (getStudLogoFlag())
		{
			glEnable(GL_TEXTURE_2D);
			bindStudTexture();
			// Don't allow mip-mapping for transparent textures; it might cause
			// artifacts.
			configureStudTexture(false);
		}
		glGetMaterialfv(GL_FRONT, GL_SHININESS, &oldShininess);
		glGetMaterialfv(GL_FRONT, GL_SPECULAR, oldSpecular);
		m_transVertexStore->activate(!m_mainFlags.sortTransparent);
		if (getStippleFlag())
		{
			glEnable(GL_POLYGON_STIPPLE);
		}
		else if (!getCutawayDrawFlag())
		{
			if (pass != 1)
			{
				enable(GL_BLEND);
				if (pass == 2)
				{
					blendFunc(GL_SRC_ALPHA_SATURATE, GL_ONE_MINUS_SRC_ALPHA);
				}
				else
				{
					blendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				}
				glDepthMask(GL_FALSE);
			}
		}
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 128.0f);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
		if (!getWireframeFlag() && getPolygonOffsetFlag())
		{
			if (getEdgeLinesFlag())
			{
				disable(GL_POLYGON_OFFSET_FILL);
//				glPolygonOffset(0.0f, 0.0f);
			}
			else
			{
				glPolygonOffset(-POLYGON_OFFSET_FACTOR, -POLYGON_OFFSET_UNITS);
				enable(GL_POLYGON_OFFSET_FILL);
			}
		}
		if (onLastStep() && m_coloredListIDs[TREMTransparent])
		{
			glCallList(m_coloredListIDs[TREMTransparent]);
		}
		else
		{
			((TRETransShapeGroup *)m_coloredShapes[TREMTransparent])->
				draw(getSortTransparentFlag() && !getCutawayDrawFlag() &&
				pass < 2);
		}
		if (getStudLogoFlag())
		{
			glDisable(GL_TEXTURE_2D);
		}
		disable(GL_POLYGON_OFFSET_FILL);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, oldSpecular);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, oldShininess);
		if (getStippleFlag())
		{
			glDisable(GL_POLYGON_STIPPLE);
		}
		else if (!getCutawayDrawFlag())
		{
			disable(GL_BLEND);
			glDepthMask(GL_TRUE);
		}
	}
}

bool TREMainModel::shouldLoadConditionalLines(void)
{
	return (m_mainFlags.edgeLines && m_mainFlags.conditionalLines) ||
		m_mainFlags.smoothCurves;
}

void TREMainModel::bindStudTexture(void)
{
	if (!sm_studTextureID && sm_studTextures)
	{
		int i;
		int count = sm_studTextures->getCount();

		glGenTextures(1, (GLuint*)&sm_studTextureID);
		glBindTexture(GL_TEXTURE_2D, sm_studTextureID);
		for (i = 0; i < count; i++)
		{
			TCImage *texture = (*sm_studTextures)[i];

			if (texture)
			{
				int textureSize = texture->getWidth();

				glTexImage2D(GL_TEXTURE_2D, i, 4, textureSize, textureSize, 0,
					GL_RGBA, GL_UNSIGNED_BYTE, texture->getImageData());
			}
		}
	}
	if (sm_studTextureID)
	{
		glBindTexture(GL_TEXTURE_2D, sm_studTextureID);
	}
}

void TREMainModel::configureStudTexture(bool allowMipMap)
{
	if (sm_studTextureID)
	{
		glBindTexture(GL_TEXTURE_2D, sm_studTextureID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		if (m_studTextureFilter == GL_NEAREST_MIPMAP_NEAREST ||
			m_studTextureFilter == GL_NEAREST_MIPMAP_LINEAR ||
			m_studTextureFilter == GL_NEAREST)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}
		else
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		if (allowMipMap)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
				m_studTextureFilter);
		}
		else
		{
			if (m_studTextureFilter == GL_NEAREST)
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
					GL_NEAREST);
			}
			else
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
					GL_LINEAR);
			}
		}
		if (TREGLExtensions::haveAnisoExtension())
		{
			GLfloat aniso = 1.0f;

			if (m_studTextureFilter == GL_LINEAR_MIPMAP_LINEAR)
			{
				aniso = m_studAnisoLevel;
			}
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT,
				aniso);
		}
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	}
}

// NOTE: static function
void TREMainModel::loadStudTexture(const char *filename)
{
	if (!sm_studTextures)
	{
		TCImage *mainImage = new TCImage;

		mainImage->setFlipped(true);
		mainImage->setLineAlignment(4);
		if (mainImage->loadFile(filename))
		{
			loadStudMipTextures(mainImage);
		}
		mainImage->release();
	}
}

// NOTE: static function
void TREMainModel::loadStudMipTextures(TCImage *mainImage)
{
	int i;
	int yPosition = 0;
	int thisSize = mainImage->getWidth();
	int imageHeight = mainImage->getHeight();

	TCObject::release(sm_studTextures);
	sm_studTextures = new TCImageArray(8);
	for (i = 0; thisSize > 0 && yPosition + thisSize <= imageHeight; i++)
	{
		TCImage *texture = mainImage->createSubImage(0, yPosition,
			thisSize, thisSize);
		sm_studTextures->addObject(texture);
		texture->release();
		yPosition += thisSize;
		thisSize /= 2;
	}
}

// NOTE: static function
void TREMainModel::setStudTextureData(TCByte *data, long length)
{
	if (!sm_studTextures)
	{
		TCImage *mainImage = new TCImage;

		mainImage->setFlipped(true);
		mainImage->setLineAlignment(4);
		if (mainImage->loadData(data, length))
		{
			loadStudMipTextures(mainImage);
		}
		mainImage->release();
	}
}

// NOTE: static function
void TREMainModel::setRawStudTextureData(TCByte *data, long length)
{
	if (!sm_studTextures)
	{
		TCImage *mainImage = new TCImage;
		int rowSize;

		mainImage->setFlipped(true);
		mainImage->setLineAlignment(4);
		mainImage->setDataFormat(TCRgba8);
		mainImage->setSize(128, 255);
		mainImage->allocateImageData();
		rowSize = mainImage->getRowSize();
		if (length == rowSize * 255)
		{
			TCByte *imageData = mainImage->getImageData();
			int i;

			for (i = 0; i < 255; i++)
			{
				memcpy(imageData + rowSize * (254 - i), data + rowSize * i,
					rowSize);
			}
			loadStudMipTextures(mainImage);
		}
		mainImage->release();
	}
}

void TREMainModel::openGlWillEnd(void)
{
	uncompile();
	m_vertexStore->openGlWillEnd();
	m_studVertexStore->openGlWillEnd();
	m_coloredVertexStore->openGlWillEnd();
	m_coloredStudVertexStore->openGlWillEnd();
	m_transVertexStore->openGlWillEnd();
	if (sm_studTextureID)
	{
		glDeleteTextures(1, &TREMainModel::sm_studTextureID);
		sm_studTextureID = 0;
	}
}

void TREMainModel::finish(void)
{
	flattenNonUniform();
	findLights();
	flattenConditionals();
	if (m_stepCounts.size() > 0)
	{
		if (m_subModels->getCount() > m_stepCounts.back())
		{
			m_numSteps = (int)m_stepCounts.size() + 1;
		}
		else
		{
			m_numSteps = (int)m_stepCounts.size();
		}
	}
	else
	{
		m_numSteps = 1;
	}
}

void TREMainModel::addLight(const TCVector &location, TCULong color)
{
	m_lightLocations.push_back(location);
	m_lightColors.push_back(color);
}

void TREMainModel::flattenConditionals(void)
{
	if (doingBackgroundConditionals())
	{
		setup(TREMConditionalLines);
		m_shapes[TREMConditionalLines]->getVertexStore()->setup();
		setupColored(TREMConditionalLines);
		m_coloredShapes[TREMConditionalLines]->getVertexStore()->setupColored();
		transferPrep();
		TREModel::flattenConditionals(TCVector::getIdentityMatrix(), 0, false);
		removeConditionals();
	}
}

void TREMainModel::transferPrep(void)
{
	m_transferStep = 0;
}

void TREMainModel::updateModelTransferStep(
	int subModelIndex,
	bool isConditionals /*= false*/)
{
	if (m_stepCounts.size() > (size_t)m_transferStep)
	{
		if (m_stepCounts[m_transferStep] <= subModelIndex)
		{
			if (isConditionals)
			{
				TREShapeGroup *coloredShapeGroup =
					m_coloredShapes[TREMConditionalLines];
				coloredShapeGroup->updateConditionalsStepCount(m_transferStep);
			}
			m_transferStep++;
		}
	}
}

void TREMainModel::setStep(int value)
{
	m_step = value;
	TRETransShapeGroup *transShapes =
		(TRETransShapeGroup *)m_coloredShapes[TREMTransparent];

	if (transShapes)
	{
		transShapes->stepChanged();
	}
}
