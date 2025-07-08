#ifndef PERSON_EDIT_WIDGET_H
#define PERSON_EDIT_WIDGET_H

#include <QWidget>
#include <QPlainTextEdit>

#include <rfl_person.h>

class PersonEditWidget : public QWidget
{
    Q_OBJECT

    protected:

        FPerson person;

        QPlainTextEdit *textEdit;

    public:

        //! Constructor.
        explicit PersonEditWidget(const FPerson &person, QWidget *parent = nullptr);

        //! Return const reference to person.
        const FPerson &getPerson() const;

    protected slots:

        void onSexIndexChanged(int sexIndex);
        void onNameChanged(const FName &name);
        void onBirthEventChanged(const FEvent &event);
        void onDeathEventChanged(const FEvent &event);
        void onPictureChanged(const FPicture &picture);
        void onTextEditChanged();

    signals:

        void changed(const FPerson &person);

};

#endif // PERSON_EDIT_WIDGET_H
