////////////////////////////////////////////////////////////////////
// Name:		wxMulticam Main Routine
// File:		wxMulticam.cpp
// Purpose:		Main routine for wxMulticam
//
// Based on source code of Larry Lart
////////////////////////////////////////////////////////////////////

#include "stdwx.h"

// wxwindows headers
#include "wx/statline.h"
#include "wx/minifram.h"
#include "wx/thread.h"

// custom headers
#include "gui/frame.h"
#include "gui/camview.h"
#include "camera/camera.h"
#include "worker.h"

// include main header
#include "wxMulticam.h"

// Multicam header
#include "MultiCam/multicam.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_APP(wxMultiCamApp)

// ***** CLASS wxMultiCamApp *****
////////////////////////////////////////////////////////////////////
// events table
BEGIN_EVENT_TABLE(wxMultiCamApp, wxApp)
END_EVENT_TABLE()



void wxMultiCamApp::OpenMulticam()
{
    // Allocate Multicam driver
    MCSTATUS Status;
    Status = McOpenDriver(NULL);
    if(Status != MC_OK)
    {
        m_MulticamDriverOK = false;
        //FormatMulticamErrorText(Status, _T("McOpenDriver"), m_MulticamAllocationErrorMessage);
    }
    else
    {
        m_MulticamDriverOK = true;
        McSetParamInt(MC_CONFIGURATION, MC_ErrorHandling, MC_ErrorHandling_NONE);
    }
}

void wxMultiCamApp::CloseMulticam()
{
    //release Multicam
    if(m_MulticamDriverOK)
    {
        McCloseDriver();
        m_MulticamDriverOK = false;
    }
}
////////////////////////////////////////////////////////////////////
// Method:	On Init
// Class:	wxMultiCamApp
// Purpose:	initialize my application
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
bool wxMultiCamApp::OnInit( )
{
    OpenMulticam();

    // create camera object
    m_pCamera = new CCamera( );

    // create widows frame
    CGUIFrame* m_pFrame = new CGUIFrame( NULL, "wxMulticam Demo",
                         wxPoint(-1, -1), wxSize(640, 600) );

    // Show the frame
    m_pFrame->Show(TRUE);
    SetTopWindow(m_pFrame);

    // set parent
    m_pFrame->m_pApp = this;

    // build worker thread to process video stream
    m_pCameraWorker = new CCameraWorker( m_pFrame );
    // create thread or fail on exit
    if ( m_pCameraWorker->Create() != wxTHREAD_NO_ERROR )
    {
        wxExit( );
    }

    // exchange data if gui defined
    CCamView *pCamView = m_pFrame->GetCameraView();

    // link robot to GUI
    m_pFrame->m_pWorker = m_pCameraWorker;
    m_pCameraWorker->m_pCamera = m_pCamera;
    m_pCamera->m_pWorker = m_pCameraWorker;
    m_pCamera->m_pCameraView = pCamView;
    m_pCamera->m_pFrame = m_pFrame;
    
    if( pCamView )
        pCamView->SetSize( m_pCamera->m_nWidth, m_pCamera->m_nHeight );
    if( m_pFrame )
        m_pFrame->ResetLayout( );

    // initialize camera 
    if( m_pCamera->Init(  ) == 0 )
    {
        wxMessageBox( "Can't initialize camera. Try to change format",
                    "Error" );
    }


    // Link camera view to camera object
    pCamView->m_pCamera = m_pCamera;

    // start the thread
    if ( m_pCameraWorker->Run() != wxTHREAD_NO_ERROR )
    {
        wxExit( );
    } 

    return TRUE;
}

////////////////////////////////////////////////////////////////////
// Method:	OnExit
// Class:	wxMultiCamApp
// Purpose:	clean on app exit
// Input:	nothing
// Output:	status ?
////////////////////////////////////////////////////////////////////
int wxMultiCamApp::OnExit( )
{
    if( m_pCameraWorker != NULL )
    {
        m_pCameraWorker->Delete( );
    }
    m_pCameraWorker = NULL;

    delete( m_pCamera );

    CloseMulticam();

    return wxApp::OnExit();
}
