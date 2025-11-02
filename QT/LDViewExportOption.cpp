#include "Preferences.h"

#include "misc.h"
#include "LDViewExportOption.h"
#include <LDLib/LDUserDefaultsKeys.h>
#include <LDExporter/LDExporter.h>
#include <QFileDialog>
#include <QToolTip>

LDViewExportOption::LDViewExportOption(QWidget *parent,LDrawModelViewer *modelViewer,LDrawModelViewer::ExportType type)
	:QDialog(parent),ExportOptionPanel(),
	m_modelViewer(modelViewer),
	m_exporter(NULL),
	m_box(NULL),
	m_origType(type)
{
	setupUi(this);
	connect( okButton, SIGNAL( clicked() ), this, SLOT( doOk() ) );
	connect( cancelButton, SIGNAL( clicked() ), this, SLOT( doCancel() ) );
	connect( resetButton, SIGNAL( clicked() ), this, SLOT( doReset() ) );
}

void LDViewExportOption::populate(void)
{
	QWidget *parent;
	parent = m_box;
	LDExporterSettingList &settings = m_exporter->getSettings();
	LDExporterSettingList::iterator it;

	if (m_box != NULL)
	{
//		scrollArea->removeChild(m_box);
		scrollArea->adjustSize();
		delete m_box;
	}
	m_box = new QWidget(scrollArea);
	m_box->setObjectName("m_box");
	m_lay = new QVBoxLayout();
	m_lay->setObjectName("m_lay");
#if QT_VERSION < 0x60000
	m_lay->setMargin(11);
#endif
	m_lay->setSpacing(4);
	m_box->setLayout(m_lay);
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
//				vbl = new QVBoxLayout(parent->layout());
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
				check = new QCheckBox(qstmp,parent);
				check->setObjectName(qstmp);
				check->setChecked(it->getBoolValue());
				m_settings[&*it] = check;
				if (vbl)
				{
					vbl->addWidget(check);
					m_groups[vbl][&*it] = check;
				}
			}
			else
			{
				// Top level group; use a group setting.
				if (vbl)
				{
					QSpacerItem *sp = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
					QHBoxLayout *hbox;
					QPushButton *rg;
					hbox = new QHBoxLayout();
					rg = new QPushButton("Reset Group");
					hbox->addItem(sp);
					hbox->addWidget(rg);
					rg->setObjectName("Reset Group");
					vbl->addLayout(hbox);
					connect( rg, SIGNAL( clicked() ), this, SLOT( doResetGroup() ) );
				}
				QString qstmp;
				ucstringtoqstring(qstmp,it->getName());
				QGroupBox *gb;
				gb = new QGroupBox (qstmp, m_box);
				gb->setObjectName(qstmp);
				m_lay->addWidget(gb);
				vbl = new QVBoxLayout(gb);
				gb->setLayout(vbl);
				parent=gb;
				if (it->getType() == LDExporterSetting::TBool)
				{
					gb->setCheckable(true);
					gb->setChecked(it->getBoolValue());
					m_settings[&*it] = gb;
					m_groups[vbl][&*it] = gb;
				}
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
			QHBoxLayout *hbox;
			QVBoxLayout *vbox;
			QLineEdit *li;
			QLabel *label;
			QHBoxLayout *hbox2;
			QCheckBox *check;
			hbox = new QHBoxLayout();
			hbox->setSpacing(4);
			switch (it->getType())
			{
			case LDExporterSetting::TBool:
				check = new QCheckBox(qstmp);
				check->setChecked(it->getBoolValue());
				check->setObjectName(qstmp);
				hbox->addWidget(check);
				m_settings[&*it] = check;
				if (vbl != NULL)
				{
					m_groups[vbl][&*it] = check;
				}
				break;
			case LDExporterSetting::TFloat:
			case LDExporterSetting::TLong:
				// Long and float are intentionally handeled the same.
				label = new QLabel(qstmp);
				label->setObjectName(qstmp);
				hbox->addWidget(label);
				li = new QLineEdit(qstmp);
				li->setObjectName(qstmp);
				hbox->addWidget(li);
				ucstringtoqstring(qstmp,it->getStringValue());
				li->setText(qstmp);
				m_settings[&*it] = li;
				if (vbl != NULL)
				{
					m_groups[vbl][&*it] = li;
				}
				break;
			case LDExporterSetting::TString:
				vbox = new QVBoxLayout();
				vbox->setSpacing(4);
				hbox->addLayout(vbox);
				label = new QLabel(qstmp);
				vbox->addWidget(label);
				hbox2 = new QHBoxLayout();
				hbox2->setSpacing(4);
				vbox->addLayout(hbox2);
				li = new QLineEdit(qstmp);
				hbox2->addWidget(li);
				ucstringtoqstring(qstmp,it->getStringValue());
				li->setText(qstmp);
				m_settings[&*it] = li;
				if (vbl != NULL)
				{
					m_groups[vbl][&*it] = li;
				}
				if (it->isPath())
				{
					QPushButton *but = new QPushButton();
					but->setText(TCObject::ls("LDXBrowse..."));
					hbox2->addWidget(but);
					connect( but, SIGNAL( clicked() ), this, SLOT( doBrowse()));
					m_button[but]=li;
				}
				break;
			case LDExporterSetting::TEnum:
				vbox = new QVBoxLayout();
				vbox->setSpacing(4);
				hbox->addLayout(vbox);
				label = new QLabel(qstmp);
		vbox->addWidget(label);
				QComboBox *combo;
				combo = new QComboBox();
				vbox->addWidget(combo);
				for (size_t i = 0; i < it->getOptions().size(); i++)
				{
					ucstringtoqstring(qstmp,it->getOptions()[i]);
					combo->addItem(qstmp);
				}
				combo->setCurrentIndex(it->getSelectedOption());
				m_settings[&*it] = combo;
				if (vbl != NULL)
				{
					m_groups[vbl][&*it] = combo;
				}
				break;
			default:
				throw "not implemented";
			}
			if (it->getTooltip().size() > 0)
			{
				ucstringtoqstring(qstmp, it->getTooltip());
				//QToolTip::add(hbox, qstmp);
			}
			if (vbl) vbl->addLayout(hbox);
			else m_lay->addLayout(hbox);
		}
	}
	if (vbl)
	{
		QSpacerItem *sp = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
		QHBoxLayout *hbox;
		QPushButton *rg;
		hbox = new QHBoxLayout();
		rg = new QPushButton("Reset Group");
		rg->setObjectName("Reset Group");
		hbox->addItem(sp);
		hbox->addWidget(rg);
		vbl->addLayout(hbox);
		connect( rg, SIGNAL( clicked() ), this, SLOT( doResetGroup() ) );
	}

	m_lay->addStretch();
	scrollArea->setWidget(m_box);
	m_box->adjustSize();
	scrollArea->adjustSize();
