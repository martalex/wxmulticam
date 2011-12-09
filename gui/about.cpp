////////////////////////////////////////////////////////////////////
// Package:		GUI About implementation
// File:		about.cpp
// Purpose:		define about window
//
// Based on source code of Larry Lart
//
////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation
#pragma interface
#endif

#include "stdwx.h"

#include <wx/bitmap.h>
#include <wx/gdicmn.h>
#include "wx/statline.h"

#if defined(__WXMSW__)
    #ifdef wxHAVE_RAW_BITMAP
    #include "wx/rawbmp.h"
    #endif
#endif

#if defined(__WXMAC__) || defined(__WXGTK__)
    #define wxHAVE_RAW_BITMAP
    #include "wx/rawbmp.h"
#endif

#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif //precompiled headers

#ifdef __WXX11__
//#include "ronn.xpm"
#endif

#include "about.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CWxOpencvAbout
// Purpose:	Initialize my about dialog
// Input:	pointer to reference window
// Output:	nothing	
////////////////////////////////////////////////////////////////////
CAbout::CAbout(wxWindow *parent)
               : wxDialog(parent, -1,
                          _("About wxMulticam Demo"),
                          wxDefaultPosition,
                          wxDefaultSize,
                          wxDEFAULT_DIALOG_STYLE /*| wxDIALOG_MODAL*/)
{
    // layout components
    wxBoxSizer *sizer = new wxBoxSizer( wxVERTICAL );
    if( NULL == sizer )
        return;

    // load logo image from resource by string identifier
//     wxBitmap bmp(wxBITMAP(IDC_ABOUT));

//     if( bmp.Ok() )
//     {
// #if !defined(__WXGTK__) && !defined(__WXMOTIF__) && !defined(__WXMAC__)
//         bmp.SetMask(new wxMask(bmp, *wxBLUE));
// #endif
//         wxStaticBitmap *sbmp = new wxStaticBitmap(this, -1, bmp);

//         sizer->Add( sbmp, 0, wxCENTRE | wxALL, 10 );
//     }

    sizer->Add( new wxStaticLine(this, -1), 0, wxGROW | wxLEFT | wxRIGHT, 5 );
    sizer->Add( CreateTextSizer(
        _(  "wxMulticam Demo\n"
            "(demo of Multicam with wxwidgets)\n"
        ) ),
        0, wxCENTRE | wxALL, 10 );

    sizer->Add( new wxStaticLine(this, -1), 0, wxGROW | wxLEFT | wxRIGHT, 5 );
    sizer->Add( CreateButtonSizer(wxOK), 0, wxCENTRE | wxALL, 10 );

    // activate
    SetSizer(sizer);
    SetAutoLayout(TRUE);
    sizer->SetSizeHints(this);
    sizer->Fit(this);
    Centre(wxBOTH | wxCENTRE_ON_SCREEN);
}

