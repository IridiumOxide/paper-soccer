#ifndef SCALINGVIEW_H
#define SCALINGVIEW_H

#include <QGraphicsView>

class ScalingView : public QGraphicsView
{
	Q_OBJECT
public:
	ScalingView(QWidget *parent = 0);

private:
	void resizeEvent(QResizeEvent *event);

};

#endif // SCALINGVIEW_H
