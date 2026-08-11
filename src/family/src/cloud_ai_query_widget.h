#ifndef CLOUD_AI_QUERY_WIDGET_H
#define CLOUD_AI_QUERY_WIDGET_H

#include <rgl_cloud_ai_query_widget.h>

class CloudAiQueryWidget : public RCloudAiQueryWidget
{
    Q_OBJECT

    public:

        //! Constructor.
        explicit CloudAiQueryWidget(RCloudConnectionHandler *connectionHandler,
                                    RApplicationSettings *applicationSettings,
                                    QWidget *parent = nullptr);

};

#endif // CLOUD_AI_QUERY_WIDGET_H
