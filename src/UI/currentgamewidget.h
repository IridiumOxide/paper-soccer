#ifndef CURRENTGAMEWIDGET_H
#define CURRENTGAMEWIDGET_H

#include <QtWidgets>
#include "gamemenuwidget.h"
#include "editormenuwidget.h"
#include "endgamewidget.h"
#include "calculatingmovewidget.h"
#include "scalingview.h"
#include "../GameState/fieldpoint.h"
#include "../AI/ai.h"

class CurrentGameWidget : public QWidget
{
	Q_OBJECT

public:
	CurrentGameWidget(QWidget *parent = nullptr);
	void setGameState(GameState *g);

	bool canSave();
	bool canEdit();

public slots:
	void recolorMove(QVector<qint32> directions, QPen pen);
	void nextMove();
	void previousMove();
	void nextSegment();
	void previousSegment();
	void confirmMove();
	void getHint();
	void revert();
	void resumeAI();
	void startEdit();
	void finishEdit();
	void placeBall(qint32 x, qint32 y);

private:
	const static qint32 SPACE = 30;
	const static qint32 CIRCLE_SIZE = 10;
	const static qint32 PEN_WIDTH = 3;

	bool placeBallMode;
	bool editLineMode;

	FieldPoint *editLinePoint;
	qint32 editLineX;
	qint32 editLineY;

	FieldPoint *ballPoint;
	ScalingView *boardView;
	QGraphicsScene *boardScene;
	QGraphicsLineItem* lines[GameState::MAX_LENGTH + 1][GameState::MAX_WIDTH + 1][GameState::NUMBER_OF_DIRECTIONS];
	FieldPoint* points[GameState::MAX_LENGTH + 1][GameState::MAX_WIDTH + 1];
	QGraphicsLineItem* goals[2][5];
	FieldPoint* goalPoints[2][3];
	QGraphicsLineItem *winLine;
	FieldPoint* goalPoint;

	QGraphicsLineItem*& getLine(qint32 x, qint32 y, qint32 dir, bool wasGoal);
	FieldPoint*& getPoint(qint32 x, qint32 y);
	FieldPoint*& getGoalPoint(qint32 x, bool isNorth);
	QPen blackLine;
	QPen blueLine;

	QMessageBox cantSaveMsg;
	QMessageBox cantEditMsg;
	QMessageBox AIMsgBox;


	GameState *gameState;
	QHBoxLayout *currentGameLayout;
	GameMenuWidget *gameMenuWidget;
	EditorMenuWidget *editorMenuWidget;
	CalculatingMoveWidget *calculatingMoveWidget;
	EndGameWidget *endGameWidget;
	QStackedWidget *stackedWidget;

	AI *ai;

	void drawBoard();
	qreal circleCenter(qreal x);
	void goalAttempt(int x, bool isNorth);
	void moveAttempt(int x, int y);
	bool isAdjacent(qint32 x, qint32 y, qint32 x2, qint32 y2) const;
	void editLine(qint32 x, qint32 y, qint32 x2, qint32 y2);
};

#endif // CURRENTGAMEWIDGET_H
