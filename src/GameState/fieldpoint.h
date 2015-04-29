#ifndef FIELDPOINT_H
#define FIELDPOINT_H

#include <QtWidgets>

class FieldPoint : public QObject, public QGraphicsEllipseItem
{
	Q_OBJECT

public:
	FieldPoint(qreal x, qreal y, qreal width, qreal height, QWidget *parent = nullptr);

signals:
	void clicked();

private:
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
};

#endif // FIELDPOINT_H
