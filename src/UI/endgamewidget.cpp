#include "endgamewidget.h"

EndGameWidget::EndGameWidget(QWidget *parent) :
	QWidget(parent),
	isStuck(false)
{
	endGameLayout = new QVBoxLayout(this);

	winInfoLabel = new QLabel(this);
	winInfoLabel->setAlignment(Qt::AlignCenter);
	winInfoLabel->setStyleSheet("color: white; background-color: black; font-size: 30px");
	endGameLayout->addWidget(winInfoLabel);

	revertButton = new QPushButton("Revert last move (Alt + &R)", this);
	connect(revertButton, &QPushButton::clicked, [this](){emit revert();});
	endGameLayout->addWidget(revertButton);

	editButton =new QPushButton("Go back to edit (Alt + &E)", this);
	connect(editButton, &QPushButton::clicked, [this](){emit backEdit();});
	endGameLayout->addWidget(editButton);

	setLayout(endGameLayout);
}

void EndGameWidget::display(bool player1){
	isStuck = false;
	if(player1){
		winInfoLabel->setText("PLAYER 1 WON!");
	}else{
		winInfoLabel->setText("PLAYER 2 WON!");
	}
	revertButton->show();
	editButton->hide();
}

void EndGameWidget::stuck(bool player1){
	isStuck = true;
	QString s = "Congratulations.\n The ball got stuck.\n";
	if(player1){
		winInfoLabel->setText(s + "Player 1 won anyway.");
	}else{
		winInfoLabel->setText(s + "Player 2 won anyway.");
	}
	revertButton->hide();
	editButton->show();
}
