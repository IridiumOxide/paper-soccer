#ifndef CALCULATINGMOVEWIDGET_H
#define CALCULATINGMOVEWIDGET_H

#include <QtWidgets>

class CalculatingMoveWidget : public QWidget
{
	Q_OBJECT
public:
	explicit CalculatingMoveWidget(QWidget *parent = 0);

signals:
	void stopAI();
public slots:

private:
	QPushButton *stopAIButton;
	QLabel *infoLabel;
	QVBoxLayout *calculatingMoveWidgetLayout;
};

#endif // CALCULATINGMOVEWIDGET_H
