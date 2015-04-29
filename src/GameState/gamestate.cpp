#include "gamestate.h"

GameState::GameState(QWidget *parent, qint32 bLen, qint32 bWid, bool p1AI, bool p2AI) :
	QObject(parent),
	boardLength(bLen),
	boardWidth(bWid),
	moveFinished(false),
	player1MovesNow(true),
	gameFinished(false),
	player1Won(false),
	p1ControlledByAI(p1AI),
	p2ControlledByAI(p2AI),
	turnNumber(0),
	turnCount(0),
	segmentNumber(0),
	segmentCount(0),
	ballX(0),
	ballY(0)
{
	QVector<qint32> emptyVector;
	moves.push_back(emptyVector);

	qint32 goalLeft = ( boardWidth / 2 ) - 1;
	qint32 goalCenter = boardWidth / 2;
	qint32 goalRight = ( boardWidth / 2 ) + 1;

	for(int i = 0; i <= boardWidth; i++){
		for(int j = 0; j <= boardLength; j++){

			// Ustawienie ilości zajętych kierunków
			if((i == goalLeft || i == goalRight) && (j == 0 || j == boardLength)){
				// słupki bramki
				edgesDrawn[i][j] = 3;
			}else if((i == goalCenter) && (j == 0 || j == boardLength)){
				// pole przed bramką
				edgesDrawn[i][j] = 0;
			}else if((i == 0 || i == boardWidth) && (j == 0 || j == boardLength)){
				// rogi
				edgesDrawn[i][j] = 7;
			}else if(i == 0 || i == boardWidth || j == 0 || j == boardLength){
				// bandy
				edgesDrawn[i][j] = 5;
			}else{
				// pozostałe pola
				edgesDrawn[i][j] = 0;
			}

			boardState[i][j][south] = false;
			boardState[i][j][southeast] = false;
			boardState[i][j][east] = false;
			boardState[i][j][northeast] = false;

			// Ustawienie linii bandy
			if((j == 0 || j == boardLength) && i < boardWidth){
				// banda pozioma, ustawiamy kreskę na wschód
				// chyba że jest to na bramce
				if(!(i == goalLeft || i == goalCenter)){
					boardState[i][j][east] = true;
				}
			}
			if((i == 0 || i == boardWidth) && j < boardLength){
				// banda pionowa, ustawiamy kreskę na południe
				boardState[i][j][south] = true;
			}
		}
	}

	datalossWarningBox.setWindowTitle("Warning");
	datalossWarningBox.setText("You will lose future moves. Do you want to continue?");
	datalossWarningBox.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
}

bool GameState::getBoardState(qint32 x, qint32 y, qint32 dir) const{
	if(dir >= 4){
		switch(dir){
		case north: y--; break;
		case northwest: x--; y--; break;
		case west: x--; break;
		case southwest: x--; y++; break;
		}
		dir -= 4;
	}
	return boardState[x + getHalfBoardWidth()][y + getHalfBoardLength()][dir];
}

bool GameState::getBoardStateRaw(qint32 x, qint32 y, qint32 dir) const{
	return boardState[x][y][dir];
}

void GameState::setBoardState(qint32 x, qint32 y, qint32 dir, bool b){
	if(dir >= 4){
		switch(dir){
		case north: y--; break;
		case northwest: x--; y--; break;
		case west: x--; break;
		case southwest: x--; y++; break;
		}
		dir -= 4;
	}
	boardState[x + getHalfBoardWidth()][y + getHalfBoardLength()][dir] = b;
}

qint32 GameState::getEdgesDrawn(qint32 x, qint32 y) const{
	return edgesDrawn[x + getHalfBoardWidth()][y + getHalfBoardLength()];
}

qint32 GameState::getEdgesDrawnRaw(qint32 x, qint32 y) const{
	return edgesDrawn[x][y];
}

void GameState::setEdgesDrawn(qint32 x, qint32 y, qint32 value){
	edgesDrawn[x + getHalfBoardWidth()][y + getHalfBoardLength()] = value;
}

void GameState::incrementEdgesDrawn(qint32 x, qint32 y){
	edgesDrawn[x + getHalfBoardWidth()][y + getHalfBoardLength()]++;
}

void GameState::decrementEdgesDrawn(qint32 x, qint32 y){
	edgesDrawn[x + getHalfBoardWidth()][y + getHalfBoardLength()]--;
}

