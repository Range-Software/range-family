#ifndef AI_CHAT_DIALOG_H
#define AI_CHAT_DIALOG_H

#include <rai_agent_settings_manager.h>
#include <rgl_application_settings.h>

#include <QDialog>

class AiChatDialog : public QDialog
{
    Q_OBJECT

    public:

        //! Constructor.
        explicit AiChatDialog(RAgentSettingsManager *aiAgentSettingsManager,
                              const RApplicationSettings *applicationSettings,
                              QWidget *parent = nullptr);

};

#endif // AI_CHAT_DIALOG_H
