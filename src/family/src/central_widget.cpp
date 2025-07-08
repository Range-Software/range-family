#include <QApplication>
#include <QVBoxLayout>
#include <QSplitter>
#include <QScrollBar>

#include <QTabWidget>

#include <rgl_logger_handler.h>
#include <rgl_message_box.h>

#include "central_widget.h"

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

    QColor tabTextColor = QApplication::palette().text().color();
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

void CentralWidget::onInfoPrinted(const QString &message)
{
    QScrollBar *sb = this->applicationOutputBrowser->verticalScrollBar();
    int scrollBarValue = sb->value();
    bool scrollBarAtMax = ((sb->maximum() - scrollBarValue) <= 10);

    this->applicationOutputBrowser->moveCursor(QTextCursor::End);
    this->applicationOutputBrowser->setTextBackgroundColor(QApplication::palette().base().color());
    this->applicationOutputBrowser->setTextColor(QApplication::palette().text().color());
    this->applicationOutputBrowser->insertPlainText(message);
    this->applicationOutputBrowser->moveCursor(QTextCursor::End);
    if (scrollBarAtMax)
    {
        scrollBarValue = sb->maximum();
    }
    sb->setValue(scrollBarValue);
    this->setTabTitle(CentralWidget::Output,RMessage::Type::Info);
}

void CentralWidget::onNoticePrinted(const QString &message)
{
    RMessageBox::information(this,tr("Notice"),QString(message).replace("NOTICE: ",""));
}

void CentralWidget::onWarningPrinted(const QString &message)
{
    QTextCharFormat charFormat = this->applicationOutputBrowser->currentCharFormat();

    this->applicationOutputBrowser->moveCursor(QTextCursor::End);
    this->applicationOutputBrowser->setTextBackgroundColor(QApplication::palette().base().color());
    this->applicationOutputBrowser->setTextColor(QColor(170,0,0));
    this->applicationOutputBrowser->insertPlainText(message);
    this->applicationOutputBrowser->moveCursor(QTextCursor::End);
    this->applicationOutputBrowser->setTextColor(QApplication::palette().text().color());
    this->applicationOutputBrowser->setCurrentCharFormat(charFormat);
    QScrollBar *sb = this->applicationOutputBrowser->verticalScrollBar();
    sb->setValue(sb->maximum());
    this->setTabTitle(CentralWidget::Output,RMessage::Type::Warning);
}

void CentralWidget::onErrorPrinted(const QString &message)
{
    QTextCharFormat charFormat = this->applicationOutputBrowser->currentCharFormat();

    this->applicationOutputBrowser->moveCursor(QTextCursor::End);
    this->applicationOutputBrowser->setTextBackgroundColor(QColor(170,0,0));
    this->applicationOutputBrowser->setTextColor(QColor(255,255,255));
    this->applicationOutputBrowser->insertPlainText(message);
    this->applicationOutputBrowser->moveCursor(QTextCursor::End);
    this->applicationOutputBrowser->setTextBackgroundColor(QApplication::palette().base().color());
    this->applicationOutputBrowser->setTextColor(QApplication::palette().text().color());
    this->applicationOutputBrowser->setCurrentCharFormat(charFormat);
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
