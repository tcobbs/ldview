#ifndef __TCCOLORBUTTON_H__
#define __TCCOLORBUTTON_H__

#include <qpushbutton.h>

class TCColorButton : public QPushButton
{
	Q_OBJECT
	Q_PROPERTY( QColor color READ color WRITE setColor )

public:
	TCColorButton(QWidget *parent, const char *name = NULL);
	TCColorButton(const QColor &c, QWidget *parent, const char *name = NULL);
	~TCColorButton(void);

	QColor color(void) const { return m_color; }
	void setColor(const QColor &c);

signals:
	void changed(const QColor &newColor);

protected:
	virtual void drawButtonLabel(QPainter *painter);
	QColor m_color;

protected slots:
	void onClick(void);
};

#endif // __TCCOLORBUTTON_H__
