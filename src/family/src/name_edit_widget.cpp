#include <QHBoxLayout>
#include <QApplication>
#include <QLineEdit>

#include "name_edit_widget.h"

#define _FN_TR(qstr) qApp->translate("FName",qstr.toUtf8().constData())

NameEditWidget::NameEditWidget(const FName &name, QWidget *parent)
    : QWidget(parent)
    , name(name)
{
    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->setContentsMargins(0,0,0,0);
    this->setLayout(mainLayout);

    QLineEdit *firstName = new QLineEdit(name.getFirstName());
    firstName->setPlaceholderText(_FN_TR(FName::ElementDesc::firstName));
    firstName->setClearButtonEnabled(true);
    mainLayout->addWidget(firstName);

    QLineEdit *middleName = new QLineEdit(name.getMiddleName());
    middleName->setPlaceholderText(_FN_TR(FName::ElementDesc::middleName));
    middleName->setClearButtonEnabled(true);
    mainLayout->addWidget(middleName);

    QLineEdit *lastName = new QLineEdit(name.getLastName());
    lastName->setPlaceholderText(_FN_TR(FName::ElementDesc::lastName));
    lastName->setClearButtonEnabled(true);
    mainLayout->addWidget(lastName);

    QLineEdit *maidenName = new QLineEdit(name.getMaidenName());
    maidenName->setPlaceholderText(_FN_TR(FName::ElementDesc::maidenName));
    maidenName->setClearButtonEnabled(true);
    mainLayout->addWidget(maidenName);

    QObject::connect(firstName,&QLineEdit::textChanged,this,&NameEditWidget::onFirstNameChanged);
    QObject::connect(middleName,&QLineEdit::textChanged,this,&NameEditWidget::onMiddleNameChanged);
    QObject::connect(lastName,&QLineEdit::textChanged,this,&NameEditWidget::onLastNameChanged);
    QObject::connect(maidenName,&QLineEdit::textChanged,this,&NameEditWidget::onMaidenNameChanged);
}

void NameEditWidget::onFirstNameChanged(const QString &text)
{
    this->name.setFirstName(text);
    emit this->changed(this->name);
}

void NameEditWidget::onMiddleNameChanged(const QString &text)
{
    this->name.setMiddleName(text);
    emit this->changed(this->name);
}

void NameEditWidget::onLastNameChanged(const QString &text)
{
    this->name.setLastName(text);
    emit this->changed(this->name);
}

void NameEditWidget::onMaidenNameChanged(const QString &text)
{
    this->name.setMaidenName(text);
    emit this->changed(this->name);
}
