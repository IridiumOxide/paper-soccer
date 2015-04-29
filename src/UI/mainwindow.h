#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QtWidgets>
#include "newgamewidget.h"
#include "currentgamewidget.h"
#include "../GameState/gamestate.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();

private:
	QGridLayout *mainWindowLayout;
	NewGameWidget *newGameWidget;
	CurrentGameWidget *currentGameWidget;
	QWidget *blankWidget;
	QStackedWidget *stackedWidget;
	GameState *gameState;

	QMessageBox unsavedWarningBox;
	QMessageBox quitBox;
	QMessageBox nothingToSaveBox;

	void createMenus();
	QMenu *gameMenu;


public slots:
	void newGame();
	void loadGame();
	void saveGame();
	void editGame();
	void quit();
	void newGameCreated(qint32 boardLength, qint32 boardWidth, bool p1ControlledByAI, bool p2ControlledByAI);
};

#endif // MAINWINDOW_H
