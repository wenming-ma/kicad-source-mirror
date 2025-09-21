

#include <eda_item.h>
#include <bitmaps.h>
#include <class_draw_panel_gal.h>
#include <dialogs/dialog_layers_select_to_pcb.h>
#include <gestfich.h>
#include <gerber_file_image.h>
#include <gerbview_id.h>
#include "gerbview_inspection_tool.h"
#include "gerbview_actions.h"
#include <gal/painter.h>
#include <pgm_base.h>
#include <preview_items/ruler_item.h>
#include <preview_items/selection_area.h>
#include <tool/tool_event.h>
#include <tool/tool_manager.h>
#include <view/view.h>
#include <view/view_controls.h>
#include <view/view_group.h>
#include <QMessageBox>
#include <QInputDialog>
#include <QDialog>
#include <QListWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QFileInfo>


GERBVIEW_INSPECTION_TOOL::GERBVIEW_INSPECTION_TOOL() :
        TOOL_INTERACTIVE( "gerbview.Inspection" ),
        m_frame( nullptr )
{
}


GERBVIEW_INSPECTION_TOOL::~GERBVIEW_INSPECTION_TOOL()
{
}


bool GERBVIEW_INSPECTION_TOOL::Init()
{
    return true;
}


void GERBVIEW_INSPECTION_TOOL::Reset( RESET_REASON aReason )
{
    m_frame = getEditFrame<GERBVIEW_FRAME>();
}


int GERBVIEW_INSPECTION_TOOL::ShowDCodes( const TOOL_EVENT& aEvent )
{
    int             ii, jj;
    QString        Line;
    QStringList   list;
    int             curr_layer = m_frame->GetActiveLayer();

    double   scale = 1.0;
    QString units;

    switch( m_frame->GetUserUnits() )
    {
    case EDA_UNITS::MM:
        scale = gerbIUScale.IU_PER_MM;
        units = "mm";
        break;

    case EDA_UNITS::INCH:
        scale = gerbIUScale.IU_PER_MILS * 1000;
        units = "in";
        break;

    case EDA_UNITS::MILS:
        scale = gerbIUScale.IU_PER_MILS;
        units = "mil";
        break;

    default:
        Q_ASSERT_X( false, Q_FUNC_INFO, "Invalid units" );
    }

    for( unsigned int layer = 0; layer < m_frame->ImagesMaxCount(); ++layer )
    {
        GERBER_FILE_IMAGE* gerber = m_frame->GetGbrImage( layer );

        if( !gerber )
            continue;

        if( gerber->GetDcodesCount() == 0 )
            continue;

        if( curr_layer == static_cast<int>( layer ) )
            Line = QString::asprintf( "*** Active layer (%2.2d) ***", layer + 1 );
        else
            Line = QString::asprintf( "*** layer %2.2d  ***", layer + 1 );

        list.append( Line );

        for( ii = 0, jj = 1; ii < TOOLS_MAX_COUNT; ii++ )
        {
            D_CODE* pt_D_code = gerber->GetDCODE( ii + FIRST_DCODE );

            if( pt_D_code == nullptr )
                continue;

            if( !pt_D_code->m_InUse && !pt_D_code->m_Defined )
                continue;

            Line = QString::asprintf( "tool %2.2d:   D%2.2d   V %.4f %s  H %.4f %s   %s  attribute '%s'",
                         jj,
                         pt_D_code->m_Num_Dcode,
                         pt_D_code->m_Size.y / scale, qPrintable(units),
                         pt_D_code->m_Size.x / scale, qPrintable(units),
                         qPrintable(D_CODE::ShowApertureType( pt_D_code->m_ApertType )),
                         pt_D_code->m_AperFunction.isEmpty()? "none" : qPrintable(pt_D_code->m_AperFunction)
                         );

            if( !pt_D_code->m_Defined )
                Line += " (not defined)";

            if( pt_D_code->m_InUse )
                Line += " (in use)";

            list.append( Line );
            jj++;
        }
    }

    // Create a simple dialog to display D Codes
    QDialog dlg( m_frame );
    dlg.setWindowTitle( "D Codes" );
    dlg.setModal( true );

    QVBoxLayout* layout = new QVBoxLayout( &dlg );
    QListWidget* listWidget = new QListWidget( &dlg );

    for( const QString& item : list )
    {
        listWidget->addItem( item );
    }

    layout->addWidget( listWidget );

    QDialogButtonBox* buttonBox = new QDialogButtonBox( QDialogButtonBox::Ok, &dlg );
    layout->addWidget( buttonBox );

    QObject::connect( buttonBox, &QDialogButtonBox::accepted, &dlg, &QDialog::accept );

    dlg.exec();

    return 0;
}


