////////////////////////////////////////////////////////////////////
// Name:		Main Frame header
// File:		frame.h
// Purpose:		interface for the main frame class.
//
// Based on source code of Larry Lart
//
////////////////////////////////////////////////////////////////////

#ifndef _MAINFRAME_H
#define _MAINFRAME_H

// DEBUG
#ifndef CRTDBG_MAP_ALLOC
#define CRTDBG_MAP_ALLOC
#endif

#include "wx/socket.h"

// this example is currently written to use only IP or only IPv6 sockets, it
// should be extended to allow using either in the future
#if wxUSE_IPV6
typedef wxIPV6address IPaddress;
#else
typedef wxIPV4address IPaddress;
#endif


// external classes
class wxMultiCamApp;
class CCamView;
class CCamera;
class CCameraWorker;

//// IDs for the controls and the menu commands
enum
{
    // id for sockets
    SERVER_ID = 100,
    SOCKET_ID = 101,

    // menu items
    // Net
    wxID_NET_START_SERVER = wxID_HIGHEST+1,
    wxID_NET_CONNECT_TO_SERVER,
    wxID_NET_WAIT_CONNECT,
    
    // Camera
    wxID_CAM_START,
    wxID_CAM_STOP
};

static wxMenuBar *menuBar = NULL;

class CGUIFrame : public wxFrame
{
// public methods	
public:
    CGUIFrame( wxFrame *frame, const wxString& title, 
            const wxPoint& pos, const wxSize& size );
    virtual ~CGUIFrame( );
    
    void SetParentApp( wxMultiCamApp *pApp );
    void SetStatusBarText( const wxString& strText, int number=0 );
    void ResetLayout( );

protected:
    bool ServerCreate();

// public data
public:

// Protected data
protected: 
    CCamera*        m_pCamera;
    CCameraWorker*  m_pWorker;

    CCamView*       m_pCamView;
    wxPanel*        m_pMainPanel;

    wxSocketServer *m_server;

// message map functions
protected:
    void OnStartServer( wxCommandEvent& event );
    void OnConnectToServer( wxCommandEvent& event );
    void OnWaitConnect( wxCommandEvent& event );
    
    void OnCameraStart( wxCommandEvent& event );
    void OnCameraStop( wxCommandEvent& event );

    void CameraStop();

    void OnAbout( wxCommandEvent& event );
    void OnExit( wxCommandEvent& WXUNUSED(pEvent) );

    void OnServerEvent(wxSocketEvent& event);

    DECLARE_EVENT_TABLE()
};
#endif
