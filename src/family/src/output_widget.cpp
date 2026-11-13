#include <QScrollBar>
#include <QVBoxLayout>

#include <rgl_logger_handler.h>
#include <rgl_message_box.h>

#include "output_widget.h"

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

OutputWidget::OutputWidget(QWidget *parent)
    : QWidget{parent}
    , nUnseenWarnings{0}
    , nUnseenErrors{0}
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setContentsMargins(0,0,0,0);
    this->setLayout(mainLayout);

    this->applicationOutputBrowser = new RTextBrowser(true);
    mainLayout->addWidget(this->applicationOutputBrowser);

    // Logger signals
    QObject::connect(&RLoggerHandler::getInstance(),&RLoggerHandler::infoPrinted,this,&OutputWidget::onInfoPrinted);
    QObject::connect(&RLoggerHandler::getInstance(),&RLoggerHandler::noticePrinted,this,&OutputWidget::onNoticePrinted);
    QObject::connect(&RLoggerHandler::getInstance(),&RLoggerHandler::warningPrinted,this,&OutputWidget::onWarningPrinted);
    QObject::connect(&RLoggerHandler::getInstance(),&RLoggerHandler::errorPrinted,this,&OutputWidget::onErrorPrinted);
}

void OutputWidget::showEvent(QShowEvent *event)
{
    this->QWidget::showEvent(event);

    if (this->nUnseenWarnings > 0 || this->nUnseenErrors > 0)
    {
        this->nUnseenWarnings = 0;
        this->nUnseenErrors = 0;
        emit this->unseenChanged(this->nUnseenWarnings,this->nUnseenErrors);
    }
}

void OutputWidget::countUnseen(RMessage::Type messageType)
{
    if (this->isVisible())
    {
        return;
    }
    if (messageType == RMessage::Type::Error)
    {
        this->nUnseenErrors++;
    }
    else
    {
        this->nUnseenWarnings++;
    }
    emit this->unseenChanged(this->nUnseenWarnings,this->nUnseenErrors);
}

void OutputWidget::onInfoPrinted(const RMessage &message)
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
}

void OutputWidget::onNoticePrinted(const RMessage &message)
{
    RMessageBox::information(this->window(),tr("Notice"),QString(RMessage::messageToLogString(message)).replace("NOTICE: ",""));
}

void OutputWidget::onWarningPrinted(const RMessage &message)
{
    appendText(this->applicationOutputBrowser,RMessage::messageToLogString(message),QColor(170,0,0));
    QScrollBar *sb = this->applicationOutputBrowser->verticalScrollBar();
    sb->setValue(sb->maximum());
    this->countUnseen(RMessage::Type::Warning);
}

void OutputWidget::onErrorPrinted(const RMessage &message)
{
    appendText(this->applicationOutputBrowser,RMessage::messageToLogString(message),QColor(255,255,255),QColor(170,0,0));
    QScrollBar *sb = this->applicationOutputBrowser->verticalScrollBar();
    sb->setValue(sb->maximum());
    this->countUnseen(RMessage::Type::Error);
}
