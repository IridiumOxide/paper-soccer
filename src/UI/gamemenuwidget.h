#ifndef GAMEMENUWIDGET_H
#define GAMEMENUWIDGET_H

#include <QtWidgets>
#include "../GameState/gamestate.h"

class GameMenuWidget : public QWidget
{
	Q_OBJECT

public:
	GameMenuWidget(QWidget *parent = nullptr);
	bool autoconfirmAIToggled() const;
	void setGameState(GameState *g);

signals:
	void previousMove();
	void previousSegment();
	void nextMove();
	void nextSegment();
	void confirmMove();
	void getHint();
	void resumeAI();

public slots:
	void nextMoveSetEnabled(bool b);
	void nextSegmentSetEnabled(bool b);
	void previousMoveSetEnabled(bool b);
	void previousSegmentSetEnabled(bool b);
	void confirmMoveSetEnabled(bool b);
	void resumeAISetEnabled(bool b);
	void hintSetEnabled(bool b);
	void disableAutoconfirmBox();
	void adjustPlayer();

private:
	void highlight(QLabel *l);
	void unhighlight(QLabel *l);

	GameState *gameState;

	QWidget *playerNamesWidget;
	QHBoxLayout *playerNamesLayout;
	QLabel *player1Label;
	QLabel *player2Label;

	QPushButton *infoButton;
	QLabel *infoLabel;

	QWidget *historyButtonsWidget;
	QGridLayout *historyButtonsLayout;
	QPushButton *nextMoveButton;
	QPushButton *previousMoveButton;
	QPushButton *nextSegmentButton;
	QPushButton *previousSegmentButton;

	QPushButton *confirmMoveButton;
	QPushButton *hintButton;

	QWidget *bottomWidget;
	QHBoxLayout *bottomLayout;
	QCheckBox *autoconfirmAI;
	QPushButton *resumeAIButton;

	QVBoxLayout *gameMenuLayout;
};

#endif // GAMEMENUWIDGET_H
