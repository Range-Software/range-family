#include <QVBoxLayout>
#include <QLineEdit>
#include <QImage>
#include <QPushButton>
#include <QApplication>
#include <QFileDialog>
#include <QBuffer>

#include <rbl_logger.h>

#include "picture_edit_widget.h"
#include "image_button.h"

#define _FN_TR(qstr) qApp->translate("FPicture",qstr.toUtf8().constData())

PictureEditWidget::PictureEditWidget(const FPicture &picture, QWidget *parent)
    : QWidget{parent}
    , picture{picture}
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setContentsMargins(0,0,0,0);
    this->setLayout(mainLayout);

    QLineEdit *title = new QLineEdit(picture.getTitle());
    title->setPlaceholderText(_FN_TR(FPicture::ElementDesc::title));
    title->setClearButtonEnabled(true);
    mainLayout->addWidget(title);

    QLineEdit *description = new QLineEdit(picture.getDescription());
    description->setPlaceholderText(_FN_TR(FPicture::ElementDesc::description));
    description->setClearButtonEnabled(true);
    mainLayout->addWidget(description);

    ImageButton *pictureButton = new ImageButton(QImage::fromData(picture.getData()));
    pictureButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mainLayout->addWidget(pictureButton,1);

    QObject::connect(title,&QLineEdit::textChanged,this,&PictureEditWidget::onTitleChanged);
    QObject::connect(description,&QLineEdit::textChanged,this,&PictureEditWidget::onDescriptionChanged);
    QObject::connect(pictureButton,&ImageButton::imageChanged,this,&PictureEditWidget::onPictureButtonImageChanged);
}

void PictureEditWidget::onTitleChanged(const QString &text)
{
    this->picture.setTitle(text);
    emit this->changed(this->picture);
}

void PictureEditWidget::onDescriptionChanged(const QString &text)
{
    this->picture.setDescription(text);
    emit this->changed(this->picture);
}

void PictureEditWidget::onPictureButtonImageChanged(const QImage &image)
{
    uint width = std::min(FPicture::maxWidth,uint(image.width()));
    uint height = std::min(FPicture::maxHeight,uint(image.height()));

    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    if (!image.scaled(width,height,Qt::KeepAspectRatio).save(&buffer, "PNG"))
    {
        RLogger::error("Failed to save image into buffer.\n");
        return;
    }
    this->picture.setData(buffer.data());
    emit this->changed(this->picture);
}
