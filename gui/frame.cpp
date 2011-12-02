////////////////////////////////////////////////////////////////////
// Name:		implementation of the CGUIFrame class 
// File:		mainframe.cpp
// Purpose:		GUI mainframe system control methods
//
// Based on source code of Larry Lart
////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation
#pragma interface
#endif

#include "stdwx.h"

#include "wx/statline.h"
#include "wx/minifram.h"
#include "wx/thread.h"

#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif //precompiled headers

// other includes
#include "about.h"
#include "../wxMulticam.h"
#include "camview.h"
#include "camera/camera.h"
#include "../worker.h"

#include "../res/camera.xpm"

// main header
#include "frame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CGUIFrame

BEGIN_EVENT_TABLE(CGUIFrame, wxFrame)
    // menu items
//     EVT_MENU( wxID_CAMSRC, CGUIFrame::OnVideoSource )
//     EVT_MENU( wxID_CAMFORMAT, CGUIFrame::OnVideoFormat )
    EVT_MENU( wxID_ABOUT, CGUIFrame::OnAbout )
    EVT_MENU( wxID_EXIT, CGUIFrame::OnExit )
END_EVENT_TABLE()

////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CGUIFrame
// Purpose:	build my MainFrame object  
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CGUIFrame::CGUIFrame( wxFrame *frame, const wxString& title, 
        const wxPoint& pos, const wxSize& size ):
    wxFrame(frame, -1, title, pos, size, wxSYSTEM_MENU | wxMAXIMIZE_BOX | 
        wxMINIMIZE_BOX |wxCLOSE_BOX| wxCAPTION |wxRESIZE_BORDER )
{

    wxIcon icon( camera_xpm );
    SetIcon( icon );

    // set bg color for my frame 
    SetBackgroundColour( *wxLIGHT_GREY );

    // create status bar
    CreateStatusBar( 2 );

    // make File menu bar
    wxMenu *pFileMenu = new wxMenu( "", wxMENU_TEAROFF );
    pFileMenu->Append( wxID_ABOUT, "About" );
    pFileMenu->AppendSeparator( );
    pFileMenu->Append( wxID_EXIT, "Exit" );

    // make Camera menu bar
//     wxMenu *pVisionMenu = new wxMenu;
//     pVisionMenu->Append( wxID_CAMSRC, "Camera Source" );
//     pVisionMenu->Append( wxID_CAMFORMAT, "Camera Format" );
    
    // make menu toolbar
    menuBar = new wxMenuBar;
    menuBar->Append( pFileMenu, "File" );
//    menuBar->Append( pVisionMenu, "Camera");
    this->SetMenuBar(menuBar);
    
    // get client size 
    int width, height;
    this->GetClientSize(&width, &height);

    m_pMainPanel = new wxPanel(this, -1, wxPoint(0,0), wxSize(width,height), 0 );
    // build static/logical boxes

    // build static/logical boxes
    wxStaticBox *pCameraBox = new wxStaticBox( m_pMainPanel, -1, "camera", 
                            wxPoint(2,0), wxSize(360,275) );

    // get my main static sizer by the box
    wxStaticBoxSizer *pMainSizer = new wxStaticBoxSizer( pCameraBox, wxHORIZONTAL );
    
    // build cam canvas
    m_pCamView = new CCamView( m_pMainPanel, wxPoint(5,15), wxSize(354, 256) );
    pMainSizer->Add( m_pCamView, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxEXPAND );

    m_pMainPanel->SetSizer( pMainSizer );
    pMainSizer->SetSizeHints( m_pMainPanel );	
    m_pMainPanel->SetAutoLayout( TRUE );


    wxBoxSizer *pTopSizer = new wxBoxSizer(wxVERTICAL);
    pTopSizer->Add( m_pMainPanel, 1, wxALIGN_CENTER|wxEXPAND, 0 );
    SetSizer( pTopSizer );
    
    // display my stuff
    SetAutoLayout( TRUE );

}

////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CGUIFrame
// Purpose:	delete/destroy GUI MainFrame object  
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CGUIFrame::~CGUIFrame( )
{
    // tell to the worker to pause camera and exit
    m_pWorker->Stop( );

    // clean on exit
    delete m_pCamView;
    delete m_pMainPanel;

    // set to null
    m_pApp->m_pCameraWorker = NULL;
    m_pMainPanel = NULL;
    m_pCamView = NULL;
    m_pWorker = NULL;
    m_pApp = NULL;
}

