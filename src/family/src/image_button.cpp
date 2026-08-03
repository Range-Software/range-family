#include <cmath>

#include <QBuffer>
#include <QFileDialog>
#include <QMessageBox>

#include <rbl_file_tools.h>
#include <rbl_logger.h>

#include <rgl_application_settings.h>

#include <rfl_tree.h>

#include "application.h"
#include "image_button.h"

const qsizetype ImageButton::maxTreeDataSize = 1024 * 1024;

ImageButton::ImageButton(const QImage &image, const QUuid &personId, uint maxWidth, uint maxHeight, QWidget *parent)
    : QPushButton(parent)
    , image{image}
    , personId{personId}
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

QImage ImageButton::reduceImage(const QImage &image, qsizetype maxDataSize)
{
    QImage reducedImage(image);

    while (!reducedImage.isNull())
    {
        QBuffer buffer;
        buffer.open(QIODevice::WriteOnly);
        if (!reducedImage.save(&buffer,"PNG"))
        {
            RLogger::error("Failed to save image into buffer.\n");
            return reducedImage;
        }

        qsizetype dataSize = buffer.data().size();

        if (dataSize <= maxDataSize)
        {
            break;
        }

        // Encoded size does not scale linearly with the number of pixels - reduce
        // the image in steps and check the encoded size after each of them.
        double ratio = std::min(std::sqrt(double(maxDataSize) / double(dataSize)),0.9);

        int width = int(double(reducedImage.width()) * ratio);
        int height = int(double(reducedImage.height()) * ratio);

        if (width < 1 || height < 1)
        {
            break;
        }

        RLogger::info("Reducing image from %dx%d to %dx%d to fit into %lld bytes.\n",
                      reducedImage.width(),
                      reducedImage.height(),
                      width,
                      height,
                      qint64(maxDataSize));

        reducedImage = reducedImage.scaled(width,height,Qt::KeepAspectRatio,Qt::SmoothTransformation);
    }

    return reducedImage;
}

void ImageButton::onButtonClicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("Select image"),
                                                    RApplicationSettings::getHomeDir(),
                                                    tr("Image files") + " (*.png *.xpm *.jpg *.jpeg)");
    if (fileName.isEmpty())
    {
        return;
    }

    QByteArray fileContent;
    if (!RFileTools::readBinaryFile(fileName, fileContent))
    {
        RLogger::error("Failed to read binary file \'%s\'.",fileName.toUtf8().constData());
        return;
    }

    QImage image = QImage::fromData(fileContent);
    if (image.isNull())
    {
        RLogger::error("Failed to read image from file \'%s\'.\n",fileName.toUtf8().constData());
        return;
    }

    QString treeFileName = Application::instance()->getSession()->getTreeFileName();
    QString pictureFilePath = FTree::pictureFilePath(treeFileName,this->personId);

    QMessageBox messageBox(this);
    messageBox.setIcon(QMessageBox::Question);
    messageBox.setWindowTitle(tr("Store image"));
    messageBox.setText(tr("How should the image be stored?"));
    messageBox.setInformativeText(tr("An image stored within the family tree is reduced to occupy a maximum of %1 MB.")
                                      .arg(ImageButton::maxTreeDataSize / (1024 * 1024))
                                  + "\n"
                                  + (pictureFilePath.isEmpty()
                                         ? tr("The family tree must be saved to a file and the person must be a part of it before an image can be stored as a separate file.")
                                         : tr("A separate image will be stored as file \'%1\'.").arg(pictureFilePath)));

    QPushButton *treeButton = messageBox.addButton(tr("Within the tree"),QMessageBox::AcceptRole);
    QPushButton *fileButton = messageBox.addButton(tr("Separate file"),QMessageBox::AcceptRole);
    messageBox.addButton(QMessageBox::Cancel);
    messageBox.setDefaultButton(treeButton);

    fileButton->setEnabled(!pictureFilePath.isEmpty());

    messageBox.exec();

    QString pictureUrl;

    if (messageBox.clickedButton() == treeButton)
    {
        image = ImageButton::reduceImage(image,ImageButton::maxTreeDataSize);
    }
    else if (messageBox.clickedButton() == fileButton)
    {
        pictureUrl = pictureFilePath;
    }
    else
    {
        return;
    }

    this->setImage(image);
    emit this->imageChanged(this->image,pictureUrl);
}
