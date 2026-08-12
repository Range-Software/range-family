#include <QCloseEvent>
#include <QVBoxLayout>

#include "cloud_ai_query_dialog.h"
#include "cloud_ai_query_widget.h"

CloudAiQueryDialog::CloudAiQueryDialog(RCloudConnectionHandler *connectionHandler,
                                       RApplicationSettings *applicationSettings,
                                       QWidget *parent)
    : QDialog{parent, Qt::Window | Qt::WindowStaysOnTopHint}
{
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setWindowTitle(tr("Cloud AI query"));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    this->cloudQueryWidget = new CloudAiQueryWidget(connectionHandler,applicationSettings);
    mainLayout->addWidget(this->cloudQueryWidget);
}

void CloudAiQueryDialog::closeEvent(QCloseEvent *event)
{
    // Answer to a query which is still being processed would never be displayed.
    this->cloudQueryWidget->cancelQuery();
    QDialog::closeEvent(event);
}