////////////////////////////////////////////////////////////////////
// Method:	SetParentApp
// Class:	CGUIFrame
// Purpose:	Set/propagate pointer to parent application here 
// Input:	pointer to parent app
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CGUIFrame::SetParentApp( wxMultiCamApp *pApp )
{
    m_pApp = pApp;
}

////////////////////////////////////////////////////////////////////
// Method:	Get Camera view
// Class:	CGUIFrame
// Purpose:	return pointer to camera view
// Input:	nothing
// Output:	pointer to camera view
////////////////////////////////////////////////////////////////////
CCamView* CGUIFrame::GetCameraView()
{
    return( m_pCamView );
}

////////////////////////////////////////////////////////////////////
// Method:		On About
// Class:		CGUIFrame
// Purpose:		display about dialog
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnAbout( wxCommandEvent& event )
{
    CAbout dialog(this);
    dialog.ShowModal();

    return;
}

////////////////////////////////////////////////////////////////////
// Method:	On Video Source
// Class:	CGUIFrame
// Purpose:	when video source menu option selected
// Input:	reference to event
// Output:	nothing
////////////////////////////////////////////////////////////////////
// void CGUIFrame::OnVideoSource( wxCommandEvent& event )
// {
//     CCamView *pView = GetCameraView();
// 
// for widnows do this
// #ifdef _WINDOWS
// 	// hack to get the windows handler
// 	typedef struct CvCaptureCAM_VFW
// 	{
// 		void* vtable;
// 		CAPDRIVERCAPS caps;
// 		HWND   capWnd;
// 		VIDEOHDR* hdr;
// 		DWORD  fourcc;
// 		HIC    hic;
// 		IplImage* rgb_frame;
// 		IplImage frame;
// 	} CvCaptureCAM_VFW;
// 
// 
// 	CvCapture* ptest = pView->m_pCamera->m_pCapture;
// 	CvCaptureCAM_VFW* p = (CvCaptureCAM_VFW*) ptest;
// 
// 	capDlgVideoSource( p->capWnd );
// #elif _LINUX
// 	// else ... TODO
// #endif
// 
//     return;
// }

////////////////////////////////////////////////////////////////////
// Method:	On Video Format
// Class:	CGUIFrame
// Purpose:	when video format menu option selected
// Input:	reference to event
// Output:	nothing
////////////////////////////////////////////////////////////////////
// void CGUIFrame::OnVideoFormat( wxCommandEvent& event )
// {
//     CCamView *pView = GetCameraView();
// 
// for widnows do this
// #ifdef _WINDOWS
// 	// hack to get the windows handler
// 	typedef struct CvCaptureCAM_VFW
// 	{
// 		void* vtable;
// 		CAPDRIVERCAPS caps;
// 		HWND   capWnd;
// 		VIDEOHDR* hdr;
// 		DWORD  fourcc;
// 		HIC    hic;
// 		IplImage* rgb_frame;
// 		IplImage frame;
// 	} CvCaptureCAM_VFW;
// 
// 
// 	CvCapture* ptest = pView->m_pCamera->m_pCapture;
// 	CvCaptureCAM_VFW* p = (CvCaptureCAM_VFW*) ptest;
// 
// 	// first pause camera
// 	pView->m_pCamera->PauseResume( );
// 
// 	capDlgVideoFormat( p->capWnd );
// 
// 	// mark change and wait for thread to restart camera
// 	pView->m_pCamera->IsChanged( );
// 
// #elif _LINUX
// 	// else ... TODO
// #endif
// 
//     return;
// }

////////////////////////////////////////////////////////////////////
// Method:	SetStatusBarText
// Class:	CGUIFrame
// Purpose:	when minimize
// Input:	text to display
// Input:	pane index (zero-based)
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CGUIFrame::SetStatusBarText( const wxString& strText, int number )
{
    wxStatusBar* pStsBar = GetStatusBar();
    if( pStsBar )
    {
        wxString strOldText = pStsBar->GetStatusText( number );
        if( strOldText != strText )
            pStsBar->SetStatusText( strText, number );
    }
    

    return;
}

////////////////////////////////////////////////////////////////////
// Method:	ResetLayout
// Class:	CGUIFrame
// Purpose:	tell all components to reset layout 
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CGUIFrame::ResetLayout( )
{
    GetSizer()->Layout( );
    m_pMainPanel->Layout();
}

////////////////////////////////////////////////////////////////////
// Method:		On Exit
// Class:		CGUIFrame
// Purpose:		exit program
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnExit( wxCommandEvent& WXUNUSED(pEvent) )
{
    Close( );
    return;
}