void GameState::addLine(qint32 x, qint32 y, qint32 x2, qint32 y2, qint32 dir){
	incrementEdgesDrawn(x, y);
	incrementEdgesDrawn(x2, y2);
	setBoardState(x, y, dir, true);
}

void GameState::removeLine(qint32 x, qint32 y, qint32 x2, qint32 y2, qint32 dir){
	decrementEdgesDrawn(x, y);
	decrementEdgesDrawn(x2, y2);
	setBoardState(x, y, dir, false);
}

void GameState::abandonHistory(){
	turnNumber = 0;
	turnCount = 0;
	segmentNumber = 0;
	segmentCount = 0;
	moveFinished = false;
	moves.clear();
	QVector<qint32> empty;
	moves.push_back(empty);
}

void GameState::placeBall(qint32 x, qint32 y){
	ballX = x;
	ballY = y;
}

void GameState::changePlayer(){
	player1MovesNow = !player1MovesNow;
}

void GameState::setPlayer1AI(bool b){
	p1ControlledByAI = b;
}

void GameState::setPlayer2AI(bool b){
	p2ControlledByAI = b;
}

bool GameState::isP1ControlledByAI() const{
	return p1ControlledByAI;
}

bool GameState::isP2ControlledByAI() const{
	return p2ControlledByAI;
}

bool GameState::doesPlayer1MoveNow() const{
	return player1MovesNow;
}

bool GameState::isMoveFinished() const{
	return moveFinished;
}

bool GameState::isGameFinished() const{
	return gameFinished;
}

bool GameState::didPlayer1Win() const{
	return player1Won;
}

bool GameState::stuck() const{
	if(getEdgesDrawn(ballX, ballY) == 8){
		return true;
	}else{
		return false;
	}
}

qint32 GameState::getHalfBoardLength() const{
	return boardLength / 2;
}

qint32 GameState::getHalfBoardWidth() const{
	return boardWidth / 2;
}

qint32 GameState::getBallX() const{
	return ballX;
}

qint32 GameState::getBallY() const{
	return ballY;
}

qint32 GameState::getTurnNumber() const{
	return turnNumber;
}

qint32 GameState::getTurnCount() const{
	return turnCount;
}

qint32 GameState::getSegmentNumber() const{
	return segmentNumber;
}

qint32 GameState::getSegmentCount() const{
	return segmentCount;
}

qint32 GameState::reverseDir(qint32 dir) const{
	return (dir + 4) % 8;
}

qint32 GameState::xofDir(qint32 dir) const{
	static const qint32 xes[8] = {0, 1, 1, 1, 0, -1, -1, -1};
	return xes[dir];
}
qint32 GameState::yofDir(qint32 dir) const{
	static const qint32 yses[8] = {1, 1, 0, -1, -1, -1, 0, 1};
	return yses[dir];
}

QVector<qint32> GameState::getMove() const{
	return moves[turnCount];
}

void GameState::saveGame() const{
	//QFileDialog saveDialog;
	//saveDialog.setDefaultSuffix(".gsf");
	QString filter = "All files (*)";
	QString fileName = QFileDialog::getSaveFileName(nullptr, "Save the game", ".", filter, &filter);
	if(fileName.isEmpty()){
		return;
	}else{
		QFile file(fileName);
		if (!file.open(QIODevice::WriteOnly)) {
			QMessageBox::information(nullptr, "Unable to open file", file.errorString());
			return;
		}
		QDataStream out(&file);
		out << boardLength << boardWidth << p1ControlledByAI << p2ControlledByAI;
		out << moveFinished << player1MovesNow << gameFinished << player1Won;
		for(int i = 0; i < MAX_LENGTH + 1; i++){
			for(qint32 j = 0; j < MAX_WIDTH + 1; j++){
				for(qint32 k = 0; k < NUMBER_OF_DIRECTIONS; k++){
					out << boardState[i][j][k];
				}
			}
		}
		for(int i = 0; i < MAX_LENGTH + 1; i++){
			for(qint32 j = 0; j < MAX_WIDTH + 1; j++){
				out << edgesDrawn[i][j];
			}
		}
		out << moves;
		out << turnNumber << turnCount << segmentNumber << segmentCount << ballX << ballY;
		file.close();
	}
}

