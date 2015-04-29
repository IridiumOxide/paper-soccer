#include "calculatingmovewidget.h"

CalculatingMoveWidget::CalculatingMoveWidget(QWidget *parent) :
	QWidget(parent)
{
	infoLabel = new QLabel("Calculating the AI move.\nPlease wait or press the button to cancel.");
	stopAIButton = new QPushButton("Stop the AI (Alt + &Z)", this);
	connect(stopAIButton, &QPushButton::clicked, [this](){emit stopAI();});

	calculatingMoveWidgetLayout = new QVBoxLayout(this);
	calculatingMoveWidgetLayout->addWidget(infoLabel);
	calculatingMoveWidgetLayout->addWidget(stopAIButton);
	setLayout(calculatingMoveWidgetLayout);
}
