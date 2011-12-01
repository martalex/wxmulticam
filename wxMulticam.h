////////////////////////////////////////////////////////////////////
// Name:		wxMulticam Header
// File:		wxMulticam.h
// Purpose:		Main header with main defines for wxMulticam
//
// Created by:  Olex Martynovskyy 
//              altanzar@gmail.com
//              December 01 2011
//
// Based on source code of Larry Lart
//
////////////////////////////////////////////////////////////////////

#ifndef _WX_MULTICAM_H
#define _WX_MULTICAM_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"       // main symbols
//#include "wx/thread.h"

// external classes
class CCameraWorker;
class CCamera;

class wxMultiCamApp : public wxApp
{
    bool m_MulticamDriverOK;

    void OpenMulticam();
    void CloseMulticam();

// public methods
public:

// public data
public:
    CCameraWorker*  m_pCameraWorker;
    CCamera*        m_pCamera;

// protected methods
protected:
    bool OnInit( );
    int OnExit( );

    DECLARE_EVENT_TABLE()
};

#endif // _WX_MULTICAM_H
