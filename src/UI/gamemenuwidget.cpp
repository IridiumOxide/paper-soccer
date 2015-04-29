#include "gamemenuwidget.h"

GameMenuWidget::GameMenuWidget(QWidget *parent) :
	QWidget(parent)
{
	playerNamesWidget = new QWidget(this);
	playerNamesLayout = new QHBoxLayout(this);
	player1Label = new QLabel(this);
	player1Label->setAlignment(Qt::AlignCenter);
	player2Label = new QLabel(this);
	player2Label->setAlignment(Qt::AlignCenter);
	playerNamesLayout->addWidget(player1Label);
	playerNamesLayout->addWidget(player2Label);
	playerNamesWidget->setLayout(playerNamesLayout);

	historyButtonsWidget = new QWidget(this);
	historyButtonsLayout = new QGridLayout(this);
	nextMoveButton = new QPushButton("Next move (Alt + &Q)", this);
	connect(nextMoveButton, &QPushButton::clicked, [this](){emit nextMove();});
	previousMoveButton = new QPushButton("Previous move (Alt + &A)", this);
	connect(previousMoveButton, &QPushButton::clicked, [this](){emit previousMove();});
	nextSegmentButton = new QPushButton("Next segment (Alt + &W)", this);
	connect(nextSegmentButton, &QPushButton::clicked, [this](){emit nextSegment();});
	previousSegmentButton = new QPushButton("Previous segment (Alt + &S)", this);
	connect(previousSegmentButton, &QPushButton::clicked, [this](){emit previousSegment();});
	historyButtonsLayout->addWidget(nextMoveButton, 0, 0, 1, 1);
	historyButtonsLayout->addWidget(previousMoveButton, 1, 0, 1, 1);
	historyButtonsLayout->addWidget(nextSegmentButton, 0, 1, 1, 1);
	historyButtonsLayout->addWidget(previousSegmentButton, 1, 1, 1, 1);
	historyButtonsWidget->setLayout(historyButtonsLayout);

	infoButton = new QPushButton("Show instructions (Alt + &H)", this);
	connect(infoButton, &QPushButton::clicked, [this](){
		if(infoLabel->isHidden()){
			infoButton->setText("Hide instructions (Alt + &H)");
			infoLabel->show();
		}else{
			infoButton->setText("Show instructions (Alt + &H)");
			infoLabel->hide();
		}
	});
	infoLabel = new QLabel(this);
	infoLabel->setStyleSheet("color: black; background-color: white");
	infoLabel->setAlignment(Qt::AlignCenter);
	QString infoText = "";
	infoText += "The ball is in the point marked with the red color.\n";
	infoText += "You can move the ball to a horizontally, vertically,\n";
	infoText += "or diagonally neighbouring point by clicking it, provided\n";
	infoText += "that a line between this point and where ball currently\n";
	infoText += "is doesn't exist yet. If there is at least one line connected\n";
	infoText += "to that point from elsewhere, you can make another move this turn.\n";
	infoText += "You can win the game by moving the ball into your opponent's\n";
	infoText += "goal, which is represented by white points.\n";
	infoText += "Player 1 should shoot south, player 2 should shoot north.\n";
	infoLabel->setText(infoText);
	infoLabel->hide();


	confirmMoveButton = new QPushButton("CONFIRM MOVE (Alt + &E)", this);
	connect(confirmMoveButton, &QPushButton::clicked, [this](){emit confirmMove();});
	hintButton = new QPushButton("HINT (Alt + &D)", this);
	connect(hintButton, &QPushButton::clicked, [this](){emit getHint();});

	bottomWidget = new QWidget(this);
	bottomLayout = new QHBoxLayout(this);
	autoconfirmAI = new QCheckBox("Autoconfirm AI moves (Alt + &Z)", this);
	resumeAIButton = new QPushButton("Resume AI processing (Alt + &X)", this);
	connect(resumeAIButton, &QPushButton::clicked, [this](){emit resumeAI();});
	bottomLayout->addWidget(autoconfirmAI);
	bottomLayout->addWidget(resumeAIButton);
	bottomWidget->setLayout(bottomLayout);

	gameMenuLayout = new QVBoxLayout(this);
	gameMenuLayout->addWidget(playerNamesWidget);
	gameMenuLayout->addWidget(historyButtonsWidget);
	gameMenuLayout->addWidget(confirmMoveButton);
	gameMenuLayout->addWidget(hintButton);
	gameMenuLayout->addWidget(bottomWidget);
	gameMenuLayout->addWidget(infoButton);
	gameMenuLayout->addWidget(infoLabel);
	setLayout(gameMenuLayout);

}

