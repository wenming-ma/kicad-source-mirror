
// QT_TRANSFORMATION_COMPLETED

#pragma once

#include <optional>
#include <vector>

#include <gal/color4d.h>

#include <QWidget>
#include <QSize>
#include <QPoint>
#include <QPaintEvent>
#include <QPainter>
#include <QString>

using KIGFX::COLOR4D;

#define MSG_PANEL_DEFAULT_PAD      6  ///< The default number of spaces between each text string.


class EDA_MSG_PANEL;
class KIID;


class MSG_PANEL_ITEM
{
public:
    MSG_PANEL_ITEM( const QString& aUpperText, const QString& aLowerText,
                    int aPadding = MSG_PANEL_DEFAULT_PAD ) :
            m_UpperText( aUpperText ),
            m_LowerText( aLowerText ),
            m_Padding( aPadding )
    {
        m_X = 0;
        m_UpperY = 0;
        m_LowerY = 0;
    }

    MSG_PANEL_ITEM() :
            m_Padding( MSG_PANEL_DEFAULT_PAD )

    {
        m_X = 0;
        m_UpperY = 0;
        m_LowerY = 0;
    }

    void SetUpperText( const QString& aUpperText ) { m_UpperText = aUpperText; }
    const QString& GetUpperText() const { return m_UpperText; }

    void SetLowerText( const QString& aLowerText )  { m_LowerText = aLowerText; }
    const QString& GetLowerText() const { return m_LowerText; }

    void SetPadding( int aPadding )  { m_Padding = aPadding; }
    int GetPadding() const { return m_Padding; }

private:
    friend class EDA_MSG_PANEL;

    int         m_X;
    int         m_UpperY;
    int         m_LowerY;
    QString     m_UpperText;
    QString     m_LowerText;
    int         m_Padding;
};


class EDA_MSG_PANEL : public QWidget
{
    Q_OBJECT

public:
    EDA_MSG_PANEL( QWidget* aParent = nullptr );
    ~EDA_MSG_PANEL();

    void OnPaint( QPaintEvent* aEvent );
    void EraseMsgBox();

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    void SetMessage( int aXPosition, const QString& aUpperText, const QString& aLowerText );

    void AppendMessage( const QString& aUpperText, const QString& aLowerText, int aPadding = 6 );

    void AppendMessage( const MSG_PANEL_ITEM& aMessageItem )
    {
        AppendMessage( aMessageItem.GetUpperText(), aMessageItem.GetLowerText(),
                       aMessageItem.GetPadding() );
    }

protected:
    void paintEvent( QPaintEvent* event ) override;

    void updateFontSize();

    void showItem( QPainter& painter, const MSG_PANEL_ITEM& aItem );

    void erase( QPainter* painter );

protected:
    std::vector<MSG_PANEL_ITEM> m_Items;
    int                         m_last_x;      // the last used x coordinate
    QSize                       m_fontSize;
};


std::optional<QString> GetMsgPanelDisplayUuid( const KIID& aKiid );
