#ifndef EDITORMENUWIDGET_H
#define EDITORMENUWIDGET_H

#include <QtWidgets>
#include "../GameState/gamestate.h"

class EditorMenuWidget : public QWidget
{
	Q_OBJECT

public:
	EditorMenuWidget(QWidget *parent = nullptr);
	void setGameState(GameState *g);
	void start();

signals:
	void finishEdit();
	void placeBallMode();
	void editLineMode();
	void changePlayer();

public slots:

private:
	QLabel *infoLabel;
	QPushButton *infoButton;
	QPushButton *finishButton;
	QPushButton *placeBallButton;
	QPushButton *editLineButton;
	QPushButton *playerChangeButton;
	QCheckBox *p1AICheckBox;
	QCheckBox *p2AICheckBox;
	GameState *gameState;

	QWidget *checkboxWidget;
	QHBoxLayout *checkboxesLayout;
	QVBoxLayout *editorMenuLayout;

	void updatePlayer();
};

#endif // EDITORMENUWIDGET_H
