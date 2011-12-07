////////////////////////////////////////////////////////////////////
// Name:		camera header
// File:		camera.h
// Purpose:		interface for the CCamera class.
//
// Based on source code of Larry Lart
//
////////////////////////////////////////////////////////////////////

#ifndef _CAMERA_H
#define _CAMERA_H

//define external classes
#if _GUI_RUN
class CCamView;
class CGUIFrame;
#endif

class CCameraWorker;

#include "MultiCam/multicam.h"

class CCamera 
{
// public methods
public:
    // constructor
    CCamera( 
#if _GUI_RUN    
    CGUIFrame* pFrame, CCamView* pCameraView 
#endif
    );
    ~CCamera( );

    // get frame method
    BYTE *GetIFrame( );

    // initialize camera frame def
    int Init( );
    void Uninitialize( );

    int Run();
    void Start( );
    void Stop( );
    void PauseResume( );
    int GetSize( );
    void IsChanged( );

    ///< called after the image had been grabbed
    void OnImageGrabbed( MCSIGNALINFO* pSigInfo ); 

    wxString GetFpsStr() { return m_strFps; }
    wxString GetFramesStr() { return m_strFrames; }

// data
public:
    // status flags
    bool            m_isRunning;
    bool            m_isAvi;
    bool            m_isPause;

    bool            m_bProcBusy;
    int             m_bIsChange;


    CCameraWorker*  m_pWorker;

    int             m_nWidth;
    int             m_nHeight;
    int             m_nImagePixelSize;

// protected data
protected:
#if _GUI_RUN
    CCamView*       m_pCameraView;
    CGUIFrame*      m_pFrame;
#endif

    BYTE*           m_pVideoImg;
    bool            m_isImageReady;

    double          m_nFpsAlpha;
    double          m_nFps;
    wxString        m_strFps;
    wxString        m_strFrames;
    double          m_timePrevFrameStamp;
    double          m_timeCurrFrameStamp;
    int             m_nTotalFrames;

    MCHANDLE m_Channel;
    bool m_MulticamDriverOK;

    bool m_bIsProcessing;

// Implementation
protected:
    void GetNextFrame( void* );

    bool OpenMulticam();
    void CloseMulticam();

};

#endif 
