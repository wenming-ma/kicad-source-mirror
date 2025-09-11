
// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-05

#ifndef FROM_TO_CACHE_H
#define FROM_TO_CACHE_H

#include <set>
#include <QString>

class PAD;
class BOARD_CONNECTED_ITEM;

class FROM_TO_CACHE
{
public:

    struct FT_ENDPOINT
    {
        QString name;
        PAD*    parent;
    };

    struct FT_PATH
    {
        int      net;
        PAD*     from;
        PAD*     to;
        QString  fromName, toName;
        QString  fromWildcard, toWildcard;
        bool     isUnique;
        std::set<BOARD_CONNECTED_ITEM*> pathItems;
    };

    FROM_TO_CACHE( BOARD* aBoard = nullptr ) :
        m_board( aBoard )
    {
    }

    ~FROM_TO_CACHE()
    {
    }

    void Rebuild( BOARD* aBoard );
    bool IsOnFromToPath( BOARD_CONNECTED_ITEM* aItem, const QString& aFrom, const QString& aTo );

    FT_PATH* QueryFromToPath( const std::set<BOARD_CONNECTED_ITEM*>& aItems );

private:
    int cacheFromToPaths( const QString& aFrom, const QString& aTo );
    void buildEndpointList();

private:
    std::vector<FT_ENDPOINT> m_ftEndpoints;
    std::vector<FT_PATH>     m_ftPaths;

    BOARD*                   m_board;
};

#endif
