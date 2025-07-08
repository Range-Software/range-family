#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>

#include <rgl_message_box.h>

#include "tree_diff_dialog.h"
#include "tree_diff_widget.h"

TreeDiffDialog::TreeDiffDialog(const QString &aFileName, const QString &bFileName, bool easyMergeFirst, QWidget *parent)
    : QDialog{parent}
    , bTreeSaved{true}
{
    QIcon closeIcon(":/icons/action/pixmaps/range-close.svg");

    this->setWindowTitle(tr("Family trees differences"));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout (mainLayout);

    TreeDiffWidget *treeDiffWidget = new TreeDiffWidget(aFileName,bFileName,easyMergeFirst);
    mainLayout->addWidget(treeDiffWidget);

    QDialogButtonBox *buttonBox = new QDialogButtonBox;
    mainLayout->addWidget(buttonBox);

    QPushButton *closeButton = new QPushButton(closeIcon, tr("Close"));
    buttonBox->addButton(closeButton,QDialogButtonBox::AcceptRole);

    QObject::connect(buttonBox,&QDialogButtonBox::accepted,this,&TreeDiffDialog::onAccepted);

    QObject::connect(treeDiffWidget,&TreeDiffWidget::bTreeChanged,this,&TreeDiffDialog::onBTreeChanged);
    QObject::connect(treeDiffWidget,&TreeDiffWidget::bTreeSaved,this,&TreeDiffDialog::onBTreeSaved);
}

void TreeDiffDialog::onAccepted()
{
    if (this->bTreeSaved ||
        RMessageBox::question(this,tr("Tree not saved"),tr("B tree was changed but not saved. Close anyway?")) == RMessageBox::Yes)
    {
        this->accept();
    }
}

void TreeDiffDialog::onBTreeChanged()
{
    this->bTreeSaved = false;
}

void TreeDiffDialog::onBTreeSaved()
{
    this->bTreeSaved = true;
}