//	resize(width() + m_box->width() - scrollArea->visibleWidth(), height());
//	setFixedWidth(width());
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
			if (strcmp(it->second->metaObject()->className(), "QGroupBox") == 0)
			{
				setting->setValue(((QGroupBox *)it->second)->isChecked(), true);
			}
			else
			{
				setting->setValue(((QCheckBox *)it->second)->isChecked(), true);
			}
			break;
		case LDExporterSetting::TLong:
		case LDExporterSetting::TFloat:
		case LDExporterSetting::TString:
			qstringtoucstring(value, ((QLineEdit *)it->second)->text());
			setting->setValue(value.c_str(), true);
			break;
		case LDExporterSetting::TEnum:
			setting->selectOption(((QComboBox *)it->second)->currentIndex(),
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
	resetSettings(m_settings);
}

void LDViewExportOption::resetSettings(SettingsMap &settings)
{
	SettingsMap::const_iterator it;
	QString value;

	for (it = settings.begin(); it != settings.end(); it++)
	{
		LDExporterSetting *setting = it->first;

		setting->reset();
		switch (setting->getType())
		{
		case LDExporterSetting::TBool:
			if (strcmp(it->second->metaObject()->className(), "QGroupBox") == 0)
			{
				((QGroupBox *)it->second)->setChecked(setting->getBoolValue());
			}
			else
			{
				((QCheckBox *)it->second)->setChecked(setting->getBoolValue());
			}
			break;
		case LDExporterSetting::TLong:
		case LDExporterSetting::TFloat:
		case LDExporterSetting::TString:
			ucstringtoqstring(value, setting->getStringValue());
			((QLineEdit *)it->second)->setText(value);
			break;
		case LDExporterSetting::TEnum:
			((QComboBox *)it->second)->setCurrentIndex(
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
//	m_origType = m_modelViewer->getExportType();
	m_exporter = m_modelViewer->getExporter((LDrawModelViewer::ExportType)m_origType);
//	populateTypeBox();
	populate();
	return QDialog::exec();
}

void LDViewExportOption::doResetGroup()
{
	QPushButton *pb = qobject_cast<QPushButton *>(sender());
	resetSettings(m_groups[pb->parentWidget()->layout()]);
}

void LDViewExportOption::doBrowse()
{
	QPushButton *pb = qobject_cast<QPushButton *>(sender());
	QString dir;
	dir = QFileDialog::getExistingDirectory(this,"",m_button[pb]->text());
	if (!dir.isEmpty())
	{
			m_button[pb]->setText (dir);
	}
}

