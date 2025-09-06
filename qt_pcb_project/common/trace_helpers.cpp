#include <trace_helpers.h>

#include <QStringList>
#include <Qt>

const char* const traceFindReplace = "KICAD_FIND_REPLACE";
const char* const kicadTraceCoords = "KICAD_COORDS";
const char* const kicadTraceKeyEvent = "KICAD_KEY_EVENTS";
const char* const kicadTraceToolStack = "KICAD_TOOL_STACK";
const char* const kicadTraceCoroutineStack = "KICAD_COROUTINE_STACK";
const char* const traceSchLibMem = "KICAD_SCH_LIB_MEM";
const char* const traceFindItem = "KICAD_FIND_ITEM";
const char* const traceSchLegacyPlugin = "KICAD_SCH_LEGACY_PLUGIN";
const char* const traceSchPlugin = "KICAD_SCH_PLUGIN";
const char* const traceGedaPcbPlugin = "KICAD_GEDA_PLUGIN";
const char* const traceKicadPcbPlugin = "KICAD_PCB_PLUGIN";
const char* const tracePrinting = "KICAD_PRINT";
const char* const traceAutoSave = "KICAD_AUTOSAVE";
const char* const tracePathsAndFiles = "KICAD_PATHS_AND_FILES";
const char* const traceLocale = "KICAD_LOCALE";
const char* const traceFonts = "KICAD_FONTS";
const char* const traceScreen = "KICAD_SCREEN";
const char* const traceZoomScroll = "KICAD_ZOOM_SCROLL";
const char* const traceSymbolResolver = "KICAD_SYM_RESOLVE";
const char* const traceDisplayLocation = "KICAD_DISPLAY_LOCATION";
const char* const traceSchSheetPaths = "KICAD_SCH_SHEET_PATHS";
const char* const traceEnvVars = "KICAD_ENV_VARS";
const char* const traceGalProfile = "KICAD_GAL_PROFILE";
const char* const traceKiCad2Step = "KICAD2STEP";
const char* const traceUiProfile = "KICAD_UI_PROFILE";
const char* const traceGit = "KICAD_GIT";
const char* const traceEagleIo = "KICAD_EAGLE_IO";
const char* const traceDesignBlocks = "KICAD_DESIGN_BLOCK";


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
        case Qt::Key_Backspace: return "BACK";
        case Qt::Key_Tab: return "TAB";
        case Qt::Key_Return: return "RETURN";
        case Qt::Key_Escape: return "ESCAPE";
        case Qt::Key_Space: return "SPACE";
        case Qt::Key_Delete: return "DELETE";
        // Qt doesn't have direct equivalents for START, LBUTTON, RBUTTON, MBUTTON
        // These would typically be handled via mouse events in Qt
        case Qt::Key_Cancel: return "CANCEL";
        case Qt::Key_Clear: return "CLEAR";
        case Qt::Key_Shift: return "SHIFT";
        case Qt::Key_Alt: return "ALT";
        case Qt::Key_Control: return "CONTROL";
        case Qt::Key_Menu: return "MENU";
        case Qt::Key_Pause: return "PAUSE";
        case Qt::Key_CapsLock: return "CAPITAL";
        case Qt::Key_End: return "END";
        case Qt::Key_Home: return "HOME";
        case Qt::Key_Left: return "LEFT";
        case Qt::Key_Up: return "UP";
        case Qt::Key_Right: return "RIGHT";
        case Qt::Key_Down: return "DOWN";
        case Qt::Key_Select: return "SELECT";
        case Qt::Key_Print: return "PRINT";
        case Qt::Key_Execute: return "EXECUTE";
        // Qt uses Key_Print for snapshot/print screen
        case Qt::Key_Insert: return "INSERT";
        case Qt::Key_Help: return "HELP";
        // Numpad keys
        case Qt::Key_0: return "NUMPAD0";
        case Qt::Key_1: return "NUMPAD1";
        case Qt::Key_2: return "NUMPAD2";
        case Qt::Key_3: return "NUMPAD3";
        case Qt::Key_4: return "NUMPAD4";
        case Qt::Key_5: return "NUMPAD5";
        case Qt::Key_6: return "NUMPAD6";
        case Qt::Key_7: return "NUMPAD7";
        case Qt::Key_8: return "NUMPAD8";
        case Qt::Key_9: return "NUMPAD9";
        case Qt::Key_Asterisk: return "MULTIPLY";
        case Qt::Key_Plus: return "ADD";
        // Qt doesn't have a specific separator key
        case Qt::Key_Minus: return "SUBTRACT";
        case Qt::Key_Period: return "DECIMAL";
        case Qt::Key_Slash: return "DIVIDE";
        // Function keys
        case Qt::Key_F1: return "F1";
        case Qt::Key_F2: return "F2";
        case Qt::Key_F3: return "F3";
        case Qt::Key_F4: return "F4";
        case Qt::Key_F5: return "F5";
        case Qt::Key_F6: return "F6";
        case Qt::Key_F7: return "F7";
        case Qt::Key_F8: return "F8";
        case Qt::Key_F9: return "F9";
        case Qt::Key_F10: return "F10";
        case Qt::Key_F11: return "F11";
        case Qt::Key_F12: return "F12";
        case Qt::Key_F13: return "F13";
        case Qt::Key_F14: return "F14";
        case Qt::Key_F15: return "F15";
        case Qt::Key_F16: return "F16";
        case Qt::Key_F17: return "F17";
        case Qt::Key_F18: return "F18";
        case Qt::Key_F19: return "F19";
        case Qt::Key_F20: return "F20";
        case Qt::Key_F21: return "F21";
        case Qt::Key_F22: return "F22";
        case Qt::Key_F23: return "F23";
        case Qt::Key_F24: return "F24";
        case Qt::Key_NumLock: return "NUMLOCK";
        case Qt::Key_ScrollLock: return "SCROLL";
        case Qt::Key_PageUp: return "PAGEUP";
        case Qt::Key_PageDown: return "PAGEDOWN";
        // Qt doesn't have specific numpad versions for all keys
        // We'll handle the basic numpad enter
        case Qt::Key_Enter: return "NUMPAD_ENTER";
        // Qt uses Key_Home, Key_End, etc. for both regular and numpad versions
        // Windows-specific keys
        case Qt::Key_Meta: return "WINDOWS_LEFT";
        case Qt::Key_Super_L: return "WINDOWS_LEFT";
        case Qt::Key_Super_R: return "WINDOWS_RIGHT";

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

        std::string key = aWhat.toStdString();
        if( m_enabledTraces.find( key ) == m_enabledTraces.end() )
            return false;
    }

    return true;
}


void TRACE_MANAGER::Trace( const QString& aWhat, const char* aFmt, ... )
{
    va_list argptr;
    va_start( argptr, aFmt );
    traceV( aWhat, QString(aFmt), argptr );
    va_end( argptr );
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
        std::string key = token.toStdString();
        m_enabledTraces[key] = true;

        if( token.toLower() == "all" )
            m_printAllTraces = true;
    }
}