void GameState::loadGame(QDataStream *in){
	*in >> moveFinished >> player1MovesNow >> gameFinished >> player1Won;
	for(int i = 0; i < MAX_LENGTH + 1; i++){
		for(qint32 j = 0; j < MAX_WIDTH + 1; j++){
			for(qint32 k = 0; k < NUMBER_OF_DIRECTIONS; k++){
				*in >> boardState[i][j][k];
			}
		}
	}
	for(int i = 0; i < MAX_LENGTH + 1; i++){
		for(qint32 j = 0; j < MAX_WIDTH + 1; j++){
			*in >> edgesDrawn[i][j];
		}
	}
	*in >> moves;
	*in >> turnNumber >> turnCount >> segmentNumber >> segmentCount >> ballX >> ballY;
}

moveInfo GameState::previousMove(){
	moveInfo move;
	if(turnCount == 0 || segmentCount != 0){
		move.valid = false;
		return move;
	}else{
		move.valid = true;
		turnCount--;
		move.directions = moves[turnCount];
		player1MovesNow = !player1MovesNow;
		segmentCount = moves[turnCount].size();
		segmentNumber = moves[turnCount].size();
		setMoveFinished(true);
		return move;
	}
}

segmentInfo GameState::previousSegment(){
	segmentInfo segment;
	if(segmentCount == 0){
		segment.valid = false;
		return segment;
	}else{
		segment.valid = true;
		segment.lineDir = moves[turnCount][segmentCount - 1];
		setBoardState(ballX, ballY, reverseDir(segment.lineDir), false);
		decrementEdgesDrawn(ballX, ballY);
		ballX -= xofDir(segment.lineDir);
		ballY -= yofDir(segment.lineDir);
		decrementEdgesDrawn(ballX, ballY);
		segmentCount--;
		setMoveFinished(false);
		return segment;
	}
}

moveInfo GameState::nextMove(){
	moveInfo move;
	if(turnCount == turnNumber){
		move.valid = false;
		return move;
	}else{
		move.valid = true;
		move.directions = moves[turnCount];
		turnCount++;
		player1MovesNow = !player1MovesNow;
		segmentCount = 0;
		segmentNumber = moves[turnCount].size();
		setMoveFinished(false);
		return move;
	}
}

segmentInfo GameState::nextSegment(){
	segmentInfo segment;
	if(segmentCount == segmentNumber){
		segment.valid = false;
		return segment;
	}else{
		segment.valid = true;
		segmentCount++;
		segment.lineDir = moves[turnCount][segmentCount - 1];
		setBoardState(ballX, ballY, segment.lineDir, true);
		incrementEdgesDrawn(ballX, ballY);
		ballX += xofDir(segment.lineDir);
		ballY += yofDir(segment.lineDir);
		incrementEdgesDrawn(ballX, ballY);

		if(getEdgesDrawn(ballX, ballY) == 8){
			gameFinished = true;
			player1Won = player1MovesNow;
		}else if(finished()){
			setMoveFinished(true);
		}
		return segment;
	}
}

void GameState::deleteLastSegment(){
	if(moves[turnCount][segmentCount] != goal){
		segmentNumber--;
	}
	moves[turnCount].pop_back();
}

bool GameState::confirmMove(){
	if(turnCount != turnNumber){
		if(datalossWarningBox.exec() == QMessageBox::Yes){
			moves.resize(turnCount + 1);
			turnNumber = turnCount;
		}else{
			return false;
		}
	}
	changePlayer();
	turnNumber++;
	turnCount++;
	segmentCount = 0;
	segmentNumber = 0;
	QVector<qint32> emptyVector;
	moves.push_back(emptyVector);
	setMoveFinished(false);
	return true;
}

void GameState::setMoveFinished(bool b){
	moveFinished = b;
}

bool GameState::finished(){
	bool result = (getEdgesDrawn(ballX, ballY) == 1);
	result = result || ((getEdgesDrawn(ballX, ballY) == 4) && ((ballY == -getHalfBoardLength()) || (ballY == getHalfBoardLength()) || (ballX == -getHalfBoardWidth()) || (ballX == getHalfBoardWidth())));
	result = result || ((getEdgesDrawn(ballX, ballY) == 6) && (((ballY == -getHalfBoardLength()) || (ballY == getHalfBoardLength())) && ((ballX == -getHalfBoardWidth()) || (ballX == getHalfBoardWidth()))));
	result = result && !((ballX == 1 || ballX == -1) && (((ballY == -getHalfBoardLength()) || (ballY == getHalfBoardLength()))));
	return result;
}

