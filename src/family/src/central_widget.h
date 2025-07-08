#ifndef CENTRAL_WIDGET_H
#define CENTRAL_WIDGET_H

#include <QUuid>
#include <QTabWidget>

#include <rbl_message.h>

#include <rgl_text_browser.h>

#include "session.h"
#include "editor_widget.h"

class CentralWidget : public QWidget
{
    Q_OBJECT

    protected:

        //! Tab widget.
        QTabWidget *tabWidget;
        //! Editor widget.
        EditorWidget *editor;
        //! Log output browser.
        RTextBrowser *applicationOutputBrowser;
        //! Editor tab position.
        int editorTabPosition;
        //! Output tab position.
        int outputTabPosition;
    public:

        enum TabType
        {
            Editor = 0,
            Output
        };

    public:

        //! Constructor.
        explicit CentralWidget(Session *session, QWidget *parent = nullptr);

    private:

        //! Set text and icon for given tab.
        void setTabTitle(CentralWidget::TabType tabType, RMessage::Type messageType = RMessage::Type::None, const QString &additionalText = QString());

    private slots:

        //! Print application info message.
        void onInfoPrinted(const QString &message);

        //! Print application notice message.
        void onNoticePrinted(const QString &message);

        //! Print application warning message.
        void onWarningPrinted(const QString &message);

        //! Print application error message.
        void onErrorPrinted(const QString &message);

        //! Central tab widget current changed.
        void onCurrentChanged(int tabPosition);

};

#endif // CENTRAL_WIDGET_H
