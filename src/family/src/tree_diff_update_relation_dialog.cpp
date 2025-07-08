#include <QVBoxLayout>
#include <QSplitter>
#include <QLabel>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QJsonDocument>

#include "tree_diff_update_relation_dialog.h"

TreeDiffUpdateRelationDialog::TreeDiffUpdateRelationDialog(const FRelation &aRelation, const FRelation &bRelation, QWidget *parent)
    : QDialog{parent}
    , aRelation{aRelation}
    , bRelation{bRelation}
    , uRelation{bRelation}
{
    QIcon cancelIcon(":/icons/action/pixmaps/range-cancel.svg");
    QIcon updateIcon(":/icons/action/pixmaps/range-edit.svg");

    this->setWindowTitle(tr("Update relation"));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    QSplitter *splitter = new QSplitter(Qt::Horizontal);
    mainLayout->addWidget(splitter);

    splitter->addWidget(TreeDiffUpdateRelationDialog::createTextBrowser("A",this->aRelation));
    splitter->addWidget(TreeDiffUpdateRelationDialog::createTextBrowser("B",this->bRelation));
    splitter->addWidget(TreeDiffUpdateRelationDialog::createTextBrowser("Updated",this->uRelation,&this->uTextBrowser));

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

    QObject::connect(onlyMissingCheck,&QCheckBox::clicked,this,&TreeDiffUpdateRelationDialog::onOnlyMissingChecked);

    QObject::connect(this->uTextBrowser,&RTextBrowser::textChanged,this,&TreeDiffUpdateRelationDialog::onUTextChanged);

    this->updateURelation(onlyMissingCheck->isChecked());
}

const FRelation &TreeDiffUpdateRelationDialog::getUpdatedRelation() const
{
    return this->uRelation;
}

QWidget *TreeDiffUpdateRelationDialog::createTextBrowser(QString title, const FRelation &relation, RTextBrowser **uTextBrowser)
{
    QWidget *widget = new QWidget;

    QVBoxLayout *vBoxLayout = new QVBoxLayout;
    widget->setLayout(vBoxLayout);

    QLabel *label = new QLabel(title);
    vBoxLayout->addWidget(label);

    RTextBrowser *textBrowser = new RTextBrowser(false);
    textBrowser->setPlainText(QJsonDocument(relation.toJson()).toJson());
    vBoxLayout->addWidget(textBrowser);

    if (uTextBrowser != nullptr)
    {
        *uTextBrowser = textBrowser;
    }

    return widget;
}

void TreeDiffUpdateRelationDialog::updateURelation(bool onlyMissing)
{
    FRelation uRelationOld(this->uRelation);
    this->uRelation = this->bRelation;

    if (onlyMissing)
    {
        this->uRelation.merge(this->aRelation);
    }
    else
    {
        this->uRelation.update(this->aRelation);
    }
    if (uRelationOld != this->uRelation)
    {
        this->uTextBrowser->setPlainText(QJsonDocument(this->uRelation.toJson()).toJson());
    }
}

void TreeDiffUpdateRelationDialog::onOnlyMissingChecked(bool clicked)
{
    this->updateURelation(clicked);
}

void TreeDiffUpdateRelationDialog::onUTextChanged()
{
    this->updateButton->setEnabled(this->bRelation != this->uRelation);
}
