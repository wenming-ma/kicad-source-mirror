#include <trace_helpers.h>

#include <QStringList>

const QChar* const traceFindReplace = "KICAD_FIND_REPLACE";
const QChar* const kicadTraceCoords = "KICAD_COORDS";
const QChar* const kicadTraceKeyEvent = "KICAD_KEY_EVENTS";
const QChar* const kicadTraceToolStack = "KICAD_TOOL_STACK";
const QChar* const kicadTraceCoroutineStack = "KICAD_COROUTINE_STACK";
const QChar* const traceSchLibMem = "KICAD_SCH_LIB_MEM";
const QChar* const traceFindItem = "KICAD_FIND_ITEM";
const QChar* const traceSchLegacyPlugin = "KICAD_SCH_LEGACY_PLUGIN";
const QChar* const traceSchPlugin = "KICAD_SCH_PLUGIN";
const QChar* const traceGedaPcbPlugin = "KICAD_GEDA_PLUGIN";
const QChar* const traceKicadPcbPlugin = "KICAD_PCB_PLUGIN";
const QChar* const tracePrinting = "KICAD_PRINT";
const QChar* const traceAutoSave = "KICAD_AUTOSAVE";
const QChar* const tracePathsAndFiles = "KICAD_PATHS_AND_FILES";
const QChar* const traceLocale = "KICAD_LOCALE";
const QChar* const traceFonts = "KICAD_FONTS";
const QChar* const traceScreen = "KICAD_SCREEN";
const QChar* const traceZoomScroll = "KICAD_ZOOM_SCROLL";
const QChar* const traceSymbolResolver = "KICAD_SYM_RESOLVE";
const QChar* const traceDisplayLocation = "KICAD_DISPLAY_LOCATION";
const QChar* const traceSchSheetPaths = "KICAD_SCH_SHEET_PATHS";
const QChar* const traceEnvVars = "KICAD_ENV_VARS";
const QChar* const traceGalProfile = "KICAD_GAL_PROFILE";
const QChar* const traceKiCad2Step = "KICAD2STEP";
const QChar* const traceUiProfile = "KICAD_UI_PROFILE";
const QChar* const traceGit = "KICAD_GIT";
const QChar* const traceEagleIo = "KICAD_EAGLE_IO";
const QChar* const traceDesignBlocks = "KICAD_DESIGN_BLOCK";


QString dump( const QStringList& aArray )
{
    QString tmp;

    for( int i = 0;  i < aArray.size();  i++ )
    {
        if( aArray[i].isEmpty() )
            tmp += "\"\" ";
        else
            tmp += aArray[i] + " ";
    }

    return tmp;
}


const char* GetVirtualKeyCodeName(int keycode)
{
    switch ( keycode )
    {
#define WXK_(x) \
        case WXK_##x: return #x;

        WXK_(BACK)
        WXK_(TAB)
        WXK_(RETURN)
        WXK_(ESCAPE)
        WXK_(SPACE)
        WXK_(DELETE)
        WXK_(START)
        WXK_(LBUTTON)
        WXK_(RBUTTON)
        WXK_(CANCEL)
        WXK_(MBUTTON)
        WXK_(CLEAR)
        WXK_(SHIFT)
        WXK_(ALT)
        WXK_(CONTROL)
        WXK_(MENU)
        WXK_(PAUSE)
        WXK_(CAPITAL)
        WXK_(END)
        WXK_(HOME)
        WXK_(LEFT)
        WXK_(UP)
        WXK_(RIGHT)
        WXK_(DOWN)
        WXK_(SELECT)
        WXK_(PRINT)
        WXK_(EXECUTE)
        WXK_(SNAPSHOT)
        WXK_(INSERT)
        WXK_(HELP)
        WXK_(NUMPAD0)
        WXK_(NUMPAD1)
        WXK_(NUMPAD2)
        WXK_(NUMPAD3)
        WXK_(NUMPAD4)
        WXK_(NUMPAD5)
        WXK_(NUMPAD6)
        WXK_(NUMPAD7)
        WXK_(NUMPAD8)
        WXK_(NUMPAD9)
        WXK_(MULTIPLY)
        WXK_(ADD)
        WXK_(SEPARATOR)
        WXK_(SUBTRACT)
        WXK_(DECIMAL)
        WXK_(DIVIDE)
        WXK_(F1)
        WXK_(F2)
        WXK_(F3)
        WXK_(F4)
        WXK_(F5)
        WXK_(F6)
        WXK_(F7)
        WXK_(F8)
        WXK_(F9)
        WXK_(F10)
        WXK_(F11)
        WXK_(F12)
        WXK_(F13)
        WXK_(F14)
        WXK_(F15)
        WXK_(F16)
        WXK_(F17)
        WXK_(F18)
        WXK_(F19)
        WXK_(F20)
        WXK_(F21)
        WXK_(F22)
        WXK_(F23)
        WXK_(F24)
        WXK_(NUMLOCK)
        WXK_(SCROLL)
        WXK_(PAGEUP)
        WXK_(PAGEDOWN)
        WXK_(NUMPAD_SPACE)
        WXK_(NUMPAD_TAB)
        WXK_(NUMPAD_ENTER)
        WXK_(NUMPAD_F1)
        WXK_(NUMPAD_F2)
        WXK_(NUMPAD_F3)
        WXK_(NUMPAD_F4)
        WXK_(NUMPAD_HOME)
        WXK_(NUMPAD_LEFT)
        WXK_(NUMPAD_UP)
        WXK_(NUMPAD_RIGHT)
        WXK_(NUMPAD_DOWN)
        WXK_(NUMPAD_PAGEUP)
        WXK_(NUMPAD_PAGEDOWN)
        WXK_(NUMPAD_END)
        WXK_(NUMPAD_BEGIN)
        WXK_(NUMPAD_INSERT)
        WXK_(NUMPAD_DELETE)
        WXK_(NUMPAD_EQUAL)
        WXK_(NUMPAD_MULTIPLY)
        WXK_(NUMPAD_ADD)
        WXK_(NUMPAD_SEPARATOR)
        WXK_(NUMPAD_SUBTRACT)
        WXK_(NUMPAD_DECIMAL)
        WXK_(NUMPAD_DIVIDE)

        WXK_(WINDOWS_LEFT)
        WXK_(WINDOWS_RIGHT)
#ifdef __WXOSX__
        WXK_(RAW_CONTROL)
#endif
#undef WXK_

    default:
        return nullptr;
    }
}


