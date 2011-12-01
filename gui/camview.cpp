////////////////////////////////////////////////////////////////////
// Name:		implementation of the CCamView class
// File:		camview.cpp
// Purpose:		eye/camera view/GUI system control methods
//
// Based on source code of Larry Lart
////////////////////////////////////////////////////////////////////

#include "stdwx.h"

// system header
#include <math.h>
#include <stdio.h>

#ifdef __GNUG__
#pragma implementation
#pragma interface
#endif

//#include "wx/wxprec.h"
#include <wx/image.h>

// #ifndef  WX_PRECOMP
//   #include "wx/wx.h"
// #endif //precompiled headers

// other headers
#include "../wxMulticam.h"
#include "../camera/camera.h"

// main header
#include "camview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// implement message map
BEGIN_EVENT_TABLE(CCamView, wxWindow)
    EVT_PAINT( CCamView::OnPaint )
    EVT_SIZE( CCamView::OnSize ) 
END_EVENT_TABLE()

////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CCamView
// Purpose:	build my CCamView object
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CCamView::CCamView( wxWindow *frame, const wxPoint& pos, const wxSize& size ):
            wxWindow(frame, -1, pos, size, wxSIMPLE_BORDER ),
            m_pWxImg( NULL )
{
    m_pCamera = NULL;

    // set my canvas width/height
    m_nWidth = size.GetWidth( );
    m_nHeight = size.GetHeight( );

    m_bDrawing = false;

    m_bNewImage = 0;
}

////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CCamView
// Purpose:	destroy my object
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CCamView::~CCamView( )
{
    if( m_pWxImg )
        delete m_pWxImg;
    
    m_pWxImg = NULL;
    m_pCamera = NULL;
}

////////////////////////////////////////////////////////////////////
// Method:	Is Capture Enabled
// Class:	CCamView
// Purpose:	check if camera is initialized
// Input:	nothing
// Output:	bool yes/no
////////////////////////////////////////////////////////////////////
bool CCamView::IsCaptureEnabled( )
{
//	return( m_pCamera->IsInitialized( ) );
    return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:	OnPaint
// Class:	CCamView
// Purpose:	on paint event
// Input:	reference to paint event
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CCamView::OnPaint( wxPaintEvent& event )
{
    wxPaintDC dc(this);
    Draw( dc );
}

////////////////////////////////////////////////////////////////////
// Method:	Draw
// Class:	CCamView
// Purpose:	camera drawing
// Input:	reference to dc
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CCamView::Draw( wxDC& dc )
{
    // check if dc available
    if( !dc.IsOk( ) || m_bDrawing == true ){ return; }

        m_bDrawing = true;

        //dc.BeginDrawing();

        int x,y,w,h;
        dc.GetClippingBox( &x, &y, &w, &h );
        // if there is a new image to draw
        if( m_bNewImage )
        {

            dc.DrawBitmap( m_pBitmap, x, y );
            m_bNewImage = false;
        } else
        {
            // draw inter frame ?
        }

        //dc.EndDrawing();
        m_bDrawing = false;

    return;
}

////////////////////////////////////////////////////////////////////
// Method:	OnDraw
// Class:	CCamView
// Purpose:	CCamView drawing
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
//void CCamView::DrawCam( IplImage* pImg )
void CCamView::DrawCam( BYTE* pImg )
{
//	return;
    if( m_bDrawing ) return;
    m_bDrawing = true;
    int i = 0;

    // if there was an image then we need to update view
    if( pImg )
    {
        int nCamWidth = m_pCamera->m_nWidth;
        int nCamHeight = m_pCamera->m_nHeight;

        // convert data from raw image to wxImg 
        if( NULL == m_pWxImg )
            m_pWxImg = new wxImage( nCamWidth, nCamHeight, pImg/*rawData*/, TRUE );
        else
            m_pWxImg->SetData( pImg, nCamWidth, nCamHeight, TRUE );

        wxImage mirror = m_pWxImg->Mirror( false );

        //wxImage pWxImg = wxImage( nCamWidth, nCamHeight, pImg/*rawData*/, TRUE );

        // convert to bitmap to be used by the window to draw
        if( m_nWidth < nCamWidth || m_nHeight < nCamHeight )
            m_pBitmap = wxBitmap( mirror.Scale(m_nWidth, m_nHeight/*, wxIMAGE_QUALITY_BICUBIC*/) );
        else
            m_pBitmap = wxBitmap( mirror/**m_pWxImg*/ );


        m_bNewImage = true;
        m_bDrawing = false;

        Refresh( FALSE );

        Update( );
    }

}

////////////////////////////////////////////////////////////////////
// Method:	CheckUpdate
// Class:	CCamView
// Purpose:	CHeck for updates
// Input:	reference to size event
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CCamView::CheckUpdate()
{
    Update( );
}

////////////////////////////////////////////////////////////////////
// Method:	OnSize
// Class:	CCamView
// Purpose:	adjust on windows resize
// Input:	reference to size event
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CCamView::OnSize( wxSizeEvent& even )
{
    int nWidth = even.GetSize().GetWidth();
    int nHeight = even.GetSize().GetHeight();

    m_nWidth = nWidth;
    m_nHeight = nHeight;

}
