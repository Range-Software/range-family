#include <QVBoxLayout>
#include <QSplitter>
#include <QLabel>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QJsonDocument>

#include "tree_diff_update_person_dialog.h"

TreeDiffUpdatePersonDialog::TreeDiffUpdatePersonDialog(const FPerson &aPerson, const FPerson &bPerson, QWidget *parent)
    : QDialog{parent}
    , aPerson{aPerson}
    , bPerson{bPerson}
    , uPerson{bPerson}
{
    QIcon cancelIcon(":/icons/action/pixmaps/range-cancel.svg");
    QIcon updateIcon(":/icons/action/pixmaps/range-edit.svg");

    this->setWindowTitle(tr("Update person"));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    QSplitter *splitter = new QSplitter(Qt::Horizontal);
    mainLayout->addWidget(splitter);

    splitter->addWidget(TreeDiffUpdatePersonDialog::createTextBrowser("A",this->aPerson));
    splitter->addWidget(TreeDiffUpdatePersonDialog::createTextBrowser("B",this->bPerson));
    splitter->addWidget(TreeDiffUpdatePersonDialog::createTextBrowser("Updated",this->uPerson,&this->uTextBrowser));

    QCheckBox *onlyMissingCheck = new QCheckBox(tr("Only missing values"));
    onlyMissingCheck->setCheckState(Qt::Unchecked);
    mainLayout->addWidget(onlyMissingCheck);

    QDialogButtonBox *buttonBox = new QDialogButtonBox;
    mainLayout->addWidget(buttonBox);

    QPushButton *cancelButton = new QPushButton(cancelIcon, tr("Cancel"));
    buttonBox->addButton(cancelButton,QDialogButtonBox::RejectRole);

    this->updateButton = new QPushButton(updateIcon, tr("Update"));
    this->updateButton->setDisabled(true);
    buttonBox->addButton(this->updateButton,QDialogButtonBox::AcceptRole);

    QObject::connect(buttonBox,&QDialogButtonBox::rejected,this,&QDialog::reject);
    QObject::connect(buttonBox,&QDialogButtonBox::accepted,this,&QDialog::accept);

    QObject::connect(onlyMissingCheck,&QCheckBox::clicked,this,&TreeDiffUpdatePersonDialog::onOnlyMissingChecked);

    QObject::connect(this->uTextBrowser,&RTextBrowser::textChanged,this,&TreeDiffUpdatePersonDialog::onUTextChanged);

    this->updateUPerson(onlyMissingCheck->isChecked());
}

const FPerson &TreeDiffUpdatePersonDialog::getUpdatedPerson() const
{
    return this->uPerson;
}

QWidget *TreeDiffUpdatePersonDialog::createTextBrowser(QString title, const FPerson &person, RTextBrowser **uTextBrowser)
{
    QWidget *widget = new QWidget;

    QVBoxLayout *vBoxLayout = new QVBoxLayout;
    widget->setLayout(vBoxLayout);

    QLabel *label = new QLabel(title);
    vBoxLayout->addWidget(label);

    RTextBrowser *textBrowser = new RTextBrowser(false);
    textBrowser->setPlainText(QJsonDocument(person.toJson()).toJson());
    vBoxLayout->addWidget(textBrowser);

    if (uTextBrowser != nullptr)
    {
        *uTextBrowser = textBrowser;
    }

    return widget;
}

void TreeDiffUpdatePersonDialog::updateUPerson(bool onlyMissing)
{
    FPerson uPersonOld(this->uPerson);
    this->uPerson = this->bPerson;

    if (onlyMissing)
    {
        this->uPerson.merge(this->aPerson);
    }
    else
    {
        this->uPerson.update(this->aPerson);
    }
    if (uPersonOld != this->uPerson)
    {
        this->uTextBrowser->setPlainText(QJsonDocument(this->uPerson.toJson()).toJson());
    }
}

void TreeDiffUpdatePersonDialog::onOnlyMissingChecked(bool clicked)
{
    this->updateUPerson(clicked);
}

void TreeDiffUpdatePersonDialog::onUTextChanged()
{
    this->updateButton->setEnabled(this->bPerson != this->uPerson);
}