QString GetKeyName( const QKeyEvent &aEvent )
{
    int keycode = aEvent.key();
    const char* virt = GetVirtualKeyCodeName( keycode );

    if( virt )
        return QString::fromLatin1( virt );

    if( keycode > 0 && keycode < 32 )
        return QString( "Ctrl-%1" ).arg( QChar('A' + keycode - 1) );

    if( keycode >= 32 && keycode < 128 )
        return QString( "'%1'" ).arg( QChar(keycode) );

    QString unicodeText = aEvent.text();
    if( !unicodeText.isEmpty() )
        return QString( "'%1'" ).arg( unicodeText.at(0) );

    return "unknown";
}


QString dump( const QKeyEvent& aEvent )
{
    QString msg;
    QString eventType = "unknown";

    if( aEvent.type() == QEvent::KeyPress )
        eventType = "KeyDown";
    else if( aEvent.type() == QEvent::KeyRelease )
        eventType = "KeyUp";

    // event  key_name  KeyCode  modifiers  Unicode  raw_code raw_flags pos
    msg = QString( "%1 %2 %3   %4%5%6%7 %8 (U+%9)  %10    %11  (%12,%13)" )
                .arg( eventType, 7 )
                .arg( GetKeyName( aEvent ), 15 )
                .arg( aEvent.key(), 5 )
                .arg( aEvent.modifiers() & Qt::ControlModifier ? 'C' : '-' )
                .arg( aEvent.modifiers() & Qt::AltModifier     ? 'A' : '-' )
                .arg( aEvent.modifiers() & Qt::ShiftModifier   ? 'S' : '-' )
                .arg( aEvent.modifiers() & Qt::MetaModifier    ? 'M' : '-' )
                .arg( aEvent.text().isEmpty() ? 0 : aEvent.text().at(0).unicode(), 5 )
                .arg( aEvent.text().isEmpty() ? 0 : aEvent.text().at(0).unicode(), 4, 16 )
                .arg( aEvent.nativeScanCode(), 7 )
                .arg( aEvent.nativeModifiers(), 8, 16 )
                .arg( 0, 5 )  // Qt doesn't provide direct position in key events
                .arg( 0, 5 );

    return msg;
}


TRACE_MANAGER& TRACE_MANAGER::Instance()
{
    static TRACE_MANAGER* self = nullptr;

    if( !self )
    {
        self = new TRACE_MANAGER;
        self->init();
    }

    return *self;
}


bool TRACE_MANAGER::IsTraceEnabled( const QString& aWhat )
{
    if( !m_printAllTraces )
    {
        if( !m_globalTraceEnabled )
            return false;

        if( m_enabledTraces.find( aWhat ) == m_enabledTraces.end() )
            return false;
    }

    return true;
}


void TRACE_MANAGER::traceV( const QString& aWhat, const QString& aFmt, va_list vargs )
{
    if( !IsTraceEnabled( aWhat ) )
        return;

    QString str;
    str = str.vasprintf( aFmt.toLocal8Bit().constData(), vargs );

#if defined( __UNIX__ ) || defined( _WIN32 )
    fprintf( stderr, " %-30s | %s", aWhat.toLocal8Bit().constData(), str.toLocal8Bit().constData() );
#endif
}


void TRACE_MANAGER::init()
{
    QString traceVars;
    QByteArray envValue = qgetenv( "KICAD_TRACE" );
    m_globalTraceEnabled = !envValue.isEmpty();
    traceVars = QString::fromLocal8Bit( envValue );
    m_printAllTraces = false;

    if( !m_globalTraceEnabled )
        return;

    QStringList tokens = traceVars.split( ",", Qt::SkipEmptyParts );

    for( const QString& token : tokens )
    {
        m_enabledTraces[token] = true;

        if( token.toLower() == "all" )
            m_printAllTraces = true;
    }
}