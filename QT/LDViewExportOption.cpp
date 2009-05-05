#include "qt4wrapper.h"
#include "Preferences.h"

#include <qstring.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qgroupbox.h>
#include <qpushbutton.h>
#include "misc.h"
#include "LDViewExportOption.h"
#include <LDLib/LDUserDefaultsKeys.h>
#include <LDExporter/LDExporter.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qtooltip.h>

LDViewExportOption::LDViewExportOption(LDrawModelViewer *modelViewer)
	:ExportOptionPanel(),
	m_modelViewer(modelViewer),
	m_exporter(NULL),
	m_sv(NULL),
	m_box(NULL),
	m_origType(LDrawModelViewer::ETPov)
{
    connect( okButton, SIGNAL( clicked() ), this, SLOT( doOk() ) );
    connect( cancelButton, SIGNAL( clicked() ), this, SLOT( doCancel() ) );
    connect( resetButton, SIGNAL( clicked() ), this, SLOT( doReset() ) );

	m_sv = new QScrollView(this, "scrollview");
	m_sv->setVScrollBarMode(QScrollView::AlwaysOn);
#if (QT_VERSION >>16)==3
	layouttop->addWidget(m_sv);
#endif
}

void LDViewExportOption::populate(void)
{
	QWidget *parent;
	parent = m_box;
    LDExporterSettingList &settings = m_exporter->getSettings();
    LDExporterSettingList::iterator it;

	if (m_box != NULL)
	{
		m_sv->removeChild(m_box);
		m_sv->adjustSize();
		delete m_box;
	}
	m_box = new QVBox(m_sv->viewport());
	m_box->setMargin(11);
	m_box->setSpacing(4);
	QVBoxLayout *vbl= NULL;
    std::stack<int> groupSizes;
	std::stack<QWidget *> parents;
    int groupSize = 0;
    for (it = settings.begin(); it != settings.end(); it++)
    {
        bool inGroup = groupSize > 0;

        if (groupSize > 0)
        {
            groupSize--;
            if (groupSize == 0)
            {
                // We just got to the end of a group, so pop the previous
                // groupSize value off the groupSizes stack.
                groupSize = groupSizes.top();
                groupSizes.pop();
				parent = parents.top();
				parents.pop();
				vbl = new QVBoxLayout(parent->layout());
				//vbl = NULL;
            }
        }
        if (it->getGroupSize() > 0)
        {
            // This item is the start of a group.
            if (inGroup)
            {
                // At the beginning of this iteration we were in a group, so
                // use a bool setting instead of a group setting.
				QString qstmp;
				ucstringtoqstring(qstmp,it->getName());

				QCheckBox *check;
				check = new QCheckBox(qstmp,parent,qstmp);
				check->setChecked(it->getBoolValue());
				m_settings[&*it] = check;
				if (vbl) vbl->addWidget(check);
            }
            else
            {
                // Top level group; use a group setting.
				QString qstmp;
				ucstringtoqstring(qstmp,it->getName());
				QGroupBox *gb;
				gb = new QGroupBox (qstmp, m_box, qstmp);
				gb->setColumnLayout(0, Qt::Vertical );
				gb->layout()->setSpacing( 4 );
				gb->layout()->setMargin( 11 );
				vbl = new QVBoxLayout(gb->layout());
				parent=gb;
            }
			parents.push(parent);
            // We're now in a new group, so push the current groupSize onto
            // the groupSizes stack.
            groupSizes.push(groupSize);
            // Update groupSize based on the new group's size.
            groupSize = it->getGroupSize();
        }
        else
        {
            // This setting isn't the start of a group; add the appropriate type
            // of option UI to the canvas.
			QString qstmp;
			ucstringtoqstring(qstmp,it->getName());
			QHBox *hbox;
			QVBox *vbox;
			QLineEdit *li;
			QLabel *label;
			QHBox *hbox2;
			QCheckBox *check;
			hbox = new QHBox(parent);
			hbox->setSpacing(4);
            switch (it->getType())
            {
            case LDExporterSetting::TBool:
				check = new QCheckBox(qstmp,hbox,qstmp);
				check->setChecked(it->getBoolValue());
				m_settings[&*it] = check;
                break;
            case LDExporterSetting::TFloat:
            case LDExporterSetting::TLong:
				// Long and float are intentionally handeled the same.
				label = new QLabel(qstmp,hbox);
				li = new QLineEdit(qstmp,hbox);
				ucstringtoqstring(qstmp,it->getStringValue());
				li->setText(qstmp);
				m_settings[&*it] = li;
                break;
            case LDExporterSetting::TString:
				vbox = new QVBox(hbox);
				vbox->setSpacing(4);
				label = new QLabel(qstmp,vbox);
				hbox2 = new QHBox(vbox);
				hbox2->setSpacing(4);
				li = new QLineEdit(qstmp,hbox2);
				ucstringtoqstring(qstmp,it->getStringValue());
				li->setText(qstmp);
				m_settings[&*it] = li;
				if (it->isPath())
				{
					QPushButton *but = new QPushButton(hbox2);
					but->setText(TCObject::ls("LDXBrowse..."));
				}
                break;
            case LDExporterSetting::TEnum:
				vbox = new QVBox(hbox);
				vbox->setSpacing(4);
                label = new QLabel(qstmp, vbox);
				QComboBox *combo;
				combo = new QComboBox(vbox);
				for (size_t i = 0; i < it->getOptions().size(); i++)
				{
					ucstringtoqstring(qstmp,it->getOptions()[i]);
					combo->insertItem(qstmp,-1);
				}
				combo->setCurrentItem(it->getSelectedOption());
				m_settings[&*it] = combo;
                break;
            default:
                throw "not implemented";
            }
			if (it->getTooltip().size() > 0)
			{
				ucstringtoqstring(qstmp, it->getTooltip());
				QToolTip::add(hbox, qstmp);
			}
			if (vbl) vbl->addWidget(hbox);
        }
	}
	m_sv->addChild(m_box);
	adjustSize();
	m_sv->adjustSize();
	m_sv->viewport()->adjustSize();
	resize(width() + m_box->width() - m_sv->visibleWidth(), height());
	setFixedWidth(width());
}

