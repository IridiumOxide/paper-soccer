#include "currentgamewidget.h"

CurrentGameWidget::CurrentGameWidget(QWidget *parent) :
	QWidget(parent),
	placeBallMode(false),
	editLineMode(false),
	editLinePoint(nullptr),
	blackLine(Qt::black),
	blueLine(Qt::blue)
{
	currentGameLayout = new QHBoxLayout(this);

	boardScene = new QGraphicsScene(this);
	boardView = new ScalingView(this);
	boardView->setScene(boardScene);

	currentGameLayout->addWidget(boardView);

	gameMenuWidget = new GameMenuWidget(this);
	connect(gameMenuWidget, SIGNAL(nextMove()), this, SLOT(nextMove()));
	connect(gameMenuWidget, SIGNAL(nextSegment()), this, SLOT(nextSegment()));
	connect(gameMenuWidget, SIGNAL(previousMove()), this, SLOT(previousMove()));
	connect(gameMenuWidget, SIGNAL(previousSegment()), this, SLOT(previousSegment()));
	connect(gameMenuWidget, SIGNAL(confirmMove()), this, SLOT(confirmMove()));
	connect(gameMenuWidget, SIGNAL(getHint()), this, SLOT(getHint()));
	connect(gameMenuWidget, SIGNAL(resumeAI()), this, SLOT(resumeAI()));

	endGameWidget = new EndGameWidget(this);
	connect(endGameWidget, SIGNAL(revert()), this, SLOT(revert()));
	connect(endGameWidget, SIGNAL(backEdit()), this, SLOT(startEdit()));

	editorMenuWidget = new EditorMenuWidget(this);
	connect(editorMenuWidget, SIGNAL(finishEdit()), this, SLOT(finishEdit()));
	connect(editorMenuWidget, &EditorMenuWidget::placeBallMode, [this](){
		if(editLineMode){
			if(editLinePoint != nullptr){
				editLinePoint->setBrush(Qt::black);
				editLinePoint = nullptr;
			}
			editLineMode = false;
		}
		ballPoint->setBrush(Qt::red);
		placeBallMode = true;
	});
	connect(editorMenuWidget, &EditorMenuWidget::editLineMode, [this](){
		if(placeBallMode){
			ballPoint->setBrush(Qt::black);
			placeBallMode = false;
		}
		editLineMode = true;
	});

	ai = new AI(this);

	calculatingMoveWidget = new CalculatingMoveWidget(this);
	connect(calculatingMoveWidget, SIGNAL(stopAI()), ai, SLOT(stopAI()));

	stackedWidget = new QStackedWidget(this);
	stackedWidget->addWidget(gameMenuWidget);
	stackedWidget->addWidget(endGameWidget);
	stackedWidget->addWidget(editorMenuWidget);
	stackedWidget->addWidget(calculatingMoveWidget);

	stackedWidget->setCurrentWidget(gameMenuWidget);

	currentGameLayout->addWidget(stackedWidget);

	setLayout(currentGameLayout);

	blackLine.setWidth(PEN_WIDTH);
	blueLine.setWidth(PEN_WIDTH);

	cantSaveMsg.setWindowTitle("Can't save");
	cantEditMsg.setWindowTitle("Can't edit");
	AIMsgBox.setWindowTitle("No move for AI");
	AIMsgBox.setText("Move has finished, AI cannot be resumed now.");
}

