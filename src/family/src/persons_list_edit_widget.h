#ifndef PERSONS_LIST_EDIT_WIDGET_H
#define PERSONS_LIST_EDIT_WIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QMap>
#include <QPushButton>

#include <rfl_tree.h>

#include <rgl_flow_layout.h>
#include <rgl_label_widget.h>

class PersonsListEditWidget : public QWidget
{
    Q_OBJECT

    protected:

        const FTree *familyTree;
        QList<QUuid> personsList;
        QList<QUuid> excludeList;

        RFlowLayout *labelLayout;
        QMap<QUuid,RLabelWidget*> labelWidgets;
        QLineEdit *lineEdit;
        QPushButton *addButton;

    public:

        explicit PersonsListEditWidget(const FTree *familyTree,
                                       const QList<QUuid> &personsList,
                                       const QList<QUuid> &excludeList = QList<QUuid>(),
                                       QWidget *parent = nullptr);

    protected:

        void addLabel(const QUuid &personId);

        static QString getPersonString(const FPerson &person);

        void updateLineEditCompleter();

    protected slots:

        void onPersonAdded(const FPerson &);

        void onPersonChanged(const FPerson &);

        void onPersonRemoved(const QUuid &);

        void onFileLoaded(const QString &);

        void onCloseLabelClicked(const QVariant &data);

        void onTextEdited(const QString &text);

        void onAddClicked();

    signals:

        void changed(const QList<QUuid> &personsList);

};

#endif // PERSONS_LIST_EDIT_WIDGET_H
