#include "ai_chat_widget.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUuid>

#include <rfl_tree.h>

#include "application.h"

const char* system_message = R"(You are a helpful assistant for analyzing a JSON file that stores a family tree.

Goals:
- Perform analysis of the family tree and answer questions about it when asked.
- Perform modifications to the JSON content when requested.

Context handling:
- The JSON file specification and file content will always be provided dynamically between FILE_BEGIN and FILE_END statements.
- Treat everything between FILE_BEGIN and FILE_END as authoritative input.
- Use the attached specification to interpret the JSON structure and semantics.

Rules:
- For analysis requests, respond in markdown format.
- For modification requests, return the modified JSON content between FILE_BEGIN and FILE_END statements.
- You may include explanatory plain text before or after the FILE_BEGIN / FILE_END block.
- Preserve valid JSON syntax at all times.
- Preserve existing data unless the user explicitly requests a change.
- Do not invent missing facts, relationships, dates, or attributes. If the information is missing or ambiguous, state that clearly.
- Base all answers and modifications only on the provided JSON content, attached specification, and the user's request.

Required modification output format:
Some text ....
FILE_BEGIN
<updated JSON content here>
FILE_END
Some other text ...

Do not:
- Produce defamatory or insulting statements.
- Add comments, commentary, metadata, or any other extra information inside the JSON unless explicitly requested and valid according to the specification.
- Treat text outside FILE_BEGIN and FILE_END as part of the JSON content.

JSON file specification:
FILE_BEGIN
<family_tree_json_specification>
FILE_END

JSON file content:
FILE_BEGIN
<family_tree_json_content>
FILE_END)";

AiChatWidget::AiChatWidget(RAgentSettingsManager *aiAgentSettingsManager, const RApplicationSettings *applicationSettings, QWidget *parent)
    : RAiChatWidget{aiAgentSettingsManager,
                    applicationSettings,
                    Application::instance()->getSession()->getLastAiChatAgent(),
                    Application::instance()->getSession()->getLastAiChatLanguage(),
                    parent}
{
    this->systemPrompt = QString(system_message);
    this->systemPrompt.replace("<family_tree_json_specification>", FTree::getJsonFileSpecs());
    QJsonObject treeJson = Application::instance()->getSession()->getTree()->toJson();
    QJsonArray persons = treeJson[QStringLiteral("persons")].toArray();
    for (int i = 0; i < persons.size(); ++i)
    {
        QJsonObject person = persons[i].toObject();
        person.remove(QStringLiteral("picture"));
        person.remove(QStringLiteral("text"));
        persons[i] = person;
    }
    treeJson[QStringLiteral("persons")] = persons;
    this->systemPrompt.replace("<family_tree_json_content>", QJsonDocument(treeJson).toJson(QJsonDocument::Compact));

    QObject::connect(this, &RAiChatWidget::aiAgentNameChanged, this, [](const QString &name) {
        Application::instance()->getSession()->setLastAiChatAgent(name);
    });
    QObject::connect(this, &RAiChatWidget::languageNameChanged, this, [](const QString &name) {
        Application::instance()->getSession()->setLastAiChatLanguage(name);
    });

    this->applyCurrentAgent();
}

QString AiChatWidget::buildPrompt(const QString &language) const
{
    QString expandedPrompt(this->systemPrompt);
    QUuid activePersonId(Application::instance()->getSession()->getActivePersonId());
    if (!activePersonId.isNull())
    {
        expandedPrompt += QStringLiteral("\n\nMy person ID in the family tree is: ") + activePersonId.toString(QUuid::WithoutBraces);
    }
    else
    {
        const QString userFullName = Application::instance()->getApplicationSettings()->getUserFullName();
        if (!userFullName.isEmpty())
        {
            expandedPrompt += QStringLiteral("\n\nMy name is: ") + userFullName;
        }
    }
    expandedPrompt += QStringLiteral("\n\nCurent date is: ") + QDate::currentDate().toString();
    return expandedPrompt + QStringLiteral("\n\nProduce response in language: ") + language;
}