QGraphicsLineItem*& CurrentGameWidget::getLine(qint32 x, qint32 y, qint32 dir, bool wasGoal){
	if(y == -gameState->getHalfBoardLength() && wasGoal){
		if(x == -1 && dir == northeast){
			return goals[0][1];
		}else if(x == 0){
			if(dir == northwest){
				return goals[0][0];
			}else if(dir == north){
				return goals[0][2];
			}else if(dir == northeast){
				return goals[0][4];
			}
		}else if(x == 1 && dir == northwest){
			return goals[0][3];
		}
	}

	if(y == gameState->getHalfBoardLength() && wasGoal){
		if(x == -1 && dir == southeast){
			return goals[1][1];
		}else if(x == 0){
			if(dir == southwest){
				return goals[1][0];
			}else if(dir == south){
				return goals[1][2];
			}else if(dir == southeast){
				return goals[1][4];
			}
		}else if(x == 1 && dir == southwest){
			return goals[1][3];
		}
	}

	if(dir >= 4){
		switch(dir){
		case north: y--; break;
		case northwest: x--; y--; break;
		case west: x--; break;
		case southwest: x--; y++; break;
		}
		dir -= 4;
	}
	return lines[x + gameState->getHalfBoardWidth()][y + gameState->getHalfBoardLength()][dir];
}

FieldPoint*& CurrentGameWidget::getPoint(qint32 x, qint32 y){
	return points[x + gameState->getHalfBoardWidth()][y + gameState->getHalfBoardLength()];
}

FieldPoint*& CurrentGameWidget::getGoalPoint(qint32 x, bool isNorth){
	return goalPoints[isNorth?0:1][x + 1];
}

qreal CurrentGameWidget::circleCenter(qreal x){
	return (x * SPACE) + (CIRCLE_SIZE / 2);
}

void CurrentGameWidget::setGameState(GameState *g){
	gameState = g;
	gameMenuWidget->setGameState(gameState);
	editorMenuWidget->setGameState(gameState);
	stackedWidget->setCurrentWidget(gameMenuWidget);
	drawBoard();
	placeBallMode = false;
	editLineMode = false;
}