LDViewExportOption::~LDViewExportOption() { }

void LDViewExportOption::doOk(void)
{
	SettingsMap::const_iterator it;
	ucstring value;

	for (it = m_settings.begin(); it != m_settings.end(); it++)
	{
		LDExporterSetting *setting = it->first;

		switch (setting->getType())
		{
		case LDExporterSetting::TBool:
			setting->setValue(((QCheckBox *)it->second)->isChecked(), true);
			break;
		case LDExporterSetting::TLong:
		case LDExporterSetting::TFloat:
		case LDExporterSetting::TString:
			qstringtoucstring(value, ((QLineEdit *)it->second)->text());
			setting->setValue(value.c_str(), true);
			break;
		case LDExporterSetting::TEnum:
			setting->selectOption(((QComboBox *)it->second)->currentItem(),
				true);
			break;
		default:
			// No default, but gets rid of warnings.
			break;
		}
	}
	// Reset the export type back to its original setting.
	m_modelViewer->getExporter((LDrawModelViewer::ExportType)m_origType);
	accept();
}

void LDViewExportOption::doCancel(void)
{
	// Reset the export type back to its original setting.
	m_modelViewer->getExporter((LDrawModelViewer::ExportType)m_origType);
	reject();
}

void LDViewExportOption::doReset(void)
{
	SettingsMap::const_iterator it;
	QString value;

	for (it = m_settings.begin(); it != m_settings.end(); it++)
	{
		LDExporterSetting *setting = it->first;

		setting->reset();
		switch (setting->getType())
		{
		case LDExporterSetting::TBool:
			((QCheckBox *)it->second)->setChecked(setting->getBoolValue());
			break;
		case LDExporterSetting::TLong:
		case LDExporterSetting::TFloat:
		case LDExporterSetting::TString:
			ucstringtoqstring(value, setting->getStringValue());
			((QLineEdit *)it->second)->setText(value);
			break;
		case LDExporterSetting::TEnum:
			((QComboBox *)it->second)->setCurrentItem(
				setting->getSelectedOption());
			break;
		default:
			// No default, but gets rid of warnings.
			break;
		}
	}
}
/*
void LDViewExportOption::doTypeBoxActivated(void)
{
	m_exporter = m_modelViewer->getExporter(
		(LDrawModelViewer::ExportType)(typeBox->currentItem() +
		LDrawModelViewer::ETFirst));
	populate();
}

void LDViewExportOption::populateTypeBox(void)
{
	LDrawModelViewer::ExportType saveExportType =
		m_modelViewer->getExportType();

	for (int i = LDrawModelViewer::ETFirst; i <= LDrawModelViewer::ETLast; i++)
	{
		const LDExporter *exporter = m_modelViewer->getExporter(
			(LDrawModelViewer::ExportType)i);

		if (exporter != NULL)
		{
			ucstring fileType = exporter->getTypeDescription();
			QString qsFileType;

			ucstringtoqstring(qsFileType, fileType);
			typeBox->insertItem(qsFileType);
		}
	}
	typeBox->setCurrentItem(saveExportType - LDrawModelViewer::ETFirst);
	m_exporter = m_modelViewer->getExporter(saveExportType);
}
*/
int LDViewExportOption::exec(void)
{
	m_origType = m_modelViewer->getExportType();
	m_exporter = m_modelViewer->getExporter(LDrawModelViewer::ETPov);
//	populateTypeBox();
	populate();
	return ExportOptionPanel::exec();
}

