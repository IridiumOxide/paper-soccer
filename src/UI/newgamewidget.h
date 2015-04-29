#ifndef NEWGAMEWIDGET_H
#define NEWGAMEWIDGET_H

#include <QtWidgets>

class NewGameWidget : public QWidget
{
	Q_OBJECT

public:
	NewGameWidget(QWidget *parent = nullptr);

signals:
	void newGameCreated(qint32 boardLength, qint32 boardWidth, bool isP1AI, bool isP2AI);

public slots:
	void emitNewGame();
	void changeLength();
	void changeWidth();

private:
	const static qint32 MIN_LENGTH = 4;
	const static qint32 MAX_LENGTH = 30;
	QMessageBox oddLengthMsgBox;
	qint32 boardLength;

	const static qint32 MIN_WIDTH = 2;
	const static qint32 MAX_WIDTH = 30;
	QMessageBox oddWidthMsgBox;
	qint32 boardWidth;

	QWidget *lengthChoiceWidget;
	QHBoxLayout *lengthChoiceLayout;
	QLabel *lengthLabel;
	QPushButton *lengthChangeButton;

	QWidget *widthChoiceWidget;
	QHBoxLayout *widthChoiceLayout;
	QLabel *widthLabel;
	QPushButton *widthChangeButton;

	QWidget *computerPlayerChoiceWidget;
	QVBoxLayout *computerPlayerChoiceLayout;
	QCheckBox *p1ControlledByAI;
	QCheckBox *p2ControlledByAI;

	QWidget *playButtonWidget;
	QHBoxLayout *playButtonLayout;
	QPushButton *playButton;

	QVBoxLayout *newGameLayout;
};

#endif // NEWGAMEWIDGET_H