void CurrentGameWidget::drawBoard(){
	boardScene->clear();
	delete boardScene;
	boardScene = new QGraphicsScene(this);
	boardView->setScene(boardScene);

	// Bramki zawsze wyglądają tak samo, więc ich linie można wygenerować osobno

	qint32 yPos = -(gameState->getHalfBoardLength() + 1);

	// kreski do strzelania goli
	goals[0][0] = new QGraphicsLineItem(circleCenter(-1), circleCenter(yPos), circleCenter(0), circleCenter(yPos + 1), nullptr);
	goals[0][1] = new QGraphicsLineItem(circleCenter(0), circleCenter(yPos), circleCenter(-1), circleCenter(yPos + 1), nullptr);
	goals[0][2] = new QGraphicsLineItem(circleCenter(0), circleCenter(yPos), circleCenter(0), circleCenter(yPos + 1), nullptr);
	goals[0][3] = new QGraphicsLineItem(circleCenter(0), circleCenter(yPos), circleCenter(1), circleCenter(yPos + 1), nullptr);
	goals[0][4] = new QGraphicsLineItem(circleCenter(1), circleCenter(yPos), circleCenter(0), circleCenter(yPos + 1), nullptr);
	for(int i = 0; i < 5; i++){
		goals[0][i]->setPen(blueLine);
		boardScene->addItem(goals[0][i]);
		goals[0][i]->hide();
	}

	for(qint32 i = -1; i <= 1; i++){
		if(i == -1 || i == 1){
			// Słupki
			QGraphicsLineItem *goalLine = new QGraphicsLineItem(circleCenter(i), circleCenter(yPos), circleCenter(i), circleCenter(yPos + 1), nullptr);
			goalLine->setPen(blackLine);
			boardScene->addItem(goalLine);
		}
		if(i < 1){
			// Tylne linie
			QGraphicsLineItem *goalLine = new QGraphicsLineItem(circleCenter(i), circleCenter(yPos), circleCenter(i + 1), circleCenter(yPos), nullptr);
			goalLine->setPen(blackLine);
			boardScene->addItem(goalLine);
		}
		goalPoints[0][i + 1] = new FieldPoint((i * SPACE), (yPos * SPACE), CIRCLE_SIZE, CIRCLE_SIZE, this);
		goalPoints[0][i + 1]->setBrush(Qt::white);
		boardScene->addItem(goalPoints[0][i + 1]);
		connect(goalPoints[0][i + 1], &FieldPoint::clicked, [i, this](){
			if(!((gameState->doesPlayer1MoveNow() && gameState->isP1ControlledByAI()) || (!(gameState->doesPlayer1MoveNow()) && gameState->isP2ControlledByAI()))){
				goalAttempt(i, true);
			}});
	}

	// druga bramka
	yPos = gameState->getHalfBoardLength() + 1;

	// kreski do strzelania goli
	goals[1][0] = new QGraphicsLineItem(circleCenter(-1), circleCenter(yPos), circleCenter(0), circleCenter(yPos - 1), nullptr);
	goals[1][1] = new QGraphicsLineItem(circleCenter(0), circleCenter(yPos), circleCenter(-1), circleCenter(yPos - 1), nullptr);
	goals[1][2] = new QGraphicsLineItem(circleCenter(0), circleCenter(yPos), circleCenter(0), circleCenter(yPos - 1), nullptr);
	goals[1][3] = new QGraphicsLineItem(circleCenter(0), circleCenter(yPos), circleCenter(1), circleCenter(yPos - 1), nullptr);
	goals[1][4] = new QGraphicsLineItem(circleCenter(1), circleCenter(yPos), circleCenter(0), circleCenter(yPos - 1), nullptr);
	for(int i = 0; i < 5; i++){
		goals[1][i]->setPen(blueLine);
		boardScene->addItem(goals[1][i]);
		goals[1][i]->hide();
	}

	for(qint32 i = -1; i <= 1; i++){
		if(i == -1 || i == 1){
			// Słupki
			QGraphicsLineItem *goalLine = new QGraphicsLineItem(circleCenter(i), circleCenter(yPos), circleCenter(i), circleCenter(yPos - 1), nullptr);
			goalLine->setPen(blackLine);
			boardScene->addItem(goalLine);
		}
		if(i < 1){
			// Tylne linie
			QGraphicsLineItem *goalLine = new QGraphicsLineItem(circleCenter(i), circleCenter(yPos), circleCenter(i + 1), circleCenter(yPos), nullptr);
			goalLine->setPen(blackLine);
			boardScene->addItem(goalLine);
		}
		goalPoints[1][i + 1] = new FieldPoint((i * SPACE), yPos * SPACE, CIRCLE_SIZE, CIRCLE_SIZE, this);
		goalPoints[1][i + 1]->setBrush(Qt::white);
		boardScene->addItem(goalPoints[1][i + 1]);
		connect(goalPoints[1][i + 1], &FieldPoint::clicked, [i, this](){ goalAttempt(i, false); });
	}

	for(qint32 i = -(gameState->getHalfBoardWidth()); i <= gameState->getHalfBoardWidth(); i++){
		for(qint32 j = -(gameState->getHalfBoardLength()); j <= gameState->getHalfBoardLength(); j++){

			getPoint(i, j) = new FieldPoint((i * SPACE), (j * SPACE), CIRCLE_SIZE, CIRCLE_SIZE, this);
			getPoint(i, j)->setPen(QColor(Qt::black));
			if(i == gameState->getBallX() && j == gameState->getBallY()){
				ballPoint = getPoint(i, j);
				getPoint(i, j)->setBrush(QBrush(QColor(Qt::red)));
			}else{
				getPoint(i, j)->setBrush(QBrush(QColor(Qt::black)));
			}

			getLine(i, j, south, false) = new QGraphicsLineItem(circleCenter(i), circleCenter(j), circleCenter(i), circleCenter(j + 1), nullptr);
			if(i != gameState->getHalfBoardWidth()){
				// nie dodajemy linii na wschód od boiska, bo źle się rysuje
				getLine(i, j, southeast, false) = new QGraphicsLineItem(circleCenter(i), circleCenter(j), circleCenter(i + 1), circleCenter(j + 1), nullptr);
				getLine(i, j, east, false) = new QGraphicsLineItem(circleCenter(i), circleCenter(j), circleCenter(i + 1),  circleCenter(j), nullptr);
				getLine(i, j, northeast, false) = new QGraphicsLineItem(circleCenter(i), circleCenter(j), circleCenter(i + 1),  circleCenter(j - 1), nullptr);
			}

			for(qint32 k = south; k < GameState::NUMBER_OF_DIRECTIONS; k++){
				if(i != gameState->getHalfBoardWidth() || k == south){
					// nie dodajemy linii na wschód od boiska, bo się za duże rysuje
					getLine(i, j, k, false)->setPen(blackLine);
					boardScene->addItem(getLine(i, j, k, false));
					if(gameState->getBoardState(i, j, k) != true){
						getLine(i, j, k, false)->hide();
					}
				}
			}

			boardScene->addItem(getPoint(i, j));
			connect(getPoint(i, j), &FieldPoint::clicked, [i, j, this](){
				if(!((gameState->doesPlayer1MoveNow() && gameState->isP1ControlledByAI()) || (!(gameState->doesPlayer1MoveNow()) && gameState->isP2ControlledByAI()))){
					moveAttempt(i, j);
				}});
		}
	}

	QVector<qint32> partialMove;
	QVector<qint32> fullMove = gameState->getMove();
	for(int i = 0; i < gameState->getSegmentCount(); i++){
		partialMove.push_back(fullMove[i]);
	}
	recolorMove(partialMove, blueLine);

	boardView->fitInView(boardView->sceneRect(), Qt::KeepAspectRatio);
}

