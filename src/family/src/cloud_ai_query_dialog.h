#ifndef RGL_CLOUD_AI_QUERY_DIALOG_H
#define RGL_CLOUD_AI_QUERY_DIALOG_H

#include <QDialog>
#include <QWidget>

#include <rgl_application_settings.h>
#include <rgl_cloud_connection_handler.h>

class CloudAiQueryWidget;

class CloudAiQueryDialog : public QDialog
{

    Q_OBJECT

    protected:

        //! Cloud AI query widget.
        CloudAiQueryWidget *cloudQueryWidget;

    public:

        //! Constructor.
        explicit CloudAiQueryDialog(RCloudConnectionHandler *connectionHandler,
                                    RApplicationSettings *applicationSettings,
                                    QWidget *parent = nullptr);

    protected:

        //! Cancel the query which is still being processed.
        void closeEvent(QCloseEvent *event) override;

};

#endif // RGL_CLOUD_AI_QUERY_DIALOG_H
