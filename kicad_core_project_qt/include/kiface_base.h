
#ifndef KIFACE_BASE_H
#define KIFACE_BASE_H

#include <kiway.h>
#include <bin_mod.h>
#include <tool/action_manager.h>


class KIFACE_BASE : public KIFACE
{
public:
    virtual bool OnKifaceStart( PGM_BASE* aProgram, int aCtlBits, KIWAY* aKiway ) override = 0;

    virtual void OnKifaceEnd() override
    {
        // overload this if you want, end_common() may be handy.
        end_common();
    }

    virtual  QWidget* CreateKiWindow( QWidget* aParent, int aClassId, KIWAY* aKIWAY,
                                      int aCtlBits = 0 ) override = 0;

    virtual void Reset() override{};

    virtual void* IfaceOrAddress( int aDataId ) override = 0;

    KIFACE_BASE( const char* aKifaceName, KIWAY::FACE_T aId ) :
        m_start_flags( 0 ),
        m_id( aId ),
        m_bm( aKifaceName )
    {
    }


protected:

    bool start_common( int aCtlBits );

    void end_common();


public:

    const QString Name()
    {
        return QString::fromUtf8( m_bm.m_name );
    }

    APP_SETTINGS_BASE* KifaceSettings() const         { return m_bm.m_config; }

    void InitSettings( APP_SETTINGS_BASE* aSettings ) { m_bm.InitSettings( aSettings ); }

    int StartFlags() const                            { return m_start_flags; }

    bool IsSingle() const                             { return m_start_flags & KFCTL_STANDALONE; }

    const QString& GetHelpFileName() const           { return m_bm.m_help_file; }

    SEARCH_STACK&       KifaceSearch()                { return m_bm.m_search; }

    void GetActions( std::vector<TOOL_ACTION*>& aActions ) const override
    {
        for( TOOL_ACTION* action : ACTION_MANAGER::GetActionList() )
            aActions.push_back( action );
    }

protected:
    int m_start_flags;      // flags provided in OnKifaceStart()

private:
    KIWAY::FACE_T m_id;
    BIN_MOD       m_bm;
};


KIFACE_BASE& Kiface();

#endif // KIFACE_BASE_H