void CurrentGameWidget::goalAttempt(int x, bool isNorth){

	// voodoo - nie usuwać
	getGoalPoint(x, isNorth)->setEnabled(false);
	getGoalPoint(x, isNorth)->setEnabled(true);
	// powinno działać dobrze, ale nie musi

	segmentInfo attemptedGoal = gameState->attemptGoal(x, isNorth);
	if(attemptedGoal.valid){
		winLine = getLine(gameState->getBallX(), gameState->getBallY(), attemptedGoal.lineDir, true);
		winLine->show();
		ballPoint->setBrush(Qt::black);
		goalPoint = getGoalPoint(x, isNorth);
		goalPoint->setBrush(Qt::green);
		endGameWidget->display(gameState->didPlayer1Win());
		stackedWidget->setCurrentWidget(endGameWidget);
	}
}

void CurrentGameWidget::moveAttempt(int x, int y){

	// voodoo - nie usuwać
	getPoint(x, y)->setEnabled(false);
	getPoint(x, y)->setEnabled(true);
	// bez tego nie działa

	if(placeBallMode){
		placeBall(x, y);
		return;
	}

	if(editLineMode){
		if(editLinePoint == nullptr){
			editLinePoint = getPoint(x, y);
			editLinePoint->setBrush(Qt::yellow);
			editLineX = x;
			editLineY = y;
		}else if(editLineX == x && editLineY == y){
			editLinePoint->setBrush(Qt::black);
			editLinePoint = nullptr;
		}else if(isAdjacent(editLineX, editLineY, x, y)){
			editLine(editLineX, editLineY, x, y);
			editLinePoint->setBrush(Qt::black);
			editLinePoint = nullptr;
		}else{
			editLinePoint->setBrush(Qt::black);
			editLinePoint = getPoint(x, y);
			editLinePoint->setBrush(Qt::yellow);
			editLineX = x;
			editLineY = y;
		}
		return;
	}

	segmentInfo attemptedMove = gameState->attemptMove(x, y);
	if(attemptedMove.valid){
		getLine(x, y, gameState->reverseDir(attemptedMove.lineDir), false)->show();
		getLine(x, y, gameState->reverseDir(attemptedMove.lineDir), false)->setPen(blueLine);
		ballPoint->setBrush(Qt::black);
		ballPoint = getPoint(x, y);
		getPoint(x, y)->setBrush(Qt::red);

		gameMenuWidget->nextSegmentSetEnabled(false);
		gameMenuWidget->previousSegmentSetEnabled(true);
	}
	if(gameState->isMoveFinished()){
		gameMenuWidget->confirmMoveSetEnabled(true);
	}
	if(gameState->isGameFinished()){
		endGameWidget->display(gameState->didPlayer1Win());
		stackedWidget->setCurrentWidget(endGameWidget);
	}
}

