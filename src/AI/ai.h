#ifndef AI_H
#define AI_H

#include <QtWidgets>
#include "../GameState/gamestate.h"


class AI : public QObject
{
	Q_OBJECT
public:
	AI(QObject *parent = nullptr);
	QVector<qint32> AIGetMove(GameState *g);
	bool getBoardState(qint32 x, qint32 y, qint32 dir) const;
	void setBoardState(qint32 x, qint32 y, qint32 dir, bool b);
	qint32 getEdgesDrawn(qint32 x, qint32 y) const;
	void incrementEdgesDrawn(qint32 x, qint32 y);
	bool canGo(qint32 x, qint32 y, qint32 dir) const;
	void go(qint32 dir);

signals:

public slots:
	void stopAI();

private:
	bool halt;
	qint32 ballX;
	qint32 ballY;
	GameState *gameState;
	bool boardState[GameState::MAX_LENGTH + 1][GameState::MAX_WIDTH + 1][GameState::NUMBER_OF_DIRECTIONS];
	qint32 edgesDrawn[GameState::MAX_LENGTH + 1][GameState::MAX_WIDTH + 1];

};

#endif // AI_H
