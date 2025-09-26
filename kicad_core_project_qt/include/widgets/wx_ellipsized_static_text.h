
// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-24

#ifndef WX_ELLIPSIZED_STATIC_TEXT_H_
#define WX_ELLIPSIZED_STATIC_TEXT_H_

#include <QLabel>
#include <QString>
#include <QWidget>
#include <QSize>
#include <QPoint>

// A version of a QLabel control that will request a smaller size than the full string.
// This can be used with the ellipsization styles to ensure that the control will actually
// ellipsize properly inside layout elements.
class WX_ELLIPSIZED_STATIC_TEXT: public QLabel
{
public:
    WX_ELLIPSIZED_STATIC_TEXT( QWidget* aParent, int aID, const QString& aLabel,
                               const QPoint& aPos = QPoint(),
                               const QSize& aSize = QSize(),
                               long aStyle = 0 );

    // Set the string that is used for determining the requested size of the control.
    // The control will return this string length from sizeHint(), regardless of what string
    // the control is displaying.
    void SetMinimumStringLength( const QString& aString )
    {
        m_minimumString = aString;
    }

protected:
    QSize sizeHint() const override;

private:
    QString m_minimumString;  // The string that is used to set the minimum control width.
};


#endif // WX_ELLIPSIZED_STATIC_TEXT_H_