void CurrentGameWidget::recolorMove(QVector<qint32> directions, QPen pen){
	// kolorujemy od piłki do początku ruchu
	qint32 currentX = gameState->getBallX();
	qint32 currentY = gameState->getBallY();
	for(qint32 i = directions.size() - 1; i >= 0; i--){
		getLine(currentX, currentY, gameState->reverseDir(directions[i]), false)->setPen(pen);
		currentX += gameState->xofDir(gameState->reverseDir(directions[i]));
		currentY += gameState->yofDir(gameState->reverseDir(directions[i]));
	}
}

void CurrentGameWidget::nextMove(){
	moveInfo move;
	while(gameState->getSegmentCount() != gameState->getSegmentNumber()){
		nextSegment();
	}

	move = gameState->nextMove();
	gameMenuWidget->adjustPlayer();
	recolorMove(move.directions, blackLine);
	gameMenuWidget->confirmMoveSetEnabled(false);

	while(gameState->getSegmentCount() != gameState->getSegmentNumber()){
		nextSegment();
	}

	gameMenuWidget->previousMoveSetEnabled(true);
	if(gameState->getTurnCount() == gameState->getTurnNumber()){
		gameMenuWidget->nextMoveSetEnabled(false);
	}
}

void CurrentGameWidget::previousMove(){
	moveInfo move;
	while(gameState->getSegmentCount() > 0){
		previousSegment();
	}
	move = gameState->previousMove();
	if(move.valid){
		recolorMove(move.directions, blueLine);
		gameMenuWidget->adjustPlayer();

		gameMenuWidget->previousSegmentSetEnabled(true);
		gameMenuWidget->nextSegmentSetEnabled(false);
		gameMenuWidget->nextMoveSetEnabled(true);
		gameMenuWidget->confirmMoveSetEnabled(true);
		if(gameState->getTurnCount() == 0){
			gameMenuWidget->previousMoveSetEnabled(false);
		}
	}
}

void CurrentGameWidget::nextSegment(){
	qint32 oldX = gameState->getBallX();
	qint32 oldY = gameState->getBallY();
	segmentInfo segment = gameState->nextSegment();
	if(segment.valid){
		getLine(oldX, oldY, segment.lineDir, false)->show();
		getLine(oldX, oldY, segment.lineDir, false)->setPen(blueLine);
		ballPoint->setBrush(Qt::black);
		ballPoint = getPoint(gameState->getBallX(), gameState->getBallY());
		ballPoint->setBrush(Qt::red);

		if(gameState->getSegmentCount() == gameState->getSegmentNumber()){
			gameMenuWidget->nextSegmentSetEnabled(false);
		}
		gameMenuWidget->previousSegmentSetEnabled(true);

		if(gameState->isMoveFinished()){
			gameMenuWidget->confirmMoveSetEnabled(true);
		}
	}
}

void CurrentGameWidget::previousSegment(){
	qint32 oldX = gameState->getBallX();
	qint32 oldY = gameState->getBallY();
	segmentInfo segment = gameState->previousSegment();
	if(segment.valid){
		getLine(oldX, oldY, gameState->reverseDir(segment.lineDir), false)->hide();
		ballPoint->setBrush(Qt::black);
		ballPoint = getPoint(gameState->getBallX(), gameState->getBallY());
		ballPoint->setBrush(Qt::red);

		if(gameState->getSegmentCount() == 0){
			gameMenuWidget->previousSegmentSetEnabled(false);
		}
		gameMenuWidget->nextSegmentSetEnabled(true);
		gameMenuWidget->confirmMoveSetEnabled(false);
	}
}

