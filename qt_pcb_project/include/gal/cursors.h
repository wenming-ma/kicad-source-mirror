
#ifndef CURSOR_STORE__H
#define CURSOR_STORE__H

#include <QCursor>
#include <QHash>
#include <QVector>
#include <QSize>
#include <QPoint>


enum class KICURSOR
{
    DEFAULT,
    ARROW,
    ARROW64,
    MOVING,
    MOVING64,
    PENCIL,
    PENCIL64,
    REMOVE,
    REMOVE64,
    HAND,
    HAND64,
    BULLSEYE,
    BULLSEYE64,
    VOLTAGE_PROBE,
    VOLTAGE_PROBE64,
    CURRENT_PROBE,
    CURRENT_PROBE64,
    TUNE,
    TUNE64,
    TEXT,
    TEXT64,
    MEASURE,
    MEASURE64,
    ADD,
    ADD64,
    SUBTRACT,
    SUBTRACT64,
    XOR,
    XOR64,
    ZOOM_IN,
    ZOOM_IN64,
    ZOOM_OUT,
    ZOOM_OUT64,
    LABEL_NET,
    LABEL_NET64,
    LABEL_GLOBAL,
    LABEL_GLOBAL64,
    COMPONENT,
    COMPONENT64,
    SELECT_WINDOW,
    SELECT_WINDOW64,
    SELECT_LASSO,
    SELECT_LASSO64,
    LINE_BUS,
    LINE_BUS64,
    LINE_GRAPHIC,
    LINE_GRAPHIC64,
    LINE_WIRE,
    LINE_WIRE64,
    LABEL_HIER,
    LABEL_HIER64,
    PLACE,
    PLACE64
};

class CURSOR_STORE
{
public:
    /**
     * Definition of a cursor
     */
    struct CURSOR_DEF
    {
        ///< The ID key used to uniquely identify a cursor in a given store
        KICURSOR m_id_key;

        ///< The image data bitmap
        const unsigned char* m_image_data;

        ///< The mask data bitmap
        const unsigned char* m_mask_data;

        const char** m_xpm;

        QSize m_size;

        QPoint m_hotspot;
    };

    CURSOR_STORE( const QVector<CURSOR_DEF>& aDefs );

    const QCursor& Get( KICURSOR aIdKey ) const;

    static const QCursor GetCursor( KICURSOR aCursorType );

    static const QCursor GetHiDPICursor( KICURSOR aCursorType );

    static Qt::CursorShape GetStockCursor( KICURSOR aCursorType );

private:
    QHash<KICURSOR, QCursor> m_store;
};

#endif // CURSOR_STORE__H
