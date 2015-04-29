#include "ai.h"

AI::AI(QObject *parent) :
	QObject(parent)
{
}

QVector<qint32> AI::AIGetMove(GameState *g){
	halt = false;
	gameState = g;
	ballX = gameState->getBallX();
	ballY = gameState->getBallY();
	for(int i = 0; i <= GameState::MAX_WIDTH; i++){
		for(int j = 0; j <= GameState::MAX_LENGTH; j++){
			QApplication::processEvents();
			if(halt){
				QVector<qint32> empty;
				return empty;
			}
			for(int k = 0; k < GameState::NUMBER_OF_DIRECTIONS; k++){
				boardState[i][j][k] = gameState->getBoardStateRaw(i, j, k);
			}
			edgesDrawn[i][j] = gameState->getEdgesDrawnRaw(i, j);
		}
	}
	qint32 reqY;
	QVector<qint32> answer;
	if(gameState->doesPlayer1MoveNow()){
		reqY = 1;
	}else{
		reqY = -1;
	}
	bool moveEnded = gameState->isMoveFinished();
	//qDebug()<<"STARTING NEXT MOVE";
	while(!moveEnded){
		//qDebug()<<"NEXT SEGMENT, STARTING AT" <<ballX <<ballY;
		QApplication::processEvents();
		if(halt){
			QVector<qint32> empty;
			return empty;
		}
		qint32 bestDirection = -1;
		qint32 bestValue = -1;
		qint32 currentValue;
		for(int i = 0; i < GameState::NUMBER_OF_ALL_DIRECTIONS; i++){
			if(canGo(ballX, ballY, i)){
				currentValue = 0;

				if(gameState->yofDir(i) == reqY){
					currentValue += 16;
				}else if(gameState->yofDir(i) != -reqY){
					currentValue += 8;
				}

				if(gameState->getEdgesDrawn(ballX + gameState->xofDir(i), ballY + gameState->yofDir(i)) != 0){
					currentValue += 12;
				}

				if(gameState->xofDir(i) * ballX < 0){
					currentValue += 2;
				}else if(gameState->xofDir(i) * ballX == 0){
					currentValue += 1;
				}

				if(gameState->getEdgesDrawn(ballX + gameState->xofDir(i), ballY + gameState->yofDir(i)) == 7){
					currentValue = 0;
				}

				if(currentValue > bestValue){
					bestValue = currentValue;
					bestDirection = i;
				}
				//qDebug()<<i <<":  " <<currentValue;
			}
		}

		// tutaj jeszcze sprawdzamy możliwość strzelenia gola (sobie lub przeciwnikowi)
		if(ballX >= -1 && ballX <= 1){
			if(ballY == gameState->getHalfBoardLength() * reqY){
				// pod bramką przeciwnika - strzelamy, gdy się da
				bestDirection = goal;
			}

			if(ballY == gameState->getHalfBoardLength() * reqY * (-1)){
				// pod swoją bramką - musimy strzelić, jeśli nie ma innego ruchu
				if(bestDirection == -1){
					bestDirection = goal;
				}
			}

		}

		answer.push_back(bestDirection);
		go(bestDirection);
		//qDebug()<<"Went" <<bestDirection;
		//qDebug()<<"There are" <<getEdgesDrawn(ballX, ballY) <<"edges drawn.";
		if(getEdgesDrawn(ballX, ballY) == 1){
			moveEnded = true;
		}
		if(getEdgesDrawn(ballX, ballY) == 8){
			moveEnded = true;
		}
		if(bestDirection == goal){
			moveEnded = true;
		}
	}
	//qDebug()<<"Full move:" <<answer;
	return answer;
}

bool AI::canGo(qint32 x, qint32 y, qint32 dir) const{
	if(x == -(gameState->getHalfBoardWidth()) && gameState->xofDir(dir) == -1){
		//qDebug()<<"can't move " <<x <<y <<dir;
		return false;
	}
	if(x == gameState->getHalfBoardWidth() && gameState->xofDir(dir) == 1){
		//qDebug()<<"can't move " <<x <<y <<dir;
		return false;
	}
	if(y == -(gameState->getHalfBoardLength()) && gameState->yofDir(dir) == -1){
		//qDebug()<<"can't move " <<x <<y <<dir;
		return false;
	}
	if(y == gameState->getHalfBoardLength() && gameState->yofDir(dir) == 1){
		//qDebug()<<"can't move " <<x <<y <<dir;
		return false;
	}
	if(getBoardState(x, y, dir)){
		//qDebug()<<"can't move " <<x <<y <<dir;
		return false;
	}
	//qDebug()<<"Can go" <<x <<y <<dir <<".";
	return true;
}

bool AI::getBoardState(qint32 x, qint32 y, qint32 dir) const{
	if(dir >= 4){
		switch(dir){
		case north: y--; break;
		case northwest: x--; y--; break;
		case west: x--; break;
		case southwest: x--; y++; break;
		}
		dir -= 4;
	}
	return boardState[x + gameState->getHalfBoardWidth()][y + gameState->getHalfBoardLength()][dir];
}

void AI::setBoardState(qint32 x, qint32 y, qint32 dir, bool b){
	if(dir >= 4){
		switch(dir){
		case north: y--; break;
		case northwest: x--; y--; break;
		case west: x--; break;
		case southwest: x--; y++; break;
		}
		dir -= 4;
	}
	boardState[x + gameState->getHalfBoardWidth()][y + gameState->getHalfBoardLength()][dir] = b;
}

qint32 AI::getEdgesDrawn(qint32 x, qint32 y) const{
	return edgesDrawn[x + gameState->getHalfBoardWidth()][y + gameState->getHalfBoardLength()];
}

void AI::incrementEdgesDrawn(qint32 x, qint32 y){
	edgesDrawn[x + gameState->getHalfBoardWidth()][y + gameState->getHalfBoardLength()]++;
}

void AI::go(qint32 dir){
	incrementEdgesDrawn(ballX, ballY);
	setBoardState(ballX, ballY, dir, true);
	ballX += gameState->xofDir(dir);
	ballY += gameState->yofDir(dir);
	incrementEdgesDrawn(ballX, ballY);
}

void AI::stopAI(){
	halt = true;
}