void CurrentGameWidget::confirmMove(){
	QVector<qint32> move = gameState->getMove();
	if(gameState->confirmMove()){
		recolorMove(move, blackLine);
		gameMenuWidget->confirmMoveSetEnabled(false);
		gameMenuWidget->previousSegmentSetEnabled(false);
		gameMenuWidget->previousMoveSetEnabled(true);
		gameMenuWidget->nextSegmentSetEnabled(false);
		gameMenuWidget->nextMoveSetEnabled(false);
		gameMenuWidget->adjustPlayer();
		if((gameState->isP1ControlledByAI() && gameState->doesPlayer1MoveNow()) || (gameState->isP2ControlledByAI() && !(gameState->doesPlayer1MoveNow()))){
			resumeAI();
		}
	}
}

void CurrentGameWidget::getHint(){
	// AI wykonuje cały ruch ale go nie zatwierdza
	stackedWidget->setCurrentWidget(calculatingMoveWidget);
	QVector<qint32> move = ai->AIGetMove(gameState);
	if(move.isEmpty()){
		//ruch przerwany
		stackedWidget->setCurrentWidget(gameMenuWidget);
	}else{
		for(int i = 0; i < move.size(); i++){
			if(move[i] == goal){
				bool isNorth;
				if(gameState->getBallY() < 0){
					isNorth = true;
				}else{
					isNorth = false;
				}
				goalAttempt(0, isNorth);
			}else{
				moveAttempt(gameState->getBallX() + gameState->xofDir(move[i]), gameState->getBallY() + gameState->yofDir(move[i]));
			}
		}
		if(!gameState->isGameFinished()){
			stackedWidget->setCurrentWidget(gameMenuWidget);
		}
	}
}

void CurrentGameWidget::resumeAI(){
	// AI rusza od piłki
	if(gameState->isMoveFinished()){
		AIMsgBox.exec();
		return;
	}
	stackedWidget->setCurrentWidget(calculatingMoveWidget);
	QVector<qint32> move = ai->AIGetMove(gameState);
	if(move.isEmpty()){
		//ruch przerwany
		stackedWidget->setCurrentWidget(gameMenuWidget);
	}else{
		for(int i = 0; i < move.size(); i++){
			if(move[i] == goal){
				bool isNorth;
				if(gameState->getBallY() < 0){
					isNorth = true;
				}else{
					isNorth = false;
				}
				goalAttempt(0, isNorth);
			}else{
				moveAttempt(gameState->getBallX() + gameState->xofDir(move[i]), gameState->getBallY() + gameState->yofDir(move[i]));
			}
		}
		if(!(gameState->isGameFinished())){
			stackedWidget->setCurrentWidget(gameMenuWidget);
			if(gameMenuWidget->autoconfirmAIToggled()){
				confirmMove();
			}
		}
	}
}

void CurrentGameWidget::startEdit(){
	// zaczynamy edycję - włączamy odpowiednie menu, i ustawiamy flagę do attemptMove
	stackedWidget->setCurrentWidget(editorMenuWidget);
	QVector<qint32> partialMove;
	QVector<qint32> fullMove = gameState->getMove();
	for(int i = 0; i < gameState->getSegmentCount(); i++){
		partialMove.push_back(fullMove[i]);
	}
	recolorMove(partialMove, blackLine);
	gameState->abandonHistory();
	editorMenuWidget->start();
	placeBallMode = true;
}

