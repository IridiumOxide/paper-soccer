#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <QVector>
#include <QtWidgets>

enum direction{
	south = 0,
	southeast = 1,
	east = 2,
	northeast = 3,
	north = 4,
	northwest = 5,
	west = 6,
	southwest = 7,
	goal = 8
};

struct segmentInfo{
	bool valid;
	qint32 lineDir;
};

struct moveInfo{
	bool valid;
	QVector<qint32> directions;
};

class GameState : public QObject
{
	Q_OBJECT

public:
	GameState(QWidget *parent = nullptr, qint32 boardLength = 0, qint32 boardWidth = 0, bool p1AI = false, bool p2AI = false);

	bool getBoardState(qint32 x, qint32 y, qint32 dir) const;
	bool getBoardStateRaw(qint32 x, qint32 y, qint32 dir) const;
	void setBoardState(qint32 x, qint32 y, qint32 dir, bool b);
	qint32 getEdgesDrawn(qint32 x, qint32 y) const;
	qint32 getEdgesDrawnRaw(qint32 x, qint32 y) const;
	void setEdgesDrawn(qint32 x, qint32 y, qint32 value);
	void incrementEdgesDrawn(qint32 x, qint32 y);
	void decrementEdgesDrawn(qint32 x, qint32 y);
	void addLine(qint32 x, qint32 y, qint32 x2, qint32 y2, qint32 dir);
	void removeLine(qint32 x, qint32 y, qint32 x2, qint32 y2, qint32 dir);
	void abandonHistory();
	void placeBall(qint32 x, qint32 y);
	void changePlayer();
	void setPlayer1AI(bool b);
	void setPlayer2AI(bool b);

	bool isP1ControlledByAI() const;
	bool isP2ControlledByAI() const;
	bool doesPlayer1MoveNow() const;
	bool isGameFinished() const;
	bool didPlayer1Win() const;
	bool isMoveFinished() const;
	bool stuck() const;
	qint32 getHalfBoardLength() const;
	qint32 getHalfBoardWidth() const;
	qint32 getBallX() const;
	qint32 getBallY() const;
	// number - łączna liczba,
	// count - obecny numer
	qint32 getTurnNumber() const;
	qint32 getTurnCount() const;
	qint32 getSegmentNumber() const;
	qint32 getSegmentCount() const;
	qint32 reverseDir(qint32 dir) const;
	qint32 xofDir(qint32 dir) const;
	qint32 yofDir(qint32 dir) const;
	QVector<qint32> getMove() const;

	const static qint32 NUMBER_OF_DIRECTIONS = 4;
	const static qint32 NUMBER_OF_ALL_DIRECTIONS = 8;
	const static qint32 MAX_LENGTH = 30;
	const static qint32 MAX_WIDTH = 30;

public slots:

	void saveGame() const;
	void loadGame(QDataStream *in);

	moveInfo previousMove();
	segmentInfo previousSegment();
	moveInfo nextMove();
	segmentInfo nextSegment();
	void deleteLastSegment();
	bool confirmMove();
	void setMoveFinished(bool b);
	bool finished();
	void unWinGame();

	segmentInfo attemptGoal(qint32 x, bool north);
	segmentInfo attemptMove(qint32 x, qint32 y);

private:
	qint32 boardLength;
	qint32 boardWidth;

	bool moveFinished;
	bool player1MovesNow;
	bool gameFinished;
	bool player1Won;

	bool p1ControlledByAI;
	bool p2ControlledByAI;


	// Tablica boardState przechowuje informacje o narysowanych liniach.
	// Punkt (0, 0) znajduje się w [boardLength / 2][boardWidth / 2].
	// Bandy znajdują się w [0][x], [x][0], [boardLength][x], [x][boardWidth].
	// Tablica nie uwzględnia stanu bramek.
	// Kierunki - południowy zachód, południe, południowy wschód, wschód.
	// ( północ pola [i][j] <- południe pola [i-1][j]  itp )
	bool boardState[MAX_LENGTH + 1][MAX_WIDTH + 1][NUMBER_OF_DIRECTIONS];

	// Tablica edgesDrawn przechowuje informacje o ilości linii wychodzących
	//		z każego punktu planszy. Jeśli punkt leży na bandzie, odpowiednio
	//		zwiększamy tę wartość (róg ma początkowo 7, banda 5)
	// - Jeśli po ruchu jest równa 8, kończymy grę
	// - Jeśli po ruchu jest większa niż 1, odbijamy się
	qint32 edgesDrawn[MAX_LENGTH + 1][MAX_WIDTH + 1];

	qint32 turnNumber;
	qint32 turnCount;
	qint32 segmentNumber;
	qint32 segmentCount;
	qint32 ballX;
	qint32 ballY;

	QMessageBox datalossWarningBox;
	// przechowujemy ruchy jako kierunki.
	// 0 - południe, kolejnie przeciwnie do wskazówek zegara
	QVector< QVector< qint32 > > moves;
};

#endif // GAMESTATE_H
