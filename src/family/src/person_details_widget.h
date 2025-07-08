#ifndef PERSON_DETAILS_WIDGET_H
#define PERSON_DETAILS_WIDGET_H

#include <QWidget>

#include <rfl_tree.h>

#include <rgl_html_viewer.h>

class PersonDetailsWidget : public QWidget
{

    Q_OBJECT

    protected:

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

    protected slots:

        //! Person changed.
        void onPersonChanged(const FPerson &person);

};

#endif // PERSON_DETAILS_WIDGET_H