int GERBVIEW_INSPECTION_TOOL::ShowSource( const TOOL_EVENT& aEvent )
{
    int                layer        = m_frame->GetActiveLayer();
    GERBER_FILE_IMAGE* gerber_layer = m_frame->GetGbrImage( layer );

    if( gerber_layer )
    {
        QString editorname = Pgm().GetTextEditor();

        if( !editorname.isEmpty() )
        {
            QFileInfo fn( gerber_layer->m_FileName );

            // Call the editor only if the Gerber/drill source file is available.
            // This is not always the case, because it can be a temporary file
            // if it comes from a zip archive.
            if( !fn.exists() )
            {
                QString msg = QString( "Source file '%1' not found." ).arg( fn.absoluteFilePath() );
                QMessageBox::information( m_frame, "Information", msg );
            }
            else
            {
                ExecuteFile( editorname, fn.absoluteFilePath() );
            }
        }
        else
        {
            QMessageBox::information( m_frame, "Information", "No text editor selected in KiCad.  Please choose one." );
        }
    }
    else
    {
        QString msg = QString( "No file loaded on the active layer %1." ).arg( layer + 1 );
        QMessageBox::information( m_frame, "Information", msg );
    }

    return 0;
}


using KIGFX::PREVIEW::TWO_POINT_GEOMETRY_MANAGER;


int GERBVIEW_INSPECTION_TOOL::MeasureTool( const TOOL_EVENT& aEvent )
{
    KIGFX::VIEW_CONTROLS&      controls = *getViewControls();
    bool                       originSet = false;
    TWO_POINT_GEOMETRY_MANAGER twoPtMgr;
    EDA_UNITS                  units = m_frame->GetUserUnits();
    KIGFX::PREVIEW::RULER_ITEM ruler( twoPtMgr, gerbIUScale, units, false, false );

    m_frame->PushTool( aEvent );

    auto setCursor =
            [&]()
            {
                m_frame->GetCanvas()->SetCurrentCursor( KICURSOR::MEASURE );
            };

    auto cleanup =
            [&] ()
            {
                getView()->SetVisible( &ruler, false );
                controls.SetAutoPan( false );
                controls.CaptureCursor( false );
                originSet = false;
            };

    Activate();
    // Must be done after Activate() so that it gets set into the correct context
    controls.ShowCursor( true );
    // Set initial cursor
    setCursor();

    getView()->Add( &ruler );
    getView()->SetVisible( &ruler, false );

    while( TOOL_EVENT* evt = Wait() )
    {
        setCursor();
        const VECTOR2I cursorPos = controls.GetCursorPosition();

        if( evt->IsCancelInteractive() )
        {
            if( originSet )
            {
                cleanup();
            }
            else
            {
                m_frame->PopTool( aEvent );
                break;
            }
        }
        else if( evt->IsActivate() )
        {
            if( originSet )
                cleanup();

            if( evt->IsMoveTool() )
            {
                // leave ourselves on the stack so we come back after the move
                break;
            }
            else
            {
                m_frame->PopTool( aEvent );
                break;
            }
        }
        else if( !originSet && ( evt->IsDrag( BUT_LEFT ) || evt->IsClick( BUT_LEFT ) ) )
        {
            // click or drag starts
            twoPtMgr.SetOrigin( cursorPos );
            twoPtMgr.SetEnd( cursorPos );

            controls.CaptureCursor( true );
            controls.SetAutoPan( true );

            originSet = true;
        }
        else if( originSet && ( evt->IsClick( BUT_LEFT ) || evt->IsMouseUp( BUT_LEFT ) ) )
        {
            // second click or mouse up after drag ends
            originSet = false;

            controls.SetAutoPan( false );
            controls.CaptureCursor( false );
        }
        else if( originSet && ( evt->IsMotion() || evt->IsDrag( BUT_LEFT ) ) )
        {
            // move or drag when origin set updates rules
            twoPtMgr.SetAngleSnap( evt->Modifier( MD_SHIFT ) );
            twoPtMgr.SetEnd( cursorPos );

            getView()->SetVisible( &ruler, true );
            getView()->Update( &ruler, KIGFX::GEOMETRY );
        }
        else if( evt->IsAction( &ACTIONS::updateUnits ) )
        {
            if( m_frame->GetUserUnits() != units )
            {
                units = m_frame->GetUserUnits();
                ruler.SwitchUnits( units );
                getView()->Update( &ruler, KIGFX::GEOMETRY );
            }
            evt->SetPassEvent();
        }
        else if( evt->IsClick( BUT_RIGHT ) )
        {
            m_menu->ShowContextMenu( m_frame->GetCurrentSelection() );
        }
        else
        {
            evt->SetPassEvent();
        }
    }

    getView()->SetVisible( &ruler, false );
    getView()->Remove( &ruler );

    m_frame->GetCanvas()->SetCurrentCursor( KICURSOR::ARROW );
    return 0;
}


void GERBVIEW_INSPECTION_TOOL::setTransitions()
{
    Go( &GERBVIEW_INSPECTION_TOOL::ShowSource,     GERBVIEW_ACTIONS::showSource.MakeEvent() );
    Go( &GERBVIEW_INSPECTION_TOOL::ShowDCodes,     GERBVIEW_ACTIONS::showDCodes.MakeEvent() );
    Go( &GERBVIEW_INSPECTION_TOOL::MeasureTool,    ACTIONS::measureTool.MakeEvent() );
}
