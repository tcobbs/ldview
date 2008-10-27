#include "qt4wrapper.h"
#include "Preferences.h"

#include <qstring.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qgroupbox.h>
#include <qpushbutton.h>
#include "misc.h"
#include "ModelViewerWidget.h"
#include <LDLib/LDUserDefaultsKeys.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qcombobox.h>

LDViewExportOption::LDViewExportOption(LDExporter *exporter)
	:ExportOptionPanel(),
	m_exporter(exporter)
{
	QWidget *parent;
	sv = new QScrollView(centralWidget(),"scrollview");
	//sv->setHScrollBarMode(QScrollView::AlwaysOff);
	sv->setVScrollBarMode(QScrollView::AlwaysOn);
#if (QT_VERSION >>16)==3
	layouttop->addWidget(sv);
#endif
	box = new QVBox(sv->viewport());
	box->setMargin(11);
	box->setSpacing(4);
	sv->addChild(box);
	parent = box;
    LDExporterSettingList &settings = m_exporter->getSettings();
    LDExporterSettingList::iterator it;

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
				if (vbl) vbl->addWidget(check);
            }
            else
            {
                // Top level group; use a group setting.
				QString qstmp;
				ucstringtoqstring(qstmp,it->getName());
				QGroupBox *gb;
				gb = new QGroupBox (qstmp, box,qstmp);
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
                break;
            case LDExporterSetting::TFloat:
				label = new QLabel(qstmp,hbox);
				li = new QLineEdit(qstmp,hbox);
				ucstringtoqstring(qstmp,it->getStringValue());
				li->setText(qstmp);
                break;
            case LDExporterSetting::TLong:
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
                break;
            default:
                throw "not implemented";
            }
			if (vbl) vbl->addWidget(hbox);
        }
	}
	adjustSize();
	sv->adjustSize();
	sv->viewport()->adjustSize();
	resize(width() + box->width() - sv->visibleWidth(), height());
	setFixedWidth(width());
}

LDViewExportOption::~LDViewExportOption() { }


