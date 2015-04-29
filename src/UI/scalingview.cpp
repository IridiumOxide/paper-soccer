#include "scalingview.h"

ScalingView::ScalingView(QWidget *parent) :
	QGraphicsView(parent)
{
}

void ScalingView::resizeEvent(QResizeEvent *event){
	fitInView(QGraphicsView::sceneRect(), Qt::KeepAspectRatio);
	QGraphicsView::resizeEvent(event);
}
