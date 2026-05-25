#ifndef AI_CHAT_WIDGET_H
#define AI_CHAT_WIDGET_H

#include <rgl_ai_chat_widget.h>

class AiChatWidget : public RAiChatWidget
{
    Q_OBJECT

    QString systemPrompt;

protected:

    QString buildPrompt(const QString &language) const override;

public:

    //! Constructor.
    explicit AiChatWidget(RAgentSettingsManager *aiAgentSettingsManager,
                          const RApplicationSettings *applicationSettings,
                          QWidget *parent = nullptr);

};

#endif // AI_CHAT_WIDGET_H
