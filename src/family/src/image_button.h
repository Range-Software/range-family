#ifndef IMAGE_BUTTON_H
#define IMAGE_BUTTON_H

#include <QPushButton>
#include <QWidget>

class ImageButton : public QPushButton
{

    Q_OBJECT

    protected:

        QImage image;
        uint maxWidth;
        uint maxHeight;

    public:

        //! Constructor.
        explicit ImageButton(const QImage &image, uint maxWidth = 150, uint maxHeight = 150, QWidget *parent = nullptr);

        //! Set new image.
        void setImage(const QImage &image);

    protected slots:

        void onButtonClicked();

    signals:

        void imageChanged(const QImage &image);

};

#endif // IMAGE_BUTTON_H
