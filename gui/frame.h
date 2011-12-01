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

// external classes
class wxMultiCamApp;
class CCamView;
class CCameraWorker;

//// IDs for the controls and the menu commands
enum
{
    // menu items
    wxID_CAMSRC         = 1800,
    wxID_CAMFORMAT      = 1801,
};

static wxMenuBar *menuBar = NULL;

class CGUIFrame : public wxFrame
{
// public methods	
public:
    CGUIFrame( wxFrame *frame, const wxString& title, 
            const wxPoint& pos, const wxSize& size );
    virtual ~CGUIFrame( );
    
    CCamView* GetCameraView( );
    void SetParentApp( wxMultiCamApp *pApp );
    void SetStatusBarText( const char* strText, int number=0 );
    void ResetLayout( );

// public data
public:
    wxMultiCamApp*  m_pApp;
    CCameraWorker*  m_pWorker;

// Protected data
protected: 
    CCamView*       m_pCamView;
    wxPanel*        m_pMainPanel;

// message map functions
protected:
//     void OnVideoSource( wxCommandEvent& event );
//     void OnVideoFormat( wxCommandEvent& event );
    void OnAbout( wxCommandEvent& event );
    void OnExit( wxCommandEvent& WXUNUSED(pEvent) );

    DECLARE_EVENT_TABLE()
};

#endif
