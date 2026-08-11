#include <QVBoxLayout>
#include <rgl_cloud_ai_query_widget.h>

#include "cloud_ai_query_dialog.h"

CloudAiQueryDialog::CloudAiQueryDialog(RCloudConnectionHandler *connectionHandler,
                                       RApplicationSettings *applicationSettings,
                                       QWidget *parent)
    : QDialog{parent}
{
    this->setWindowTitle(tr("Cloud AI query"));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    RCloudAiQueryWidget *clouddQueryWidget = new RCloudAiQueryWidget(connectionHandler,applicationSettings);
    mainLayout->addWidget(clouddQueryWidget);
}
