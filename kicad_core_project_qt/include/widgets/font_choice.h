
// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-21

#ifndef FONT_CHOICE_H
#define FONT_CHOICE_H

#include <QComboBox>
#include <QFontDatabase>
#include <font/font.h>


class FONT_CHOICE : public QComboBox
{
public:
    FONT_CHOICE( QWidget* aParent, int aId, QPoint aPosition, QSize aSize, int nChoices,
                 QString* aChoices, int aStyle );

    virtual ~FONT_CHOICE();

    /**
     * Set the selection in QComboBox widget
     * @param aFont is the font to select
     * @param aSilentMode allow or not a currentIndexChanged signal on selection
     * default = false: the signal is sent.
     * if true, just make silently the selection
     */
    void SetFontSelection( KIFONT::FONT* aFont, bool aSilentMode = false );

    bool HaveFontSelection() const;

    KIFONT::FONT* GetFontSelection( bool aBold, bool aItalic, bool aForDrawingSheet = false ) const;

private:
    int       m_systemFontCount;
    QString  m_notFound;
};

#endif // FONT_CHOICE_H
