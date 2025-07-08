#ifndef SCALE_CONTROLL_WIDGET_H
#define SCALE_CONTROLL_WIDGET_H

#include <QWidget>
#include <QDoubleSpinBox>

class ScaleControllWidget : public QWidget
{
    Q_OBJECT

    protected:

        //! Saved scale value.
        double savedScaleValue;

        //! Scale spin box.
        QDoubleSpinBox *scaleBox;

    public:

        //! Constructor.
        explicit ScaleControllWidget(QWidget *parent = nullptr);

    public slots:

        //! Set new scale.
        void setScale(double newScale);

    protected slots:

        void onDownButtonClicked();

        void onUpButtonClicked();

        void onResetButtonClicked();

        void onScaleValueChanged(double value);

    signals:

        void factorChanged(double factor);

};

#endif // SCALE_CONTROLL_WIDGET_H
