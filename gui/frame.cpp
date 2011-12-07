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

#include "wx/busyinfo.h"
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
    EVT_MENU( wxID_NET_START_SERVER, CGUIFrame::OnStartServer )
    EVT_MENU( wxID_NET_CONNECT_TO_SERVER, CGUIFrame::OnConnectToServer )
    EVT_MENU( wxID_NET_WAIT_CONNECT, CGUIFrame::OnWaitConnect )

    EVT_MENU( wxID_CAM_START, CGUIFrame::OnCameraStart )
    EVT_MENU( wxID_CAM_STOP, CGUIFrame::OnCameraStop )

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
        wxMINIMIZE_BOX |wxCLOSE_BOX| wxCAPTION |wxRESIZE_BORDER ),
    m_pCamera(NULL),
    m_pCamView(NULL),
    m_pWorker(NULL)
{

    wxIcon icon( camera_xpm );
    SetIcon( icon );

    // set bg color for my frame 
    SetBackgroundColour( *wxLIGHT_GREY );

    // create status bar
    CreateStatusBar( 2 );

    // make File menu bar
    wxMenu *pFileMenu = new wxMenu( _T(""), wxMENU_TEAROFF );
    pFileMenu->Append( wxID_ABOUT, "&About" );
    pFileMenu->AppendSeparator( );
    pFileMenu->Append( wxID_EXIT, "E&xit" );

    // make Camera menu bar
    wxMenu *pCameraMenu = new wxMenu;
    pCameraMenu->Append( wxID_CAM_START, _T("S&tart") );
    pCameraMenu->Append( wxID_CAM_STOP, _T("Sto&p") );


    // make Net menu bar
    wxMenu *pNetMenu = new wxMenu;
    pNetMenu->Append( wxID_NET_START_SERVER, _T("S&tart server") );
    pNetMenu->Append( wxID_NET_WAIT_CONNECT, _T("&Wait for connection") );
    pFileMenu->AppendSeparator( );
    pNetMenu->Append( wxID_NET_CONNECT_TO_SERVER, _T("&Connect to server") );


    // make menu toolbar
    menuBar = new wxMenuBar;
    menuBar->Append( pFileMenu, "&File" );
    menuBar->Append( pCameraMenu, _T("&Camera") );
    menuBar->Append( pNetMenu, _T("&Net") );
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
    m_pCamView = new CCamView( m_pMainPanel, wxPoint(5,15), wxSize(640, 480) );
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
    CameraStop();

    // clean on exit
    delete m_pCamView;
    delete m_pMainPanel;
    
    // set to null
    m_pMainPanel = NULL;
    m_pCamView = NULL;
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


void CGUIFrame::OnServerEvent(wxSocketEvent& event)
{
    wxString s = _("OnServerEvent: ");
    wxSocketBase *sock;

    switch(event.GetSocketEvent())
    {
    case wxSOCKET_CONNECTION : s.Append(_("wxSOCKET_CONNECTION\n")); break;
    default                  : s.Append(_("Unexpected event !\n")); break;
    }

//    m_text->AppendText(s);

    // Accept new connection if there is one in the pending
    // connections queue, else exit. We use Accept(false) for
    // non-blocking accept (although if we got here, there
    // should ALWAYS be a pending connection).

    sock = m_server->Accept(false);

    if (sock)
    {
        IPaddress addr;
        if ( !sock->GetPeer(addr) )
        {
            wxLogMessage("New connection from unknown client accepted.");
        }
        else
        {
            wxLogMessage("New client connection from %s:%u accepted",
                addr.IPAddress(), addr.Service());
        }
    }
    else
    {
        wxLogMessage("Error: couldn't accept a new connection");
        return;
    }

    sock->SetEventHandler(*this, SOCKET_ID);
    sock->SetNotify(wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG);
    sock->Notify(true);

    //m_numClients++;
    //UpdateStatusBar();
}

void CGUIFrame::OnWaitConnect( wxCommandEvent& event )
{
//    TestLogger logtest("WaitForAccept() test");
    if( NULL == m_server )
        return;

    wxBusyInfo("Waiting for connection for 10 seconds...", this);
    if ( m_server->WaitForAccept(10) )
        wxLogMessage("Accepted client connection.");
    else
        wxLogMessage("Connection error or timeout expired.");

}

bool CGUIFrame::ServerCreate()
{
    // Create the address - defaults to localhost:0 initially
    IPaddress addr;
    addr.Service(3000);

    wxLogMessage("Creating server at %s:%u", addr.IPAddress(), addr.Service());

    // Create the socket
    m_server = new wxSocketServer(addr);

    // We use IsOk() here to see if the server is really listening
    if (! m_server->IsOk())
    {
        wxLogMessage("Could not listen at the specified port !");
        return false;
    }

    IPaddress addrReal;
    if ( !m_server->GetLocal(addrReal) )
    {
        wxLogMessage("ERROR: couldn't get the address we bound to");
    }
    else
    {
        wxLogMessage("Server listening at %s:%u",
            addrReal.IPAddress(), addrReal.Service());
    }

    // Setup the event handler and subscribe to connection events
    m_server->SetEventHandler(*this, SERVER_ID);
    m_server->SetNotify(wxSOCKET_CONNECTION_FLAG);
    m_server->Notify(true);

    return true;
}

void CGUIFrame::OnStartServer( wxCommandEvent& event )
{
    ServerCreate();
}

void CGUIFrame::OnConnectToServer( wxCommandEvent& event )
{
    if( m_server )
    {
        // delete server
        delete m_server;
    }
}

void CGUIFrame::OnCameraStart( wxCommandEvent& event )
{
    CameraStop();

//     if( NULL == m_pWorker )
//         return;
    wxASSERT( m_pCamera == NULL );
    wxASSERT( m_pWorker == NULL );

    // create camera object
    m_pCamera = new CCamera( this, m_pCamView );

    // build worker thread to process video stream
    m_pWorker = new CCameraWorker( this, m_pCamera );

    // create thread or fail on exit
    if ( m_pWorker->Create() != wxTHREAD_NO_ERROR )
    {
        wxExit( );
        return;
    }

    if( m_pWorker->IsRunning() )
        return;

    // initialize camera 
    if( m_pCamera->Init() == 0 )
    {
        wxMessageBox( "Can't initialize camera.",  "Error" );
        return;
    }

    // start the thread
    if ( m_pWorker->Run() != wxTHREAD_NO_ERROR )
    {
        wxExit();
        return;
    } 

}

void CGUIFrame::OnCameraStop( wxCommandEvent& event )
{
    CameraStop();
}

void CGUIFrame::CameraStop()
{
    // tell to the worker to pause camera and exit
    if( m_pWorker )
        m_pWorker->Stop( );
    m_pWorker = NULL;

    // destroy the camera object
    if( m_pCamera )
        delete m_pCamera;
    m_pCamera = NULL;
}
