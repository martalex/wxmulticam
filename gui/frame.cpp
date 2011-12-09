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
#include "camview.h"
#include "camera/camera.h"
#include "../wxMulticam.h"
#include "../worker.h"
#include "../res/camera.xpm"

// main header
#include "frame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const int SOCKET_CODE_FRAME_NUMBER = 0xBE; 
const int SOCKET_CODE_FRAME_DATA = 0xCE; 
const int SOCKET_CODE_GET_FRAME_DATA = 0xEF;

/////////////////////////////////////////////////////////////////////////////
// CGUIFrame

BEGIN_EVENT_TABLE(CGUIFrame, wxFrame)
    // menu items
    EVT_MENU( wxID_NET_CONNECT_TO_SERVER, CGUIFrame::OnConnectToServer )
    EVT_MENU( wxID_NET_GET_FRAME, CGUIFrame::OnNetGetFrame )
    EVT_MENU( wxID_NET_AUTO_GET_FRAME, CGUIFrame::OnNetAutoGetFrame )

    EVT_MENU( wxID_CAM_START, CGUIFrame::OnCameraStart )
    EVT_MENU( wxID_CAM_STOP, CGUIFrame::OnCameraStop )

    EVT_MENU( wxID_ABOUT, CGUIFrame::OnAbout )
    EVT_MENU( wxID_EXIT, CGUIFrame::OnExit )

    EVT_SOCKET(SERVER_ID,  CGUIFrame::OnServerEvent)
    EVT_SOCKET(SOCKET_ID,  CGUIFrame::OnSocketEvent)

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
    m_pWorker(NULL),

    m_server( NULL ),
    m_sock( NULL ),
    m_numClients(0),
    m_IsServer( false ),
    m_IsBusy( false ),
    m_AutoRepeat( false )
{

    wxIcon icon( camera_xpm );
    SetIcon( icon );

    // set bg color for my frame 
    SetBackgroundColour( *wxLIGHT_GREY );

    // create status bar
    CreateStatusBar( SBID_NOF );

    // make File menu bar
    wxMenu *pFileMenu = new wxMenu( _T(""), wxMENU_TEAROFF );
    pFileMenu->Append( wxID_ABOUT, _T("&About") );
    pFileMenu->AppendSeparator( );
    pFileMenu->Append( wxID_EXIT, _T("E&xit") );

    // make Camera menu bar
    m_menuCam = new wxMenu;
    m_menuCam->Append( wxID_CAM_START, _T("S&tart") );
    m_menuCam->Append( wxID_CAM_STOP, _T("Sto&p") );


    // make Net menu bar
    m_menuNet = new wxMenu;
    m_menuNet->Append( wxID_NET_CONNECT_TO_SERVER, _T("&Connect to server"), 
        _T("Display the remote image"), wxITEM_CHECK );
    m_menuNet->Append( wxID_NET_GET_FRAME, _T("&Get frame\tF5") );
    m_menuNet->Append( wxID_NET_AUTO_GET_FRAME, _T("&AutoRepeat"),
        _T("Get new frame automatically"), wxITEM_CHECK );

    // make menu toolbar
    menuBar = new wxMenuBar;
    menuBar->Append( pFileMenu, _T("&File") );
    menuBar->Append( m_menuCam, _T("&Camera") );
    menuBar->Append( m_menuNet, _T("&Net") );
    this->SetMenuBar(menuBar);
    
    // get client size 
    int width, height;
    this->GetClientSize(&width, &height);

    m_pMainPanel = new wxPanel(this, -1, wxPoint(0,0), wxSize(width,height), 0 );
    // build static/logical boxes

    // build static/logical boxes
    wxStaticBox *pCameraBox = new wxStaticBox( m_pMainPanel, -1, _T("camera"), 
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
    UpdateStatusBar();
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
    NetworkStop();

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
// Method:	SetStatusBarText
// Class:	CGUIFrame
// Purpose:	when minimize
// Input:	text to display
// Input:	pane index (zero-based)
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CGUIFrame::SetStatusBarText( const wxString& strText, int number )
{
    wxASSERT( number >=0 && number < SBID_NOF );

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
    if( m_IsServer && m_server != NULL )
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
            sock->SetEventHandler(*this, SOCKET_ID);
            sock->SetNotify(wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG);
            sock->Notify(true);

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

        m_numClients++;
        UpdateStatusBar();
    }
    else
    {
        switch ( event.GetSocketEvent() )
        {
        case wxSOCKET_INPUT:
            wxLogMessage(_T("Input available on the socket"));
            break;

        case wxSOCKET_LOST:
            wxLogMessage(_T("Socket connection was unexpectedly lost."));
            UpdateStatusBar();
            break;

        case wxSOCKET_CONNECTION:
            wxLogMessage(_T("... socket is now connected."));
            UpdateStatusBar();
            break;

        default:
            wxLogMessage(_T("Unknown socket event!!!"));
            break;
        }
    }
}

void CGUIFrame::OnSocketEvent(wxSocketEvent& event)
{
    if( m_IsServer && m_server != NULL )
    {
        wxString s = _("Server OnSocketEvent: ");
        wxSocketBase *sock = event.GetSocket();

        // First, print a message
        switch(event.GetSocketEvent())
        {
        case wxSOCKET_INPUT : s.Append(_("wxSOCKET_INPUT\n")); break;
        case wxSOCKET_LOST  : s.Append(_("wxSOCKET_LOST\n")); break;
        default             : s.Append(_("Unexpected event !\n")); break;
        }

        //m_text->AppendText(s);

        // Now we process the event
        switch(event.GetSocketEvent())
        {
        case wxSOCKET_INPUT:
            {
                // We disable input events, so that the test doesn't trigger
                // wxSocketEvent again.
                sock->SetNotify(wxSOCKET_LOST_FLAG);

                // Which test are we going to run?
                unsigned char c;
                sock->Read(&c, 1);

                switch (c)
                {
//                 case SOCKET_CODE_FRAME_NUMBER: //0xBE: 
//                     UpdateFrameNumber(sock); 
//                     break;
                 case SOCKET_CODE_GET_FRAME_DATA: //0xCE: 
                    //UpdateFrameData(sock); 
                    if( m_pCamera )
                        m_pCamera->SendFrame( sock );
                    break;
//                 case 0xDE: Test3(sock); break;
                default:
                    wxLogMessage( _T("Unknown test id received from client") );
                }

                // Enable input events again.
                sock->SetNotify(wxSOCKET_LOST_FLAG | wxSOCKET_INPUT_FLAG);
                break;
            }
        case wxSOCKET_LOST:
            {
                m_numClients--;

                // Destroy() should be used instead of delete wherever possible,
                // due to the fact that wxSocket uses 'delayed events' (see the
                // documentation for wxPostEvent) and we don't want an event to
                // arrive to the event handler (the frame, here) after the socket
                // has been deleted. Also, we might be doing some other thing with
                // the socket at the same time; for example, we might be in the
                // middle of a test or something. Destroy() takes care of all
                // this for us.

                wxLogMessage( _T("Deleting socket.") );
                sock->Destroy();
                break;
            }
        default: ;
        }

        UpdateStatusBar();
    }
    else if( !m_IsServer && m_sock )
    {
        bool bRes = false;
        wxString s = _("Client OnSocketEvent: ");
        wxSocketBase *sock = event.GetSocket();

        // First, print a message
        switch(event.GetSocketEvent())
        {
        case wxSOCKET_INPUT : s.Append(_("wxSOCKET_INPUT\n")); break;
        case wxSOCKET_LOST  : s.Append(_("wxSOCKET_LOST\n")); break;
        default             : s.Append(_("Unexpected event !\n")); break;
        }

        //m_text->AppendText(s);

        // Now we process the event
        switch(event.GetSocketEvent())
        {
        case wxSOCKET_INPUT:
            {
                // We disable input events, so that the test doesn't trigger
                // wxSocketEvent again.
                sock->SetNotify(wxSOCKET_LOST_FLAG);

                // Which test are we going to run?
                unsigned char c;
                sock->Read(&c, 1);

                switch (c)
                {
                    //                 case SOCKET_CODE_FRAME_NUMBER: //0xBE: 
                    //                     UpdateFrameNumber(sock); 
                    //                     break;
                case SOCKET_CODE_FRAME_DATA: //0xCE: 
                    bRes = UpdateFrameData(sock); 
                    break;
                    //                 case 0xDE: Test3(sock); break;
                default:
                    wxLogMessage( _T("Unknown test id received from client") );
                }

                // Enable input events again.
                sock->SetNotify(wxSOCKET_LOST_FLAG | wxSOCKET_INPUT_FLAG);

                if( bRes && m_AutoRepeat )
                    GetNextFrame();
                //wxMessageBox( _T("Update Next Frame is FALSE") );
                break;
            }
        case wxSOCKET_LOST:
            {
                // Destroy() should be used instead of delete wherever possible,
                // due to the fact that wxSocket uses 'delayed events' (see the
                // documentation for wxPostEvent) and we don't want an event to
                // arrive to the event handler (the frame, here) after the socket
                // has been deleted. Also, we might be doing some other thing with
                // the socket at the same time; for example, we might be in the
                // middle of a test or something. Destroy() takes care of all
                // this for us.

                wxLogMessage( _T("Deleting socket.") );
                sock->Destroy();
                m_sock = NULL;
                break;
            }
        default: ;
        }

        UpdateStatusBar();

    }
}

//void CGUIFrame::OnServerWaitConnect( wxCommandEvent& event )
bool CGUIFrame::WaitForConnection()
{
//    TestLogger logtest("WaitForAccept() test");
/*    if( NULL == m_server )
        return false;

    wxBusyInfo("Waiting for connection for 10 seconds...", this);
    if ( m_server->WaitForAccept(10) )
        wxLogMessage("Accepted client connection.");
    else
        wxLogMessage("Connection error or timeout expired.");
*/
    return true;
}

bool CGUIFrame::CreateServer()
{
    if( NULL != m_server )
    {
        wxMessageBox( _T("Server is already created"), _T("wxMulticam") );
        return true;
    }


    // Create the address - defaults to localhost:0 initially
    IPaddress addr;
    addr.Service(3000);

    wxLogMessage( _T("Creating server at %s:%u"), addr.IPAddress(), addr.Service());

    // Create the socket
    m_server = new wxSocketServer(addr);

    // We use IsOk() here to see if the server is really listening
    if (! m_server->IsOk())
    {
        wxLogMessage( _T("Could not listen at the specified port !") );
        return false;
    }

    IPaddress addrReal;
    if ( !m_server->GetLocal(addrReal) )
    {
        wxLogMessage( _T("ERROR: couldn't get the address we bound to") );
    }
    else
    {
        wxLogMessage( _T("Server listening at %s:%u"),
            addrReal.IPAddress(), addrReal.Service());
    }

    // Setup the event handler and subscribe to connection events
    m_server->SetEventHandler(*this, SERVER_ID);
    m_server->SetNotify(wxSOCKET_CONNECTION_FLAG);
    m_server->Notify(true);

    m_IsServer = true;
    UpdateStatusBar();

    return true;
}

// void CGUIFrame::OnServerStart( wxCommandEvent& event )
// {
//     if( CreateServer() )
//         WaitForConnection();
// }

void CGUIFrame::OnConnectToServer( wxCommandEvent& event )
{
    wxASSERT( m_server == NULL );
    CreateClient();

    if( m_sock )
        if( m_sock->IsDisconnected() )
        {
            OpenConnection();
            GetNextFrame();
        }
        else
            Disconnect();
}

void CGUIFrame::OnNetGetFrame( wxCommandEvent& event )
{
    GetNextFrame();
}

void CGUIFrame::OnNetAutoGetFrame( wxCommandEvent& event )
{
    m_AutoRepeat = !m_AutoRepeat;
    if( m_AutoRepeat )
        GetNextFrame();
}

void CGUIFrame::Disconnect()
{
    if( m_sock && m_sock->IsConnected() )
        m_sock->Close();
}

void CGUIFrame::OpenConnection()
{
    wxIPaddress * addr;
    wxIPV4address addr4;
#if wxUSE_IPV6
    wxIPV6address addr6;
    if ( family == wxSockAddress::IPV6 )
        addr = &addr6;
    else
#endif
        addr = &addr4;

    // Ask user for server address
    wxString hostname = wxGetTextFromUser(
        _("Enter the address of the wxMulticam server:"),
        _("Connect ..."),
        _("localhost"));
    if ( hostname.empty() )
        return;

    addr->Hostname(hostname);
    addr->Service(3000);

    // we connect asynchronously and will get a wxSOCKET_CONNECTION event when
    // the connection is really established
    //
    // if you want to make sure that connection is established right here you
    // could call WaitOnConnect(timeout) instead
    //wxLogMessage( _T("Trying to connect to %s:%d"), hostname, addr->Service());

    m_sock->Connect(*addr, false);
    
    bool waitMore = true;
    while( !m_sock->WaitOnConnect( 20 ) && waitMore )
    {
        wxLogMessage( _T("Cannot connect to %s:%d"), hostname, addr->Service());

        if( wxMessageBox( _T("Try to reconnect?"), _T("Connect to the server"), wxYES_NO ) != wxYES )
        {
            waitMore = false;
            break;
        }
    }

    UpdateStatusBar();
}

void CGUIFrame::OnCameraStart( wxCommandEvent& event )
{
    CameraStop();

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
        delete m_pCamera;
        m_pCamera = NULL;

        wxMessageBox( _T("Can't initialize camera."),  _T("Error") );
        return;
    }

    // start the thread
    if ( m_pWorker->Run() != wxTHREAD_NO_ERROR )
    {
        wxExit();
        return;
    } 

    // Start the server after successful initialization the grabber (camera)
    if( CreateServer() )
        WaitForConnection();
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

void CGUIFrame::NetworkStop()
{
    if( NULL != m_server )
    {
        m_server->Close();
        delete m_server;
    }
    m_server = NULL;

    if( NULL != m_sock )
    {
        m_sock->Close();
        delete m_sock;
    }
    m_sock = NULL;

    m_IsServer = false;
}

bool CGUIFrame::CreateClient()
{
    if( m_sock )
        return true;

    // Create the socket
    m_sock = new wxSocketClient();

    // Setup the event handler and subscribe to most events
    m_sock->SetEventHandler(*this, SOCKET_ID);
    m_sock->SetNotify(wxSOCKET_CONNECTION_FLAG |
        wxSOCKET_INPUT_FLAG |
        wxSOCKET_LOST_FLAG);
    m_sock->Notify(true);

    m_IsServer = false;
    UpdateStatusBar();

    return true;
}

void CGUIFrame::UpdateStatusBar()
{
#if wxUSE_STATUSBAR
    wxString s;

    if( m_IsServer )
    {
        if( m_server == NULL )
            s = _T("Server is not running");
        else
            s.Printf( _T("%d clients connected"), m_numClients );
    }
    else
    {
        if( m_sock != NULL )
        {
            if (!m_sock->IsConnected())
            {
                s = _T("Not connected");
            }
            else
            {
#if wxUSE_IPV6
                wxIPV6address addr;
#else
                wxIPV4address addr;
#endif

                m_sock->GetPeer(addr);
                s.Printf( _T("%s : %d"), addr.Hostname(), addr.Service());
            }
        }
    }

    SetStatusText(s, SBID_NETWORK_STATUS);
#endif // wxUSE_STATUSBAR

    m_menuCam->Enable( wxID_CAM_START, m_pCamera == NULL );
    m_menuCam->Enable( wxID_CAM_STOP, m_pCamera != NULL );

    m_menuNet->Enable( wxID_NET_CONNECT_TO_SERVER, m_server == NULL );
    m_menuNet->Check( wxID_NET_CONNECT_TO_SERVER, m_sock && m_sock->IsConnected() );
    m_menuNet->Enable( wxID_NET_GET_FRAME, m_sock && m_sock->IsConnected() );
    m_menuNet->Enable( wxID_NET_AUTO_GET_FRAME, m_sock && m_sock->IsConnected() );
    m_menuNet->Check( wxID_NET_AUTO_GET_FRAME, m_sock && m_sock->IsConnected() && m_AutoRepeat );
}


void CGUIFrame::SendFrameNumber( int number )
{
    if( m_sock && m_sock->IsConnected() )
    {
        m_IsBusy = true;

        // Tell the server which test we are running
        unsigned char c = SOCKET_CODE_FRAME_NUMBER;
        m_sock->Write(&c, 1);

        m_sock->SetFlags(wxSOCKET_WAITALL);

        m_sock->Write( &number, sizeof(number) );

        int numberRead=0;
        m_sock->Read( &numberRead, sizeof( numberRead ) );
        if( numberRead != number )
        {
            m_sock->Close();
            wxMessageBox( _T(" Frame number: numberRead != number ") );
        }

        m_IsBusy = false;
    }
}

void CGUIFrame::SendFrameData( wxSocketBase *sock, BYTE* pImg, int w, int h, int pxs )
{
    if( !m_IsBusy )
    {
        m_IsBusy = true;

        if( sock && sock->IsConnected() )
        {
            // Tell the server which test we are running
            unsigned char c = SOCKET_CODE_FRAME_DATA;
            sock->Write(&c, 1);

            sock->SetFlags(wxSOCKET_WAITALL);

            // Write image width
            sock->Write( &w, sizeof(w) );
            // Write image height
            sock->Write( &h, sizeof(h) );
            // Write image pixel size
            sock->Write( &pxs, sizeof(pxs) );

            // Write image frame data
            static BYTE* pFrameData = NULL;
            if( NULL == pFrameData )
            {
                pFrameData = new BYTE [w*h];
            }
            for( int r=0; r<h; ++r )
                for( int c=0; c<w; ++c )
                   pFrameData[r*w+c] = pImg[r*w*pxs + c*pxs];

            sock->Write( pFrameData, w*h );

            int numberRead=0;
            sock->Read( &numberRead, sizeof( numberRead ) );
            if( numberRead != w*h )
            {
                sock->Close();
                wxMessageBox( _T(" Frame data: numberRead != w*h ") );
            }
        }
        
        m_IsBusy = false;
    }
}

void CGUIFrame::UpdateFrameNumber( wxSocketBase *sock )
{
    // Receive data from socket and send it back. We will first
    // get a byte with the buffer size, so we can specify the
    // exact size and use the wxSOCKET_WAITALL flag. Also, we
    // disabled input events so we won't have unwanted reentrance.
    // This way we can avoid the infamous wxSOCKET_BLOCK flag.

    sock->SetFlags(wxSOCKET_WAITALL);

    // Read the size
    int totalFrames;
    sock->Read( &totalFrames, sizeof(int) );

    wxString text;
    text << _T("Received frames: ") << totalFrames;
    SetStatusText( text, SBID_FRAMES );

    sock->Write( &totalFrames, sizeof(int) );
}

bool CGUIFrame::UpdateFrameData( wxSocketBase *sock )
{
    if( !sock || !sock->IsConnected() )
        return false;

    // Receive data from socket and send it back. We will first
    // get a byte with the buffer size, so we can specify the
    // exact size and use the wxSOCKET_WAITALL flag. Also, we
    // disabled input events so we won't have unwanted reentrance.
    // This way we can avoid the infamous wxSOCKET_BLOCK flag.

    sock->SetFlags(wxSOCKET_WAITALL);

    // Read the image width, height and pixel size
    int w, h, pxs;
    sock->Read( &w, sizeof(int) );
    sock->Read( &h, sizeof(int) );
    sock->Read( &pxs, sizeof(int) );

    // Read the frame image data
    static BYTE* pFrameData = NULL;
    if( NULL == pFrameData )
        pFrameData = new BYTE [w*h];
    sock->Read( pFrameData, w*h );

    int numberRead=w*h;
    sock->Write( &numberRead, sizeof(int) );

    static BYTE* pVideoImg = NULL;
    if( NULL == pVideoImg )
        pVideoImg = new BYTE [w*h*pxs];

    for( int r=0; r<h; ++r )
        for( int c=0; c<w; ++c )
            for( int x=0; x<pxs; ++x )
                pVideoImg[r*w*pxs + c*pxs + x] = pFrameData[r*w+c];

    wxString text;
    text << _T("Frame: ") << w << _T(" x ") << h << _T(" x ") << pxs;
    SetStatusText( text, SBID_FPS );


    static int nTotalReceivedFrames = 0;
    nTotalReceivedFrames++;

    wxString text2;
    text2 << _T("Received frames: ") << nTotalReceivedFrames;
    SetStatusText( text2, SBID_FRAMES );

#ifdef _GUI_RUN
    // Update gui
    if( m_pCamView )
        m_pCamView->DrawCam( pVideoImg, w, h );
#endif

    return true;
}

void CGUIFrame::GetNextFrame()
{
    // Set a delay 1 sec between auto-retrieve frames
    wxSleep(1);

    if( m_sock && m_sock->IsConnected() )
    {
        // Tell the server which test we are running
        unsigned char c = SOCKET_CODE_GET_FRAME_DATA;
        m_sock->Write(&c, 1);
    }
}
