#include <QVBoxLayout>
#include <QGroupBox>
#include <QApplication>
#include <QComboBox>

#include "person_edit_widget.h"
#include "name_edit_widget.h"
#include "event_edit_widget.h"
#include "picture_edit_widget.h"

#define _FP_TR(qstr) qApp->translate("FPerson",qstr.toUtf8().constData())

PersonEditWidget::PersonEditWidget(const FPerson &person, QWidget *parent)
    : QWidget(parent)
    , person(person)
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    QGroupBox *nameGroupBox = new QGroupBox(_FP_TR(FPerson::ElementDesc::name));
    mainLayout->addWidget(nameGroupBox);

    QVBoxLayout *nameLayout = new QVBoxLayout;
    // nameLayout->setSpacing(0);
    nameGroupBox->setLayout(nameLayout);

    NameEditWidget *nameEdit = new NameEditWidget(this->person.getName());
    nameLayout->addWidget(nameEdit);

    QGroupBox *sexGroupBox = new QGroupBox(_FP_TR(FPerson::ElementDesc::sex));
    mainLayout->addWidget(sexGroupBox);

    QVBoxLayout *sexLayout = new QVBoxLayout;
    sexGroupBox->setLayout(sexLayout);

    QComboBox *sexCombo = new QComboBox;
    for (int sexIndex=0;sexIndex<FPerson::SexIndex::nSexes;sexIndex++)
    {
        sexCombo->addItem(_FP_TR(FPerson::sexIndexToDesc(sexIndex)));
        if (person.getSex().toLower() == FPerson::sexIndexToKey(sexIndex))
        {
            sexCombo->setCurrentIndex(sexIndex);
        }
    }
    sexLayout->addWidget(sexCombo);

    QGroupBox *birthGroupBox = new QGroupBox(_FP_TR(FPerson::ElementDesc::birth));
    mainLayout->addWidget(birthGroupBox);

    QVBoxLayout *birthLayout = new QVBoxLayout;
    birthGroupBox->setLayout(birthLayout);

    EventEditWidget *birthEdit = new EventEditWidget(this->person.getBirth());
    birthLayout->addWidget(birthEdit);

    QGroupBox *deathGroupBox = new QGroupBox(_FP_TR(FPerson::ElementDesc::death));
    mainLayout->addWidget(deathGroupBox);

    QVBoxLayout *deathLayout = new QVBoxLayout;
    deathGroupBox->setLayout(deathLayout);

    EventEditWidget *deathEdit = new EventEditWidget(this->person.getDeath());
    deathLayout->addWidget(deathEdit);

    QHBoxLayout *infoBoxLayout = new QHBoxLayout;
    infoBoxLayout->setContentsMargins(0,0,0,0);
    mainLayout->addLayout(infoBoxLayout);

    QGroupBox *pictureGroupBox = new QGroupBox(_FP_TR(FPerson::ElementDesc::picture));
    infoBoxLayout->addWidget(pictureGroupBox);

    QVBoxLayout *pictureLayout = new QVBoxLayout;
    pictureGroupBox->setLayout(pictureLayout);

    PictureEditWidget *pictureEdit = new PictureEditWidget(this->person.getPicture());
    pictureLayout->addWidget(pictureEdit);

    QGroupBox *textGroupBox = new QGroupBox(_FP_TR(FPerson::ElementDesc::text));
    infoBoxLayout->addWidget(textGroupBox);

    QVBoxLayout *textLayout = new QVBoxLayout;
    textGroupBox->setLayout(textLayout);

    this->textEdit = new QPlainTextEdit;
    this->textEdit->setPlainText(this->person.getText());
    textLayout->addWidget(this->textEdit);

    QObject::connect(sexCombo,&QComboBox::currentIndexChanged,this,&PersonEditWidget::onSexIndexChanged);
    QObject::connect(nameEdit,&NameEditWidget::changed,this,&PersonEditWidget::onNameChanged);
    QObject::connect(birthEdit,&EventEditWidget::changed,this,&PersonEditWidget::onBirthEventChanged);
    QObject::connect(deathEdit,&EventEditWidget::changed,this,&PersonEditWidget::onDeathEventChanged);
    QObject::connect(pictureEdit,&PictureEditWidget::changed,this,&PersonEditWidget::onPictureChanged);
    QObject::connect(this->textEdit,&QPlainTextEdit::textChanged,this,&PersonEditWidget::onTextEditChanged);
}

const FPerson &PersonEditWidget::getPerson() const
{
    return this->person;
}

void PersonEditWidget::onSexIndexChanged(int sexIndex)
{
    this->person.setSex(FPerson::sexIndexToKey(sexIndex));
    emit this->changed(this->person);
}

void PersonEditWidget::onNameChanged(const FName &name)
{
    this->person.setName(name);
    emit this->changed(this->person);
}

void PersonEditWidget::onBirthEventChanged(const FEvent &event)
{
    this->person.setBirth(event);
    emit this->changed(this->person);
}

void PersonEditWidget::onDeathEventChanged(const FEvent &event)
{
    this->person.setDeath(event);
    emit this->changed(this->person);
}

void PersonEditWidget::onPictureChanged(const FPicture &picture)
{
    this->person.setPicture(picture);
    emit this->changed(this->person);
}

void PersonEditWidget::onTextEditChanged()
{
    this->person.setText(this->textEdit->toPlainText().toHtmlEscaped());
    emit this->changed(this->person);
}
