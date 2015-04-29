#include "fieldpoint.h"

FieldPoint::FieldPoint(qreal x, qreal y, qreal width, qreal height, QWidget *parent) :
	QObject(parent),
	QGraphicsEllipseItem(x, y, width, height, nullptr)
{
}

void FieldPoint::mousePressEvent(QGraphicsSceneMouseEvent *event){
	event->accept();
	emit clicked();
}
