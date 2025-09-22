
// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-21

#ifndef STEPPED_SLIDER_H
#define STEPPED_SLIDER_H

#include <QSlider>
#include <QWidget>
#include <QString>
#include <QPoint>
#include <QSize>

/**
 * Customized QSlider with forced stepping.
 */
class STEPPED_SLIDER : public QSlider
{
    Q_OBJECT

public:
    STEPPED_SLIDER(
            QWidget* aParent,
            int aValue,
            int aMinValue,
            int aMaxValue,
            const QPoint& aPos = QPoint(),
            const QSize& aSize = QSize(),
            Qt::Orientation aOrientation = Qt::Horizontal,
            const QString& aName = QString() );

    virtual ~STEPPED_SLIDER();

    /**
     * Set the step size.
     */
    void SetStep( int aSize );

    /**
     * Get the step size.
     */
    int GetStep() const;

protected slots:
    void OnScroll( int aValue );

private:
    int m_step;
};


#endif // STEPPED_SLIDER_H
