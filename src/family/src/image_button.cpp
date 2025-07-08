#include <QFileDialog>

#include <rbl_logger.h>
#include <rcl_file_tools.h>

#include <rgl_application_settings.h>

#include "image_button.h"

ImageButton::ImageButton(const QImage &image, uint maxWidth, uint maxHeight, QWidget *parent)
    : QPushButton(parent)
    , image{image}
    , maxWidth(maxWidth)
    , maxHeight(maxHeight)
{
    this->setImage(image);

    QObject::connect(this,&QPushButton::clicked,this,&ImageButton::onButtonClicked);
}

void ImageButton::setImage(const QImage &image)
{
    if (image.isNull())
    {
        this->setText(tr("Click to select"));
    }
    else
    {
        this->image = image;
        QPixmap iconPixmap = QPixmap::fromImage(this->image);

        QSize iconSize(iconPixmap.rect().size());

        iconSize.scale(this->maxWidth,this->maxHeight,Qt::KeepAspectRatio);

        this->setIcon(QIcon(iconPixmap));
        this->setIconSize(iconSize);
        this->setText(QString());
    }
}

void ImageButton::onButtonClicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("Select image"),
                                                    RApplicationSettings::getHomeDir(),
                                                    tr("Image files") + " (*.png *.xpm *.jpg *.jpeg)");
    if (!fileName.isEmpty())
    {
        QByteArray fileContent;
        if (!RFileTools::readBinaryFile(fileName, fileContent))
        {
            RLogger::error("Failed to read binary file \'%s\'.",fileName.toUtf8().constData());
            return;
        }
        this->setImage(QImage::fromData(fileContent));
        emit this->imageChanged(this->image);
    }
}
