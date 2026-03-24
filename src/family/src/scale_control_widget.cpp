#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

#include <rbl_utils.h>

#include "scale_control_widget.h"

ScaleControlWidget::ScaleControlWidget(QWidget *parent)
    : QWidget{parent}
    , savedScaleValue{1.0}
{
    QHBoxLayout *mainLayout = new QHBoxLayout;
    this->setLayout(mainLayout);

    QLabel *scaleLabel = new QLabel(tr("Zoom") + ":");
    mainLayout->addWidget(scaleLabel);

    QPushButton *downButton = new QPushButton("-");
    mainLayout->addWidget(downButton);

    this->scaleBox = new QDoubleSpinBox;
    this->scaleBox->setMinimum(0.0);
    this->scaleBox->setSuffix("x");
    this->scaleBox->setDecimals(3);
    this->scaleBox->setValue(this->savedScaleValue);
    // this->scaleBox->setSingleStep(0.3);
    this->scaleBox->setStepType(QDoubleSpinBox::AdaptiveDecimalStepType);
    mainLayout->addWidget(this->scaleBox);

    QPushButton *upButton = new QPushButton("+");
    mainLayout->addWidget(upButton);

    QPushButton *resetButton = new QPushButton(tr("Reset"));
    mainLayout->addWidget(resetButton);

    QObject::connect(downButton,&QPushButton::clicked,this,&ScaleControlWidget::onDownButtonClicked);
    QObject::connect(upButton,&QPushButton::clicked,this,&ScaleControlWidget::onUpButtonClicked);
    QObject::connect(resetButton,&QPushButton::clicked,this,&ScaleControlWidget::onResetButtonClicked);

    QObject::connect(this->scaleBox,&QDoubleSpinBox::valueChanged,this,&ScaleControlWidget::onScaleValueChanged);
}

void ScaleControlWidget::setScale(double newScale)
{
    bool signalsBlockedSaved = this->signalsBlocked();
    this->blockSignals(true);
    this->scaleBox->setValue(newScale);
    this->blockSignals(signalsBlockedSaved);
}

void ScaleControlWidget::onDownButtonClicked()
{
    this->scaleBox->stepDown();
}

void ScaleControlWidget::onUpButtonClicked()
{
    this->scaleBox->stepUp();
}

void ScaleControlWidget::onResetButtonClicked()
{
    this->scaleBox->setValue(1.0);
}

void ScaleControlWidget::onScaleValueChanged(double value)
{
    double factor = (std::fabs(this->savedScaleValue) < RConstants::eps ? 1.0 : value / this->savedScaleValue);
    this->savedScaleValue = value;
    emit this->factorChanged(factor);
}
