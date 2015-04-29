#include "newgamewidget.h"

NewGameWidget::NewGameWidget(QWidget *parent) :
	QWidget(parent),
	boardLength(10),
	boardWidth(8)
{
	lengthChoiceWidget = new QWidget(this);
	lengthChoiceLayout = new QHBoxLayout(this);
	lengthLabel = new QLabel(QString::number(boardLength), this);
	lengthChangeButton = new QPushButton("CHANGE", this);
	connect(lengthChangeButton, SIGNAL(clicked()), this, SLOT(changeLength()));
	lengthChoiceLayout->addWidget(new QLabel("LENGTH\n(VERTICAL)", this));
	lengthChoiceLayout->addWidget(lengthLabel);
	lengthChoiceLayout->addWidget(lengthChangeButton);
	lengthChoiceWidget->setLayout(lengthChoiceLayout);
	oddLengthMsgBox.setText("Length value must be an even number!");
	oddLengthMsgBox.setWindowTitle("Invalid length");

	widthChoiceWidget = new QWidget(this);
	widthChoiceLayout = new QHBoxLayout(this);
	widthLabel = new QLabel(QString::number(boardWidth), this);
	widthChangeButton = new QPushButton("CHANGE", this);
	connect(widthChangeButton, SIGNAL(clicked()), this, SLOT(changeWidth()));
	widthChoiceLayout->addWidget(new QLabel("WIDTH\n(HORIZONTAL)", this));
	widthChoiceLayout->addWidget(widthLabel);
	widthChoiceLayout->addWidget(widthChangeButton);
	widthChoiceWidget->setLayout(widthChoiceLayout);
	oddWidthMsgBox.setText("Width value must be an even number!");
	oddWidthMsgBox.setWindowTitle("Invalid width");

	computerPlayerChoiceWidget = new QWidget(this);
	computerPlayerChoiceLayout = new QVBoxLayout(this);
	p1ControlledByAI = new QCheckBox("Player 1 controlled by AI", this);
	p2ControlledByAI = new QCheckBox("Player 2 controlled by AI", this);
	computerPlayerChoiceLayout->addWidget(p1ControlledByAI);
	computerPlayerChoiceLayout->addWidget(p2ControlledByAI);
	computerPlayerChoiceWidget->setLayout(computerPlayerChoiceLayout);

	playButtonWidget = new QWidget(this);
	playButtonLayout = new QHBoxLayout(this);
	playButton = new QPushButton("PLAY", this);
	connect(playButton, SIGNAL(clicked()), this, SLOT(emitNewGame()));
	playButtonLayout->addWidget(computerPlayerChoiceWidget);
	playButtonLayout->addWidget(playButton);
	playButtonWidget->setLayout(playButtonLayout);

	newGameLayout = new QVBoxLayout(this);
	newGameLayout->addWidget(lengthChoiceWidget);
	newGameLayout->addWidget(widthChoiceWidget);
	newGameLayout->addWidget(playButtonWidget);

	setLayout(newGameLayout);
}

void NewGameWidget::changeLength(){
	bool ok;
	qint32 value = QInputDialog::getInt(this, "CHANGE LENGTH", "Length:", 0, MIN_LENGTH, MAX_LENGTH, 2, &ok);
	if(ok){
		if(value % 2 == 0){
			lengthLabel->setText(QString::number(value));
			boardLength = value;
		}else{
			oddLengthMsgBox.exec();
			changeLength();
		}
	}
}

void NewGameWidget::changeWidth(){
	bool ok;
	qint32 value = QInputDialog::getInt(this, "CHANGE WIDTH", "Width:", 0, MIN_WIDTH, MAX_WIDTH, 2, &ok);
	if(ok){
		if(value % 2 == 0){
			widthLabel->setText(QString::number(value));
			boardWidth = value;
		}else{
			oddWidthMsgBox.exec();
			changeLength();
		}
	}
}

void NewGameWidget::emitNewGame(){
	emit newGameCreated(boardLength, boardWidth, p1ControlledByAI->isChecked(), p2ControlledByAI->isChecked());
}
