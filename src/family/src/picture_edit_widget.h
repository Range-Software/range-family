#ifndef PICTURE_EDIT_WIDGET_H
#define PICTURE_EDIT_WIDGET_H

#include <QWidget>

#include <rfl_picture.h>

class PictureEditWidget : public QWidget
{

    Q_OBJECT

    protected:

        FPicture picture;

    public:

        //! Constructor.
        explicit PictureEditWidget(const FPicture &picture, QWidget *parent = nullptr);

    protected slots:

        void onTitleChanged(const QString &text);

        void onDescriptionChanged(const QString &text);

        void onPictureButtonImageChanged(const QImage &image);

    signals:

        void changed(const FPicture &picture);


};

#endif // PICTURE_EDIT_WIDGET_H
