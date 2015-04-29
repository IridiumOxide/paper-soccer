#include "editormenuwidget.h"

EditorMenuWidget::EditorMenuWidget(QWidget *parent) :
	QWidget(parent)
{
	editorMenuLayout = new QVBoxLayout(this);

	placeBallButton = new QPushButton(this);
	placeBallButton->setText("Modify ball position (Alt + &B)");
	connect(placeBallButton, &QPushButton::clicked, [this](){
		placeBallButton->setStyleSheet("color: black; background-color: red");
		editLineButton->setStyleSheet("color: black; background-color: white");
		emit placeBallMode();});
	editorMenuLayout->addWidget(placeBallButton);

	editLineButton = new QPushButton(this);
	editLineButton->setText("Modify field lines (Alt + &G)");
	connect(editLineButton, &QPushButton::clicked, [this](){
		placeBallButton->setStyleSheet("color: black; background-color: white");
		editLineButton->setStyleSheet("color: black; background-color: yellow");
		emit editLineMode();});
	editorMenuLayout->addWidget(editLineButton);

	playerChangeButton = new QPushButton(this);
	connect(playerChangeButton, &QPushButton::clicked, [this](){
		gameState->changePlayer();
		updatePlayer();
	});
	editorMenuLayout->addWidget(playerChangeButton);

	checkboxWidget = new QWidget(this);
	checkboxesLayout = new QHBoxLayout(checkboxWidget);
	p1AICheckBox = new QCheckBox(this);
	p1AICheckBox->setText("Player 1 controlled by AI");
	p2AICheckBox = new QCheckBox(this);
	p2AICheckBox->setText("Player 2 controlled by AI");
	checkboxesLayout->addWidget(p1AICheckBox);
	checkboxesLayout->addWidget(p2AICheckBox);
	checkboxWidget->setLayout(checkboxesLayout);

	editorMenuLayout->addWidget(checkboxWidget);

	finishButton = new QPushButton(this);
	finishButton->setText("Finish Edit (Alt + &Q)");
	connect(finishButton, &QPushButton::clicked, [this](){
		gameState->setPlayer1AI(p1AICheckBox->isChecked());
		gameState->setPlayer2AI(p2AICheckBox->isChecked());
		emit finishEdit();
	});
	editorMenuLayout->addWidget(finishButton);

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
	QString text = "";
	text += "When the PLACE BALL MODE is activated, click a point\n";
	text += "to place the ball there. When the EDIT LINE MODE is\n";
	text += "activated, click two adjacent points to add/remove the line\n";
	text += "between them. To change modes, use buttons.\n";
	text += "The goal cannot be modified.";
	infoLabel->setText(text);
	infoLabel->hide();

	editorMenuLayout->addWidget(infoButton);
	editorMenuLayout->addWidget(infoLabel);

	setLayout(editorMenuLayout);

}

void EditorMenuWidget::setGameState(GameState *g){
	gameState = g;
	updatePlayer();
}

void EditorMenuWidget::updatePlayer(){
	if(gameState->doesPlayer1MoveNow()){
		playerChangeButton->setText("Current Player: Player 1.\nClick to change. (Alt + &T)");
	}else{
		playerChangeButton->setText("Current Player: Player 2.\nClick to change. (Alt + &T)");
	}
}

void EditorMenuWidget::start(){
	placeBallButton->setStyleSheet("color: black; background-color: red");
	editLineButton->setStyleSheet("color: black; background-color: white");
	p1AICheckBox->setChecked(gameState->isP1ControlledByAI());
	p2AICheckBox->setChecked(gameState->isP2ControlledByAI());
	updatePlayer();
}