segmentInfo GameState::attemptGoal(qint32 x, bool isNorth){
	segmentInfo result;

	if(gameFinished){
		result.valid = false;
		return result;
	}

	if(moveFinished){
		result.valid = false;
		return result;
	}

	if(isNorth){
		if(ballY != -getHalfBoardLength()){
			result.valid = false;
			return result;
		}
		if(ballX == -1){
			if(x == 0){
				result.valid = true;
				result.lineDir = northeast;
			}else{
				result.valid = false;
			}
		}else if(ballX == 0){
			result.valid = true;
			if(x == -1){
				result.lineDir = northwest;
			}else if(x == 0){
				result.lineDir = north;
			}else if(x == 1){
				result.lineDir = northeast;
			}else{
				result.valid = false;
			}
		}else if(ballX == 1){
			if(x == 0){
				result.valid = true;
				result.lineDir = northwest;
			}else{
				result.valid = false;
			}
		}else{
			result.valid = false;
		}
	}else{
		if(ballY != getHalfBoardLength()){
			result.valid = false;
			return result;
		}
		if(ballX == -1){
			if(x == 0){
				result.valid = true;
				result.lineDir = southeast;
			}else{
				result.valid = false;
			}
		}else if(ballX == 0){
			result.valid = true;
			if(x == -1){
				result.lineDir = southwest;
			}else if(x == 0){
				result.lineDir = south;
			}else if(x == 1){
				result.lineDir = southeast;
			}else{
				result.valid = false;
			}
		}else if(ballX == 1){
			if(x == 0){
				result.valid = true;
				result.lineDir = southwest;
			}else{
				result.valid = false;
			}
		}else{
			result.valid = false;
		}
	}

	if(turnCount != turnNumber || segmentCount != segmentNumber){
		if(datalossWarningBox.exec() == QMessageBox::Yes){
			moves.resize(turnCount + 1);
			moves[turnCount].resize(segmentCount);
			turnNumber = turnCount;
			segmentNumber = segmentCount;
		}else{
			result.valid = false;
			return result;
		}
	}

	if(result.valid == true){
		// bramka strzelona
		gameFinished = true;

		if(isNorth){
			player1Won = false;
		}else{
			player1Won = true;
		}

		moves[turnCount].push_back(goal);
	}
	return result;
}

segmentInfo GameState::attemptMove(qint32 x, qint32 y){
	segmentInfo result;
	if(x == ballX && y == ballY){
		result.valid = false;
		return result;
	}
	if(abs(x - ballX) > 1 || abs(y - ballY) > 1){
		result.valid = false;
		return result;
	}
	if(moveFinished){
		result.valid = false;
		return result;
	}
	if(gameFinished){
		result.valid = false;
		return result;
	}

	if(x - ballX == 1){
		if(y - ballY == 1){
			result.lineDir = southeast;
		}else if(y - ballY == 0){
			result.lineDir = east;
		}else if(y - ballY == -1){
			result.lineDir = northeast;
		}
	}else if(x - ballX == 0){
		if(y - ballY == 1){
			result.lineDir = south;
		}else if(y - ballY == -1){
			result.lineDir = north;
		}
	}else if(x - ballX == -1){
		if(y - ballY == 1){
			result.lineDir = southwest;
		}else if(y - ballY == 0){
			result.lineDir = west;
		}else if(y - ballY == -1){
			result.lineDir = northwest;
		}
	}

	if(getBoardState(ballX, ballY, result.lineDir) == true){
		result.valid = false;
		return result;
	}else{
		if(turnCount != turnNumber || segmentCount != segmentNumber){
			if(datalossWarningBox.exec() == QMessageBox::Yes){
				moves.resize(turnCount + 1);
				moves[turnCount].resize(segmentCount);
				turnNumber = turnCount;
				segmentNumber = segmentCount;
			}else{
				result.valid = false;
				return result;
			}
		}
		result.valid = true;

		moves[turnCount].push_back(result.lineDir);
		segmentCount++;
		segmentNumber++;

		incrementEdgesDrawn(ballX, ballY);
		incrementEdgesDrawn(x, y);
		setBoardState(ballX, ballY, result.lineDir, true);
		ballX = x;
		ballY = y;

		if(getEdgesDrawn(x, y) == 8){
			gameFinished = true;
			player1Won = !player1MovesNow;
		}else if(finished()){
			setMoveFinished(true);
		}
		return result;
	}
}

void GameState::unWinGame(){
	gameFinished = false;
}
