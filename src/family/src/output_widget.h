#ifndef OUTPUT_WIDGET_H
#define OUTPUT_WIDGET_H

#include <QWidget>

#include <rbl_message.h>

#include <rgl_text_browser.h>

//! Application output with messages of the logger.
class OutputWidget : public QWidget
{

    Q_OBJECT

    protected:

        //! Log output browser.
        RTextBrowser *applicationOutputBrowser;
        //! Number of warnings printed while the output was not visible.
        int nUnseenWarnings;
        //! Number of errors printed while the output was not visible.
        int nUnseenErrors;

    public:

        //! Constructor.
        explicit OutputWidget(QWidget *parent = nullptr);

    protected:

        //! Messages are seen once the output is shown.
        void showEvent(QShowEvent *event) override;

    private:

        //! Count warning or error printed while the output is not visible.
        void countUnseen(RMessage::Type messageType);

    private slots:

        //! Print application info message.
        void onInfoPrinted(const RMessage &message);

        //! Print application notice message.
        void onNoticePrinted(const RMessage &message);

        //! Print application warning message.
        void onWarningPrinted(const RMessage &message);

        //! Print application error message.
        void onErrorPrinted(const RMessage &message);

    signals:

        //! Number of warnings and errors printed while the output was not visible has changed.
        void unseenChanged(int nWarnings, int nErrors);

};

#endif // OUTPUT_WIDGET_H
