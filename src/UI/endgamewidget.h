#ifndef ENDGAMEWIDGET_H
#define ENDGAMEWIDGET_H

#include <QtWidgets>

class EndGameWidget : public QWidget
{
	Q_OBJECT
public:
	EndGameWidget(QWidget *parent = nullptr);
	bool isStuck;

signals:
	void revert();
	void backEdit();

public slots:
	void display(bool player1);
	void stuck(bool player1);

private:
	QLabel *winInfoLabel;
	QVBoxLayout *endGameLayout;
	QPushButton *revertButton;
	QPushButton *editButton;

};

#endif // ENDGAMEWIDGET_H