bool GameMenuWidget::autoconfirmAIToggled() const{
	return autoconfirmAI->isChecked();
}

void GameMenuWidget::nextMoveSetEnabled(bool b){
	nextMoveButton->setEnabled(b);
}

void GameMenuWidget::nextSegmentSetEnabled(bool b){
	nextSegmentButton->setEnabled(b);
}

void GameMenuWidget::previousMoveSetEnabled(bool b){
	previousMoveButton->setEnabled(b);
}

void GameMenuWidget::previousSegmentSetEnabled(bool b){
	previousSegmentButton->setEnabled(b);
}

void GameMenuWidget::confirmMoveSetEnabled(bool b){
	confirmMoveButton->setEnabled(b);
}

void GameMenuWidget::resumeAISetEnabled(bool b){
	resumeAIButton->setEnabled(b);
}

void GameMenuWidget::hintSetEnabled(bool b){
	hintButton->setEnabled(b);
}

void GameMenuWidget::disableAutoconfirmBox(){
	autoconfirmAI->setDisabled(true);
}

void GameMenuWidget::adjustPlayer(){
	if(gameState->doesPlayer1MoveNow()){
		unhighlight(player2Label);
		highlight(player1Label);

		hintButton->setDisabled(gameState->isP1ControlledByAI());
		resumeAIButton->setEnabled(gameState->isP1ControlledByAI());
	}else{
		highlight(player2Label);
		unhighlight(player1Label);

		hintButton->setDisabled(gameState->isP2ControlledByAI());
		resumeAIButton->setEnabled(gameState->isP2ControlledByAI());
	}

}

void GameMenuWidget::highlight(QLabel *l){
	l->setStyleSheet("background-color: yellow");
}

void GameMenuWidget::unhighlight(QLabel *l){
	l->setStyleSheet("background-color: none");
}

void GameMenuWidget::setGameState(GameState *g){
	gameState = g;

	if(gameState->isP1ControlledByAI()){
		player1Label->setText("AI PLAYER 1\n(SHOOT SOUTH)");
	}else{
		player1Label->setText("HUMAN PLAYER 1\n(SHOOT SOUTH)");
	}

	if(gameState->isP2ControlledByAI()){
		player2Label->setText("AI PLAYER 2\n(SHOOT NORTH)");
	}else{
		player2Label->setText("HUMAN PLAYER 2\n(SHOOT NORTH)");
	}

	if(gameState->isP1ControlledByAI() || gameState->isP2ControlledByAI()){
		autoconfirmAI->setEnabled(true);
		autoconfirmAI->setChecked(true);
	}else{
		autoconfirmAI->setDisabled(true);
		autoconfirmAI->setChecked(false);
	}
	unhighlight(player1Label);
	unhighlight(player2Label);

	if(gameState->doesPlayer1MoveNow()){
		highlight(player1Label);
	}else{
		highlight(player2Label);
	}

	if(gameState->getSegmentCount() == 0){
		previousSegmentSetEnabled(false);
	}else{
		previousSegmentSetEnabled(true);
	}

	if(gameState->getSegmentCount() == gameState->getSegmentNumber()){
		nextSegmentSetEnabled(false);
	}else{
		nextSegmentSetEnabled(true);
	}

	if(gameState->getTurnCount() == 0){
		previousMoveSetEnabled(false);
	}else{
		previousMoveSetEnabled(true);
	}

	if(gameState->getTurnCount() == gameState->getTurnNumber()){
		nextMoveSetEnabled(false);
	}else{
		nextMoveSetEnabled(true);
	}

	if(gameState->isMoveFinished()){
		confirmMoveSetEnabled(true);
	}else{
		confirmMoveSetEnabled(false);
	}

	adjustPlayer();
}
