#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent), gameState(nullptr)
{
	newGameWidget = new NewGameWidget(this);
	connect(newGameWidget, SIGNAL(newGameCreated(qint32, qint32, bool, bool)),
			this, SLOT(newGameCreated(qint32, qint32, bool, bool)));

	currentGameWidget = new CurrentGameWidget(this);
	blankWidget = new QWidget(this);
	stackedWidget = new QStackedWidget(this);
	stackedWidget->addWidget(newGameWidget);
	stackedWidget->addWidget(currentGameWidget);
	stackedWidget->addWidget(blankWidget);
	stackedWidget->setCurrentWidget(blankWidget);
	setCentralWidget(stackedWidget);

	unsavedWarningBox.setWindowTitle("Warning");
	unsavedWarningBox.setText("All unsaved progress will be lost. Do you want to continue?");
	unsavedWarningBox.setStandardButtons(QMessageBox::Yes|QMessageBox::No);

	quitBox.setWindowTitle("You don't like me? :<");
	quitBox.setText("Are you sure you want to leave?");
	quitBox.setStandardButtons(QMessageBox::Yes|QMessageBox::No);

	nothingToSaveBox.setWindowTitle("Nothing to save");
	nothingToSaveBox.setText("There's nothing to save.");

	createMenus();
}

void MainWindow::createMenus(){
	gameMenu = menuBar()->addMenu("&Game");

	QAction *newGameAct = new QAction("&New game", this);
	newGameAct->setShortcut(QKeySequence("ctrl+N"));
	connect(newGameAct, SIGNAL(triggered()), this, SLOT(newGame()));
	gameMenu->addAction(newGameAct);

	QAction *loadGameAct = new QAction("&Load game", this);
	loadGameAct->setShortcut(QKeySequence("ctrl+L"));
	connect(loadGameAct, SIGNAL(triggered()), this, SLOT(loadGame()));
	gameMenu->addAction(loadGameAct);

	QAction *saveGameAct = new QAction("&Save game", this);
	saveGameAct->setShortcut(QKeySequence("ctrl+S"));
	connect(saveGameAct, SIGNAL(triggered()), this, SLOT(saveGame()));
	gameMenu->addAction(saveGameAct);

	QAction *editGameAct = new QAction("&Edit game", this);
	editGameAct->setShortcut(QKeySequence("ctrl+E"));
	connect(editGameAct, SIGNAL(triggered()), this, SLOT(editGame()));
	gameMenu->addAction(editGameAct);

	QAction *quit = new QAction("&Quit", this);
	quit->setShortcut(QKeySequence("ctrl+Q"));
	connect(quit, SIGNAL(triggered()), this, SLOT(quit()));
	gameMenu->addAction(quit);

}

MainWindow::~MainWindow()
{

}

void MainWindow::newGame(){
	if(stackedWidget->currentWidget() == blankWidget){
		stackedWidget->setCurrentWidget(newGameWidget);
	}else if(unsavedWarningBox.exec() == QMessageBox::Yes){
		stackedWidget->setCurrentWidget(newGameWidget);
	}
}

void MainWindow::loadGame(){
	if(stackedWidget->currentWidget() == blankWidget || unsavedWarningBox.exec() == QMessageBox::Yes){
		QString filter = "All filers (*)";
		QString fileName = QFileDialog::getOpenFileName(nullptr, "Save the game", ".", filter);
		if(fileName.isEmpty()){
			return;
		}else{
			QFile file(fileName);
			if (!file.open(QIODevice::ReadOnly)) {
				QMessageBox::information(nullptr, "Unable to open file", file.errorString());
				return;
			}
			QDataStream *in = new QDataStream(&file);
			qint32 boardLength;
			qint32 boardWidth;
			bool p1ControlledByAI;
			bool p2ControlledByAI;
			*in >> boardLength >> boardWidth >> p1ControlledByAI >> p2ControlledByAI;
			delete gameState;
			gameState = new GameState(this, boardLength, boardWidth, p1ControlledByAI, p2ControlledByAI);
			gameState->loadGame(in);
			delete in;
			currentGameWidget->setGameState(gameState);
			stackedWidget->setCurrentWidget(currentGameWidget);
		}
	}
}

void MainWindow::saveGame(){
	if(stackedWidget->currentWidget() == blankWidget || stackedWidget->currentWidget() == newGameWidget){
		nothingToSaveBox.setWindowTitle("Nothing to save");
		nothingToSaveBox.setText("Nothing to save.");
		nothingToSaveBox.exec();
	}else if(currentGameWidget->canSave()){
		gameState->saveGame();
	}
}

void MainWindow::editGame(){
	if(stackedWidget->currentWidget() == blankWidget || stackedWidget->currentWidget() == newGameWidget){
		nothingToSaveBox.setWindowTitle("Nothing to edit");
		nothingToSaveBox.setText("Nothing to edit.");
		nothingToSaveBox.exec();
	}else if(currentGameWidget->canEdit() && unsavedWarningBox.exec() == QMessageBox::Yes){
		currentGameWidget->startEdit();
	}
}

void MainWindow::quit(){
	if(quitBox.exec() == QMessageBox::Yes){
		close();
		QApplication::quit();
	}
}

void MainWindow::newGameCreated(qint32 boardLength, qint32 boardWidth, bool p1ControlledByAI, bool p2ControlledByAI){
	delete gameState;
	gameState = new GameState(this, boardLength, boardWidth, p1ControlledByAI, p2ControlledByAI);
	currentGameWidget->setGameState(gameState);
	stackedWidget->setCurrentWidget(currentGameWidget);
}
