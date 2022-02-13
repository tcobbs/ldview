#ifndef __LDVIEWStatistics_H__
#define __LDVIEWStatistics_H__

#include "ui_StatisticsPanel.h"
#include <QDialog>
#include <QCloseEvent>

class ModelViewerWidget;

class Statistics: public QDialog , Ui::StatisticsPanel
{
public:
	Statistics(QWidget *parent, ModelViewerWidget *modelWidget);
	~Statistics(void);

    ModelViewerWidget *m_modelWindow;
    LDLMainModel *m_model;

public slots:
	void show(void);
};

#endif

