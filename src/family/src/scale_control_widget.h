#ifndef SCALE_CONTROL_WIDGET_H
#define SCALE_CONTROL_WIDGET_H

#include <QWidget>
#include <QDoubleSpinBox>

class ScaleControlWidget : public QWidget
{
    Q_OBJECT

    protected:

        //! Saved scale value.
        double savedScaleValue;

        //! Scale spin box.
        QDoubleSpinBox *scaleBox;

    public:

        //! Constructor.
        explicit ScaleControlWidget(QWidget *parent = nullptr);

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

#endif // SCALE_CONTROL_WIDGET_H
