// QT_TRANSFORMATION_COMPLETED

#ifndef LIB_TREE_MODEL_H
#define LIB_TREE_MODEL_H

#include <vector>
#include <map>
#include <memory>
#include <QString>
#include <eda_pattern_match.h>
#include <lib_tree_item.h>


class LIB_TREE_NODE
{
public:
    virtual void UpdateScore( const std::vector<std::unique_ptr<EDA_COMBINED_MATCHER>>& aMatchers,
                              std::function<bool( LIB_TREE_NODE& aNode )>* aFilter ) = 0;

    void AssignIntrinsicRanks( bool presorted = false );

    void SortNodes( bool aUseScores );
    static bool Compare( LIB_TREE_NODE const& aNode1, LIB_TREE_NODE const& aNode2,
                         bool aUseScores );

    LIB_TREE_NODE();
    virtual ~LIB_TREE_NODE() {}

    enum class TYPE
    {
        ROOT,
        LIBRARY,
        ITEM,
        UNIT,
        INVALID
    };

    typedef std::vector<std::unique_ptr<LIB_TREE_NODE>> PTR_VECTOR;

    LIB_TREE_NODE*  m_Parent;
    PTR_VECTOR      m_Children;
    enum TYPE       m_Type;

    int         m_IntrinsicRank;
    int         m_Score;
    bool        m_Pinned;

    QString     m_Name;
    QString     m_Desc;
    QString     m_Footprint;
    int         m_PinCount;

    std::vector<SEARCH_TERM>     m_SearchTerms;
    std::map<QString, QString>   m_Fields;

    LIB_ID      m_LibId;
    int         m_Unit;
    bool        m_IsRoot;

    bool        m_IsRecentlyUsedGroup;
    bool        m_IsAlreadyPlacedGroup;
};


class LIB_TREE_NODE_UNIT: public LIB_TREE_NODE
{
public:
    LIB_TREE_NODE_UNIT( LIB_TREE_NODE_UNIT const& _ ) = delete;
    void operator=( LIB_TREE_NODE_UNIT const& _ ) = delete;

    LIB_TREE_NODE_UNIT( LIB_TREE_NODE* aParent, LIB_TREE_ITEM* aItem, int aUnit );

    void UpdateScore( const std::vector<std::unique_ptr<EDA_COMBINED_MATCHER>>& aMatchers,
                      std::function<bool( LIB_TREE_NODE& aNode )>* aFilter ) override;
};


class LIB_TREE_NODE_ITEM : public LIB_TREE_NODE
{
public:
    LIB_TREE_NODE_ITEM( LIB_TREE_NODE_ITEM const& _ ) = delete;
    void operator=( LIB_TREE_NODE_ITEM const& _ ) = delete;

    LIB_TREE_NODE_ITEM( LIB_TREE_NODE* aParent, LIB_TREE_ITEM* aItem );

    void Update( LIB_TREE_ITEM* aItem );
    void UpdateScore( const std::vector<std::unique_ptr<EDA_COMBINED_MATCHER>>& aMatchers,
                      std::function<bool( LIB_TREE_NODE& aNode )>* aFilter ) override;

protected:
    LIB_TREE_NODE_UNIT& AddUnit( LIB_TREE_ITEM* aItem, int aUnit );
};


class LIB_TREE_NODE_LIBRARY : public LIB_TREE_NODE
{
public:
    LIB_TREE_NODE_LIBRARY( LIB_TREE_NODE_LIBRARY const& _ ) = delete;
    void operator=( LIB_TREE_NODE_LIBRARY const& _ ) = delete;

    LIB_TREE_NODE_LIBRARY( LIB_TREE_NODE* aParent, const QString& aName, const QString& aDesc );

    LIB_TREE_NODE_ITEM& AddItem( LIB_TREE_ITEM* aItem );

    void UpdateScore( const std::vector<std::unique_ptr<EDA_COMBINED_MATCHER>>& aMatchers,
                      std::function<bool( LIB_TREE_NODE& aNode )>* aFilter ) override;
};


class LIB_TREE_NODE_ROOT: public LIB_TREE_NODE
{
public:
    LIB_TREE_NODE_ROOT( LIB_TREE_NODE_ROOT const& _ ) = delete;
    void operator=( LIB_TREE_NODE_ROOT const& _ ) = delete;

    LIB_TREE_NODE_ROOT();

    LIB_TREE_NODE_LIBRARY& AddLib( QString const& aName, QString const& aDesc );

    void RemoveGroup( bool aRecentlyUsedGroup, bool aAlreadyPlacedGroup );

    void Clear();

    void UpdateScore( const std::vector<std::unique_ptr<EDA_COMBINED_MATCHER>>& aMatchers,
                      std::function<bool( LIB_TREE_NODE& aNode )>* aFilter ) override;
};


#endif // LIB_TREE_MODEL_H
