#include "ModelViewerWidget.h"
#include <TCFoundation/TCAlert.h>
#include <TCFoundation/TCAlertManager.h>

Statistics::Statistics(QWidget *parent,ModelViewerWidget *modelWidget)
	:QDialog(parent),StatisticsPanel(),
	m_modelWindow(modelWidget),
	m_model(NULL)
{
	setupUi(this);
	connect( okButton, SIGNAL( clicked() ), this, SLOT( hide() ) );
}


Statistics::~Statistics(void)
{
}


void Statistics::show(void)
{
    if (m_modelWindow != NULL && m_modelWindow->getModelViewer() != NULL)
    {
		QString parts,models,triangles,quads,lines,edgelines,conditionallines;
		parts.setNum(m_modelWindow->getModelViewer()->getMainModel()->m_statistics.parts);
		models.setNum(m_modelWindow->getModelViewer()->getMainModel()->m_statistics.models);
		triangles.setNum(m_modelWindow->getModelViewer()->getMainModel()->m_statistics.triangles);
		quads.setNum(m_modelWindow->getModelViewer()->getMainModel()->m_statistics.quads);
		lines.setNum(m_modelWindow->getModelViewer()->getMainModel()->m_statistics.lines);
		edgelines.setNum(m_modelWindow->getModelViewer()->getMainModel()->m_statistics.edgeLines);
		conditionallines.setNum(m_modelWindow->getModelViewer()->getMainModel()->m_statistics.conditionalLines);
		partsLine->setText(parts);
		modelsLine->setText(models);
		trianglesLine->setText(triangles);
		quadsLine->setText(quads);
		linesLine->setText(lines);
		edgeLinesLine->setText(edgelines);
		conditionalLinesLine->setText(conditionallines);
		
    }
    else
    {
    }
	QDialog::show();
}
