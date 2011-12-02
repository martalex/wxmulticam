////////////////////////////////////////////////////////////////////
// Name:		camera view header
// File:		camview.h
// Purpose:		interface for the CCamView class.
//
// Based on source code of Larry Lart
//
////////////////////////////////////////////////////////////////////

#ifndef _CCAMVIEW_H
#define _CCAMVIEW_H

#include <stdint.h>

class CCamView : public wxWindow
{
public: 
    CCamView( wxWindow *frame, const wxPoint& pos, const wxSize& size );
    virtual ~CCamView( );

// Operations
public:

// Implementation
public:
    bool IsCaptureEnabled( );
    void CheckUpdate( );

    // Draw method
    void DrawCam( uint8_t* pImg, int w, int h );
    void Draw( wxDC& dc );

// Public data
public:

// Protected data
protected:
    wxImage*    m_pWxImg;
    wxBitmap    m_pBitmap;

    bool        m_bDrawing;
    bool        m_bNewImage;

    int         m_nWidth;
    int         m_nHeight;

// private methods
private:
    void OnPaint( wxPaintEvent& event );
    void OnSize( wxSizeEvent& even );

// protected data
protected:

    DECLARE_EVENT_TABLE()
};

#endif