void CurrentGameWidget::finishEdit(){
	// kończymy edycję, włączamy gameMenu, usuwamy flagę do attemptMove
	if(gameState->stuck()){
		endGameWidget->stuck(gameState->doesPlayer1MoveNow());
		stackedWidget->setCurrentWidget(endGameWidget);
	}else{
		stackedWidget->setCurrentWidget(gameMenuWidget);
		gameMenuWidget->setGameState(gameState);
	}
	if(editLinePoint != nullptr){
		editLinePoint->setBrush(Qt::black);
		editLinePoint = nullptr;
	}
	ballPoint->setBrush(Qt::red);
	placeBallMode = false;
	editLineMode = false;
}

void CurrentGameWidget::placeBall(qint32 x, qint32 y){
	gameState->placeBall(x, y);
	ballPoint->setBrush(Qt::black);
	ballPoint = getPoint(x, y);
	ballPoint->setBrush(Qt::red);
}

bool CurrentGameWidget::isAdjacent(qint32 x, qint32 y, qint32 x2, qint32 y2) const{
	if((abs(x - x2) <= 1) && (abs(y - y2) <= 1) && !(x == x2 && y == y2)){
		return true;
	}else{
		return false;
	}
}

void CurrentGameWidget::editLine(qint32 x, qint32 y, qint32 x2, qint32 y2){
	// usuwamy linię od (x, y) do (x2, y2).
	qint32 dir;
	if(x2 - x == 1){
		if(y2 - y == 1){
			dir = southeast;
		}else if(y2 - y == 0){
			dir = east;
		}else if(y2 - y == -1){
			dir = northeast;
		}
	}else if(x2 - x == 0){
		if(y2 - y == 1){
			dir = south;
		}else if(y2 - y == -1){
			dir = north;
		}
	}else if(x2 - x == -1){
		if(y2 - y == 1){
			dir = southwest;
		}else if(y2 - y == 0){
			dir = west;
		}else if(y2 - y == -1){
			dir = northwest;
		}
	}

	if(gameState->getBoardState(x, y, dir)){
		gameState->removeLine(x, y, x2, y2, dir);
		getLine(x, y, dir, false)->hide();
	}else{
		gameState->addLine(x, y, x2, y2, dir);
		getLine(x, y, dir, false)->show();
	}
}

void CurrentGameWidget::revert(){
	QVector<qint32> lastMove = gameState->getMove();
	gameState->unWinGame();
	if(lastMove[lastMove.size() - 1] == goal){
		winLine->hide();
		ballPoint->setBrush(Qt::red);
		goalPoint->setBrush(Qt::white);
	}else{
		previousSegment();
	}
	stackedWidget->setCurrentWidget(gameMenuWidget);
	gameState->deleteLastSegment();
}

bool CurrentGameWidget::canSave(){
	if(stackedWidget->currentWidget() == editorMenuWidget){
		cantSaveMsg.setText("Can't save while editing.");
		cantSaveMsg.exec();
		return false;
	}else if(stackedWidget->currentWidget() == endGameWidget){
		cantSaveMsg.setText("Revert the last move to save.");
		cantSaveMsg.exec();
		return false;
	}else if(stackedWidget->currentWidget() == calculatingMoveWidget){
		cantSaveMsg.setText("Wait for AI to finish the move or stop it to save.");
		cantSaveMsg.exec();
		return false;
	}else{
		return true;
	}
}

bool CurrentGameWidget::canEdit(){
	if(stackedWidget->currentWidget() == editorMenuWidget){
		cantEditMsg.setText("You are already editing.");
		cantEditMsg.exec();
		return false;
	}else if(stackedWidget->currentWidget() == endGameWidget && !(endGameWidget->isStuck)){
		cantEditMsg.setText("Revert the last move to edit.");
		cantEditMsg.exec();
		return false;
	}else if(stackedWidget->currentWidget() == calculatingMoveWidget){
		cantEditMsg.setText("Wait for AI to finish the move or stop it to edit.");
		cantEditMsg.exec();
		return false;
	}else{
		return true;
	}
}
