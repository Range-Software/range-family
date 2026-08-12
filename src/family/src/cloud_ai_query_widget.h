#ifndef CLOUD_AI_QUERY_WIDGET_H
#define CLOUD_AI_QUERY_WIDGET_H

#include <rgl_cloud_ai_query_widget.h>

class CloudAiQueryWidget : public RCloudAiQueryWidget
{
    Q_OBJECT

    protected:

        //! Family tree file the conversation history belongs to.
        QString historyTreeFileName;

    public:

        //! Constructor.
        explicit CloudAiQueryWidget(RCloudConnectionHandler *connectionHandler,
                                    RApplicationSettings *applicationSettings,
                                    QWidget *parent = nullptr);

    protected:

        //! Reference the family tree file stored in Cloud which corresponds to
        //! the currently loaded family tree file.
        void buildQuery(RAIQuery &aiQuery) override;

        //! Append emphasized note to the query history.
        void appendNote(const QString &note);

    protected slots:

        //! Forget the conversation history if it belongs to a different family tree.
        void resetHistoryIfTreeChanged();

};

#endif // CLOUD_AI_QUERY_WIDGET_H
