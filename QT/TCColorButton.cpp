#include "TCColorButton.h"
#include <qapplication.h>
#include <qstyle.h>
#include <qframe.h>
#include <qcolor.h>
#include <qpainter.h>
#include <qcolordialog.h>

TCColorButton::TCColorButton(QWidget *parent, const char *name)
	:QPushButton(parent, name),
	m_color(QColor(0, 0, 0))
{
	connect(this, SIGNAL(clicked()), this, SLOT(onClick()) );
}

TCColorButton::~TCColorButton(void)
{
}

void TCColorButton::setColor(const QColor &c)
{
	m_color = c;
	repaint();
	changed(c);
}

void TCColorButton::drawButtonLabel(QPainter *painter)
{
	QStyle &style = QApplication::style();
	QRect rect = style.subRect(QStyle::SR_PushButtonContents, this);
	QRect focusRect = style.subRect(QStyle::SR_PushButtonFocusRect, this);
	int margin = 4;

	QPushButton::drawButtonLabel(painter);
	if (focusRect.width() < rect.width() && focusRect.height() < rect.height())
	{
		rect = focusRect;
		margin = 1;
	}
	rect.addCoords(margin, margin, -margin, -margin);
	painter->fillRect(rect, m_color);
	style.drawPrimitive(QStyle::PE_GroupBoxFrame, painter, rect, colorGroup(),
		QStyle::Style_Sunken, QStyleOption(1, 1, QFrame::Panel,
		QFrame::Sunken));
}

void TCColorButton::onClick(void)
{
	setColor(QColorDialog::getColor());
}

