#ifndef NAME_EDIT_WIDGET_H
#define NAME_EDIT_WIDGET_H

#include <QWidget>

#include <rfl_name.h>

class NameEditWidget : public QWidget
{
    Q_OBJECT

    protected:

        FName name;

    public:

        explicit NameEditWidget(const FName &name, QWidget *parent = nullptr);

    protected slots:

        void onFirstNameChanged(const QString &text);
        void onMiddleNameChanged(const QString &text);
        void onLastNameChanged(const QString &text);
        void onMaidenNameChanged(const QString &text);

    signals:

        void changed(const FName &name);

};

#endif // NAME_EDIT_WIDGET_H
