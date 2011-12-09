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

#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif //precompiled headers

// custom headers
#include "gui/frame.h"
#include "gui/camview.h"
#include "camera/camera.h"
#include "worker.h"

// include main header
#include "wxMulticam.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_APP(wxMultiCamApp)

// ***** CLASS wxMultiCamApp *****
////////////////////////////////////////////////////////////////////
// events table
BEGIN_EVENT_TABLE(wxMultiCamApp, wxApp)
END_EVENT_TABLE()


////////////////////////////////////////////////////////////////////
// Method:	On Init
// Class:	wxMultiCamApp
// Purpose:	initialize my application
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
bool wxMultiCamApp::OnInit( )
{
    // create widows frame
    CGUIFrame* m_pFrame = new CGUIFrame( NULL, L"wxMulticam Demo",
                         wxPoint(-1, -1), wxSize(640, 600) );

    // Show the frame
    m_pFrame->Show(TRUE);
    SetTopWindow(m_pFrame);

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

    return wxApp::OnExit();
}
