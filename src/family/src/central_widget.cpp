#include <QApplication>
#include <QVBoxLayout>
#include <QSplitter>
#include <QScrollBar>

#include <QTabWidget>

#include <rgl_logger_handler.h>
#include <rgl_message_box.h>

#include "central_widget.h"

//! Append text at the end of the browser. Colors are given only to highlight
//! text, otherwise the text follows the palette, also after the color scheme
//! changes.
static void appendText(QTextEdit *browser, const QString &text, const QColor &foreground = QColor(), const QColor &background = QColor())
{
    QTextCharFormat charFormat;
    if (foreground.isValid())
    {
        charFormat.setForeground(foreground);
    }
    if (background.isValid())
    {
        charFormat.setBackground(background);
    }
    QTextCursor cursor(browser->document());
    cursor.movePosition(QTextCursor::End);
    cursor.insertText(text,charFormat);
    browser->moveCursor(QTextCursor::End);
}

CentralWidget::CentralWidget(Session *session, QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    this->tabWidget = new QTabWidget;
    this->tabWidget->setTabPosition(QTabWidget::South);
    mainLayout->addWidget(this->tabWidget);

    QSplitter *verticalSplitter = new QSplitter(Qt::Vertical);
    verticalSplitter->setChildrenCollapsible(false);
    this->editorTabPosition = this->tabWidget->addTab(verticalSplitter,tr("Editor"));

    this->editor = new EditorWidget(session);
    verticalSplitter->addWidget(this->editor);

    this->applicationOutputBrowser = new RTextBrowser(true);
    this->outputTabPosition = this->tabWidget->addTab(this->applicationOutputBrowser,tr("Output"));

    this->setTabTitle(CentralWidget::Editor);
    this->setTabTitle(CentralWidget::Output);

    QObject::connect(this->tabWidget,&QTabWidget::currentChanged,this,&CentralWidget::onCurrentChanged);

    // Logger signals
    QObject::connect(&RLoggerHandler::getInstance(),&RLoggerHandler::infoPrinted,this,&CentralWidget::onInfoPrinted);
    QObject::connect(&RLoggerHandler::getInstance(),&RLoggerHandler::noticePrinted,this,&CentralWidget::onNoticePrinted);
    QObject::connect(&RLoggerHandler::getInstance(),&RLoggerHandler::warningPrinted,this,&CentralWidget::onWarningPrinted);
    QObject::connect(&RLoggerHandler::getInstance(),&RLoggerHandler::errorPrinted,this,&CentralWidget::onErrorPrinted);
}

EditorWidget *CentralWidget::getEditor() const
{
    return this->editor;
}

void CentralWidget::setTabTitle(TabType tabType, RMessage::Type messageType, const QString &additionalText)
{
    QString defaultText;
    int tabPosition = 0;


    if (tabType == CentralWidget::Editor)
    {
        defaultText = tr("Editor");
        tabPosition = this->editorTabPosition;
    }
    else if (tabType == CentralWidget::Output)
    {
        defaultText = tr("Output");
        tabPosition = this->outputTabPosition;
    }
    else
    {
        return;
    }

    if (!additionalText.isEmpty())
    {
        defaultText += " (" + additionalText + ")";
    }

    // An invalid color lets the tab follow the palette, also after the color
    // scheme changes. The palette color at this moment would stay.
    QColor tabTextColor;
    if (messageType == RMessage::Type::Info)
    {
        defaultText.prepend("* ");
        tabTextColor = Qt::darkCyan;
    }
    else if (messageType == RMessage::Type::Warning)
    {
        defaultText.prepend("! ");
        tabTextColor = Qt::red;
    }
    else if (messageType == RMessage::Type::Error)
    {
        defaultText.prepend("!!! ");
        defaultText.append(" !!!");
        tabTextColor = Qt::red;
    }

    this->tabWidget->tabBar()->setTabTextColor(tabPosition,tabTextColor);
    this->tabWidget->setTabText(tabPosition,defaultText);
}

void CentralWidget::onInfoPrinted(const RMessage &message)
{
    QScrollBar *sb = this->applicationOutputBrowser->verticalScrollBar();
    int scrollBarValue = sb->value();
    bool scrollBarAtMax = ((sb->maximum() - scrollBarValue) <= 10);

    appendText(this->applicationOutputBrowser,RMessage::messageToLogString(message));
    if (scrollBarAtMax)
    {
        scrollBarValue = sb->maximum();
    }
    sb->setValue(scrollBarValue);
    this->setTabTitle(CentralWidget::Output,RMessage::Type::Info);
}

void CentralWidget::onNoticePrinted(const RMessage &message)
{
    RMessageBox::information(this,tr("Notice"),QString(RMessage::messageToLogString(message)).replace("NOTICE: ",""));
}

void CentralWidget::onWarningPrinted(const RMessage &message)
{
    appendText(this->applicationOutputBrowser,RMessage::messageToLogString(message),QColor(170,0,0));
    QScrollBar *sb = this->applicationOutputBrowser->verticalScrollBar();
    sb->setValue(sb->maximum());
    this->setTabTitle(CentralWidget::Output,RMessage::Type::Warning);
}

void CentralWidget::onErrorPrinted(const RMessage &message)
{
    appendText(this->applicationOutputBrowser,RMessage::messageToLogString(message),QColor(255,255,255),QColor(170,0,0));
    QScrollBar *sb = this->applicationOutputBrowser->verticalScrollBar();
    sb->setValue(sb->maximum());
    this->setTabTitle(CentralWidget::Output,RMessage::Type::Error);
}

void CentralWidget::onCurrentChanged(int tabPosition)
{
    if (tabPosition == this->outputTabPosition)
    {
        this->setTabTitle(CentralWidget::Output);
    }
}
