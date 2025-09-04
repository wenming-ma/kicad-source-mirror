// QT_TRANSFORMATION_COMPLETED

#ifndef STEPPED_SLIDER_H
#define STEPPED_SLIDER_H

#include <QSlider>
#include <QWidget>

class STEPPED_SLIDER : public QSlider
{
    Q_OBJECT

public:
    STEPPED_SLIDER(
            QWidget* aParent = nullptr,
            int aValue = 0,
            int aMinValue = 0,
            int aMaxValue = 100,
            Qt::Orientation aOrientation = Qt::Horizontal );

    virtual ~STEPPED_SLIDER();

    void SetStep( int aSize );

    int GetStep() const;

private:
    int m_step;

private slots:
    void OnValueChanged( int aValue );
};


#endif // STEPPED_SLIDER_H