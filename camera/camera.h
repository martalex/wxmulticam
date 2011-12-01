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
class CCamView;
class CCameraWorker;
class CGUIFrame;

#include "MultiCam/multicam.h"

class CCamera 
{
// public methods
public:
    // constructor
    CCamera( );
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

// data
public:
    // status flags
    bool            m_isRunning;
    bool            m_isAvi;
    bool            m_isPause;

    bool            m_bProcBusy;
    int             m_bIsChange;

    CCamView*       m_pCameraView;
    CGUIFrame*      m_pFrame;
    CCameraWorker*  m_pWorker;

    int             m_nWidth;
    int             m_nHeight;

// protected data
protected:
    BYTE*           m_pVideoImg;
    bool            m_isImageReady;

    double          m_nFpsAlpha;
    double          m_nFps;
    char            m_strFps[32];
    double          m_timePrevFrameStamp;
    double          m_timeCurrFrameStamp;
    int             m_nTotalFrames;

    MCHANDLE m_Channel;

// Implementation
protected:
    void GetNextFrame( void* );
};

#endif 
