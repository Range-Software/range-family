#ifndef IMAGE_BUTTON_H
#define IMAGE_BUTTON_H

#include <QPushButton>
#include <QToolButton>
#include <QUuid>
#include <QWidget>

class ImageButton : public QPushButton
{

    Q_OBJECT

    public:

        //! Maximum number of bytes an image stored within the family tree can occupy.
        static const qsizetype maxTreeDataSize;

    protected:

        QImage image;
        //! ID of the person the image belongs to.
        QUuid personId;
        uint maxWidth;
        uint maxHeight;
        //! Button removing the current image.
        QToolButton *clearButton;

    public:

        //! Constructor.
        explicit ImageButton(const QImage &image, const QUuid &personId, uint maxWidth = 150, uint maxHeight = 150, QWidget *parent = nullptr);

        //! Set new image.
        void setImage(const QImage &image);

    protected:

        //! Return image scaled down so that its PNG representation does not exceed maxDataSize bytes.
        static QImage reduceImage(const QImage &image, qsizetype maxDataSize);

        //! Move the clear button to the upper right corner.
        void placeClearButton();

        //! Widget has been resized.
        void resizeEvent(QResizeEvent *event) override;

    protected slots:

        void onButtonClicked();

        void onClearButtonClicked();

    signals:

        //! Image has changed. Url is empty if the image is to be stored within the family tree.
        void imageChanged(const QImage &image, const QString &url);

};

#endif // IMAGE_BUTTON_H
