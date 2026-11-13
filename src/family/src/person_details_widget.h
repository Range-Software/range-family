#ifndef PERSON_DETAILS_WIDGET_H
#define PERSON_DETAILS_WIDGET_H

#include <QWidget>

#include <rfl_tree.h>

#include <rgl_html_viewer.h>

class PersonDetailsWidget : public QWidget
{

    Q_OBJECT

    protected:

        //! Largest number of persons shown with full details. More persons are shown as a summary.
        static constexpr int maxDetailedPersons = 3;

        const FTree *tree;
        QList<QUuid> personsIds;

        RHtmlViewer *htmlViewer;

    public:

        //! Constructor.
        explicit PersonDetailsWidget(const FTree *tree, QWidget *parent = nullptr);

        //! Set list of persons to show.
        void setPersons(const QList<QUuid> &personsIds);

    protected:

        //! Populate.
        void populate();

        //! Generate markdown.
        QString generateMarkdown() const;

        //! Generate html.
        QString generateHtml() const;

        //! Return name of given person as a link activating the person.
        static QString personLink(const FPerson &person);

        //! Return name of given person as a link activating the person, followed by dates of birth and death.
        static QString personLinkWithDates(const FPerson &person);

        //! Return date and place of given event.
        static QString eventString(const FEvent &event);

    protected slots:

        //! Person changed.
        void onPersonChanged(const FPerson &person);

        //! Application style or color scheme has been applied.
        void onStyleApplied();

        //! Link in the details has been clicked.
        void onAnchorClicked(const QUrl &url);

    signals:

        //! Link to given person has been clicked.
        void personActivated(const QUuid &personId);

};

#endif // PERSON_DETAILS_WIDGET_H
