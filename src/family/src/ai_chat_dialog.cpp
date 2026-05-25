#include "ai_chat_dialog.h"
#include "ai_chat_widget.h"

#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>

AiChatDialog::AiChatDialog(RAgentSettingsManager *aiAgentSettingsManager, const RApplicationSettings *applicationSettings, QWidget *parent)
    : QDialog{parent, Qt::Window | Qt::WindowStaysOnTopHint}
{
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setWindowTitle(tr("AI chat"));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    AiChatWidget *aiChatWidget = new AiChatWidget(aiAgentSettingsManager,applicationSettings);
    mainLayout->addWidget(aiChatWidget);
}
