////////////////////////////////////////////////////////////////////
// Name:		implementation of the CCamera class 
// File:		camera.cpp
// Purpose:		camera system control methods
//
// Based on source code of Larry Lart
////////////////////////////////////////////////////////////////////

#include "stdwx.h"

#define _GUI_RUN 1

// Multicam includes

#include "MultiCam/multicam.h"

// other local includes
#include "../wxMulticam.h"
#include "../worker.h"

// GUI include
#ifdef _GUI_RUN
#include "../gui/camview.h"
#include "../gui/frame.h"
#endif

// Main header
#include "camera.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void WINAPI GlobalGrabberCallback(PMCSIGNALINFO pSigInfo)
{
    if (pSigInfo && pSigInfo->Context)
    {
        CCamera* pCamera = (CCamera*) pSigInfo->Context;
        pCamera->OnImageGrabbed(pSigInfo);
    }
}

const int SURF_COUNT = 10;
        
////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CCamera
// Purpose:	build my Camera object  
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CCamera::CCamera(  ) :
    m_pCameraView( NULL ),
    m_pFrame( NULL ),
    m_pWorker( NULL )
{
    m_isRunning = false;
    m_isPause = false;
    
    // the new camera implementation
    m_pVideoImg = NULL;
    m_nFps = -1;
    m_nFpsAlpha = 0.1;
    
    m_isAvi = false;
    m_isImageReady = false;
    
    m_bIsChange = 0;
    m_nTotalFrames = 0;
}

////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CCamera
// Purpose:	destroy my object
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CCamera::~CCamera( )
{
    Uninitialize( );

    m_pWorker = NULL;
    m_pCameraView = NULL;
    m_pFrame = NULL;
}

////////////////////////////////////////////////////////////////////
// Method:	Init
// Class:	CCamera
// Purpose:	initialize my startup variables
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
int CCamera::Init(  )
{
    m_timeCurrFrameStamp = m_pWorker->GetTime( );
    m_timePrevFrameStamp = m_timeCurrFrameStamp;
    

    int ColorFormat;

    MCSTATUS Status;
    //channel
    Status = McCreate(MC_CHANNEL, &m_Channel);
    if(Status != MC_OK)
    {
        //FormatMulticamErrorText(Status, _T("McCreate"), ErrorStr);
        return false;
    }
    //board sys num
    Status = McSetParamInt(m_Channel, MC_DriverIndex, 0/*iBoardSystemNumber*/);
    if(Status != MC_OK)
    {
        // = _T("Please check camera connection to the computer and specified grabber system number.");
        //ErrorStr.Format(IDS_ERROR_GRABBER_CONNECTION, m_StartupSettings.m_iBoardSystemNumber);
        //Str.Format("McSetParamInt: MC_DriverIndex = %d", m_StartupSettings.m_iBoardSystemNumber);
        //FormatMulticamErrorText(Status, Str, ErrorStr);
        return false;
    }

// #ifdef _UNICODE
// 	USES_CONVERSION;
// 	char *connector = W2A(m_StartupSettings.m_strConnector);
// #else
    char *connector = "VID1";//(char *)LPCTSTR(m_StartupSettings.m_strConnector);
// #endif

    //connector
    Status = McSetParamStr(m_Channel, MC_Connector, connector);
    if(Status != MC_OK)
    {
        //Str.Format( _T("McSetParamStr: MC_Connector = %s"), (LPCTSTR)m_StartupSettings.m_strConnector);
        //FormatMulticamErrorText(Status, Str, ErrorStr);
        return false;
    }

// #ifdef _UNICODE
// 	//USES_CONVERSION;
// 	char *dcft = W2A(m_StartupSettings.m_strCameraFilename);
// #else
    char *dcft = "NTSC.cam";//(char *)LPCTSTR(m_StartupSettings.m_strCameraFilename);
// #endif

    //camera file
    Status = McSetParamStr(m_Channel, MC_CamFile, dcft);
    if(Status != MC_OK)
    {
        //Str.Format( _T("McSetParamStr: MC_CamFile = %s"), (LPCTSTR)m_StartupSettings.m_strCameraFilename);
        //FormatMulticamErrorText(Status, Str, ErrorStr);
        return false;
    }
    
    //image type
    int iPixelDepth = 1;
    ColorFormat = MC_ColorFormat_RGB24;//MC_ColorFormat_Y8;
    if( ColorFormat == MC_ColorFormat_RGB24 )
        iPixelDepth = 3;

    Status = McSetParamInt(m_Channel, MC_ColorFormat, ColorFormat);
    if(Status != MC_OK)
    {
        //Str.Format( _T("McSetParamInt: MC_ColorFormat = %d"), ColorFormat);
        //FormatMulticamErrorText(Status, Str, ErrorStr);
        return false;
    }

    //surface count
    int SurfaceCount = SURF_COUNT;
    Status = McSetParamInt(m_Channel, MC_SurfaceCount, SurfaceCount);
    if(Status != MC_OK)
    {
        //Str.Format( _T("McSetParamInt: MC_SurfaceCount = %d"), SurfaceCount);
        //FormatMulticamErrorText(Status, Str, ErrorStr);
        return false;
    }

    // Choose the way the first acquisition is triggered
    Status = McSetParamInt(m_Channel, MC_TrigMode, MC_TrigMode_IMMEDIATE);
    if(Status != MC_OK)
    {
        //Str.Format( _T("McSetParamInt: MC_TrigMode = %s"), _T("MC_TrigMode_IMMEDIATE"));
        //FormatMulticamErrorText(Status, Str, ErrorStr);
        return false;
    }  
    // Choose the triggering mode for subsequent acquisitions
    Status = McSetParamInt(m_Channel, MC_NextTrigMode, MC_NextTrigMode_REPEAT);
    if(Status != MC_OK)
    {
        //Str.Format( _T("McSetParamInt: MC_NextTrigMode = %s"), _T("MC_NextTrigMode_REPEAT"));
        //FormatMulticamErrorText(Status, Str, ErrorStr);
        return false;
    }

    // Choose the number of images to acquire
    Status = McSetParamInt(m_Channel, MC_SeqLength_Fr, MC_INDETERMINATE);
    if(Status != MC_OK)
    {
        //Str.Format( _T("McSetParamInt: MC_SeqLength_Fr = %s"), _T("MC_INDETERMINATE"));
        //FormatMulticamErrorText(Status, Str, ErrorStr);
        return false;
    }

    //image size
    INT32 cx = 640;
    INT32 cy = 480;

    //width
    Status = McSetParamInt(m_Channel, MC_ImageSizeX, /*m_StartupSettings.m_szImage.*/cx);
    if(Status != MC_OK)
    {
        //Str.Format( _T("McSetParamInt: MC_ImageSizeX = %d"), m_StartupSettings.m_szImage.cx);
        //FormatMulticamErrorText(Status, Str, ErrorStr);
        return false;
    }
    //height
    Status = McSetParamInt(m_Channel, MC_ImageSizeY, /*m_StartupSettings.m_szImage.*/cy);
    if(Status != MC_OK)
    {
        //Str.Format( _T("McSetParamInt: MC_ImageSizeY = %d"), m_StartupSettings.m_szImage.cy);
        //FormatMulticamErrorText(Status, Str, ErrorStr);
        return false;
    }

    //set line pitch  = image width (as required by Dr. Lee's library
    int iLinePitch = /*m_StartupSettings.m_szImage.*/cx * iPixelDepth;
    Status = McSetParamInt(m_Channel, MC_BufferPitch, iLinePitch);
    if(Status != MC_OK)
    {
        //Str.Format( _T("McSetParamInt: MC_BufferPitch = %d"), iLinePitch);
        //FormatMulticamErrorText(Status, Str, ErrorStr);
        return false;
    }

    // Enable MultiCam signals
    //processing
    Status = McSetParamInt(m_Channel, MC_SignalEnable + MC_SIG_SURFACE_PROCESSING, 
        MC_SignalEnable_ON);
    if(Status != MC_OK)
    {
        //FormatMulticamErrorText(Status, _T("McSetParamInt: MC_SignalEnable + MC_SIG_SURFACE_PROCESSING"), ErrorStr);
        return false;
    }
    //failure
    Status = McSetParamInt(m_Channel, MC_SignalEnable + MC_SIG_ACQUISITION_FAILURE, 
        MC_SignalEnable_ON);
    if(Status != MC_OK)
    {
        //FormatMulticamErrorText(Status, _T("McSetParamInt: MC_SignalEnable + MC_SIG_ACQUISITION_FAILURE"), ErrorStr);
        return false;
    }


    // Register the callback function
    McRegisterCallback(m_Channel, GlobalGrabberCallback, this);

    //GetSize();
    
    //reset the callback event
//	ResetEvent(m_ImageGrabbedEvent);

    //activate
// 	if(!StartGrab(ErrorStr))
// 	{
// 		return false;
// 	}
    return( 1 );
}

void CCamera::Uninitialize()
{
    if(m_Channel)
    {
        McSetParamInt(m_Channel, MC_ChannelState, MC_ChannelState_IDLE);
        McDelete(m_Channel);
        m_Channel = 0;
    }

    if( m_pVideoImg )
        delete [] m_pVideoImg;
    
    m_pVideoImg = NULL;
}

////////////////////////////////////////////////////////////////////
// Method:	GetSize
// Class:	CCamera
// Purpose:	initialize my startup variables
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
int CCamera::GetSize( )
{
    //width
    MCSTATUS Status = McGetParamInt(m_Channel, MC_ImageSizeX, &m_nWidth);
    if(Status != MC_OK)
    {
        //Str.Format( _T("McSetParamInt: MC_ImageSizeX = %d"), m_StartupSettings.m_szImage.cx);
        //FormatMulticamErrorText(Status, Str, ErrorStr);
        return false;
    }
    //height
    Status = McGetParamInt(m_Channel, MC_ImageSizeY, &m_nHeight);
    if(Status != MC_OK)
    {
        //Str.Format( _T("McSetParamInt: MC_ImageSizeY = %d"), m_StartupSettings.m_szImage.cy);
        //FormatMulticamErrorText(Status, Str, ErrorStr);
        return false;
    }

        
    // now get my properties
// 	m_nWidth = (int) cvGetCaptureProperty( m_pCapture, CV_CAP_PROP_FRAME_WIDTH );
// 	m_nHeight = (int) cvGetCaptureProperty( m_pCapture, CV_CAP_PROP_FRAME_HEIGHT );
    
    // set camview size
    wxASSERT( m_pCameraView && m_pFrame );

    if( m_pCameraView )
        m_pCameraView->SetSize( m_nWidth, m_nHeight );
    if( m_pFrame )
        m_pFrame->ResetLayout( );

    return( 0 );
}

////////////////////////////////////////////////////////////////////
// Method:	Start
// Class:	CCamera
// Purpose:	Start capture
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CCamera::Start( )
{
    MCSTATUS Status;
    Status = McSetParamInt(m_Channel, MC_ChannelState, MC_ChannelState_ACTIVE);
    if(Status != MC_OK)
    {
//         FormatMulticamErrorText(Status, _T("McSetParamInt: MC_ChannelState_ACTIVE"), ErrorStr);
//         CString strPossibleReasonOfFailure;
//         strPossibleReasonOfFailure.Format(
//             IDS_ERROR_CANT_START_GRABBER_ONE_OF_POSSIBLE_REASONS_OF_FAILURE,
//             m_StartupSettings.m_szImage.cx, m_StartupSettings.m_szImage.cy
//             );
//         ErrorStr += strPossibleReasonOfFailure;
        return /*false*/;
    }


/*
#ifdef WIN32_LARRY
    m_pCapture = cvCaptureFromCAM( -1 );

    // grab first frame to initialize format
    IplImage* pFrame = cvQueryFrame( m_pCapture );
    // get camera's size
    GetSize( );

#else
    int ncameras = cvcamGetCamerasCount();
    printf("DEBUG :: Found cameras=%d\n", ncameras );
    int w = 640;
    int h = 480;
    int nCam = 0;
    int t=1;
    int p=1;
    
    cvcamSetProperty(nCam, CVCAM_RNDWIDTH,  &w );
    cvcamSetProperty(nCam, CVCAM_RNDHEIGHT, &h );

    cvcamSetProperty(nCam, CVCAM_PROP_ENABLE, &t );

    int width = 320;
    int height = 200;


    HWND MyWin = (HWND)m_pCameraView->GetHandle();
    cvcamSetProperty(nCam, CVCAM_PROP_WINDOW, &MyWin ); 
    cvcamSetProperty(nCam, CVCAM_PROP_RENDER, &p );  

    
    cvcamSetProperty(0, CVCAM_PROP_CALLBACK, (void*)testcallback );
    cvcamInit();
    Sleep( 5000 );
    cvcamStart();
#endif	
*/
    m_isRunning = true;
}

////////////////////////////////////////////////////////////////////
// Method:	PauseResume
// Class:	CCamera
// Purpose:	pause/resume capture
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CCamera::PauseResume( )
{
    if( m_isPause )
        m_isPause = false;
    else
        m_isPause = true;
}

////////////////////////////////////////////////////////////////////
// Method:	IsChanged
// Class:	CCamera
// Purpose:	mark flag change
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CCamera::IsChanged( )
{
    m_bIsChange = 1;
    m_isPause = true;
}

////////////////////////////////////////////////////////////////////
// Method:	Stop
// Class:	CCamera
// Purpose:	Stop capture
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void  CCamera::Stop( )
{
    m_isRunning = false;

    MCSTATUS Status;
    Status = McSetParamInt(m_Channel, MC_ChannelState, MC_ChannelState_IDLE);
    if(Status != MC_OK)
    {
        //FormatMulticamErrorText(Status, _T("McSetParamInt: MC_ChannelState_IDLE"), ErrorStr);
        return /*false*/;
    }

// #ifdef WIN32_LARRY
// 	cvReleaseCapture( &m_pCapture );
// 	if( m_pVideoImg )
// 	{
// 		cvReleaseImage( &m_pVideoImg );
// 		m_pVideoImg = NULL;
// 	}
// #else
// 	cvcamExit();
// #endif

}

////////////////////////////////////////////////////////////////////
// feedback - only for linux
// #ifndef WIN32_LARRY
// void testcallback(void* _image)
// {
// 	IplImage* image = (IplImage*)_image;
// 	cvLine(image, cvPoint(0, 0), cvPoint(image->width, image->height),CV_RGB(255, 0, 0), 1);
// 	m_pLnxFrame = image ;
// }
// #endif

////////////////////////////////////////////////////////////////////
// Method:	Get Next Frame
// Class:	CCamera
// Purpose:	get next frame from camera buffer
// Input:	pointer to void
// Output:	Nothing
////////////////////////////////////////////////////////////////////
void CCamera::GetNextFrame( void* )
{
    static int repositioning = 0;
//    IplImage* pFrame = NULL;

    const int kMinimalPoolingTime = 1; // 1 ms

    // get current frame time stamp
    m_timeCurrFrameStamp = m_pWorker->GetTime( );
    if( m_timeCurrFrameStamp - m_timePrevFrameStamp  < kMinimalPoolingTime )
        return;
//    else
//        m_timePrevFrameStamp = m_timeCurrFrameStamp;

// #ifdef WIN32_LARRY	
// 	// get frame if any
//  	pFrame = cvQueryFrame( m_pCapture );
// #else
// 	// to test this - don't remmember how i got this working 
// 	// on linux 
// 	pFrame = m_pLnxFrame;
// 	// get frame in linux ?
// 
// 	cvcamPause();
// 	cvcamGetProperty(0,CVCAM_PROP_RAW,&pFrame);
// 	cvcamResume();
// 
// 	pFrame = cvCloneImage( fimg );
// 
// 	Sleep(400);
// #endif

    // if this was avi and frame is zero(end or none?) stop
//     if( pFrame == 0 && m_isAvi )
//     {
//         //this->StopAvi( 0,0 );
//         return;
// 	}

    ////////////////////////////
    // if video window ? & frame ?
//     if( pFrame )
//     {
// 		// if no video image 
//         if( !m_pVideoImg ) 
//         {
//             cvReleaseImage( &m_pVideoImg );
// 			m_pVideoImg = cvCreateImage( cvSize( m_nWidth, m_nHeight ), 8, 3 );
//         }
// 		
// 		// check for the last origin of the frame
// 		if( pFrame->origin == 1 )
// 		{
// 			cvConvertImage( pFrame, m_pVideoImg, CV_CVTIMG_FLIP | CV_CVTIMG_SWAP_RB );
// 		} else
// 		{
// 			cvCopy( pFrame, m_pVideoImg, 0 );
// 		}		
// 
// #ifdef _GUI_RUN
// 		// Update gui
// 		m_pCameraView->DrawCam( m_pVideoImg );
// #endif	
// 		
//     }
    if( m_isImageReady )
    {
#ifdef _GUI_RUN
        // Update gui
        if( m_pCameraView )
            m_pCameraView->DrawCam( m_pVideoImg );
#endif	

        m_isImageReady = false;

        // If camera started
        if( m_isRunning )
        {
            // get current frame time stamp
            m_timeCurrFrameStamp = m_pWorker->GetTime( );

            int timeBetweenFrames = m_timeCurrFrameStamp - m_timePrevFrameStamp;
            if( timeBetweenFrames <= 0 )
                timeBetweenFrames = 1;

            // update fps
            if( m_nFps < 0 )
                m_nFps = 1000 / timeBetweenFrames;
            else
                m_nFps = ( 1 - m_nFpsAlpha ) * m_nFps + 
                    m_nFpsAlpha * 1000 / timeBetweenFrames;

            // set current time stamp as previous
            m_timePrevFrameStamp = m_timeCurrFrameStamp;

            ++m_nTotalFrames;

            // get info of number of frames per second in a string
            // for debugging/etc
            sprintf( m_strFps, "FPS: %5.1f", m_nFps );
            m_pFrame->SetStatusBarText( m_strFps,0 );

            // get info of number of received frames in a string
            // for debugging/etc
            sprintf( m_strFps, "Frames: %d", m_nTotalFrames );
            m_pFrame->SetStatusBarText( m_strFps, 1 );
        }
    }
}

////////////////////////////////////////////////////////////////////
// Method:	GetIFrame
// Class:	CCamera
// Purpose:	get last frame grabbed
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
//IplImage *CCamera::GetIFrame( )
BYTE *CCamera::GetIFrame( )
{
    return( m_pVideoImg );
}


////////////////////////////////////////////////////////////////////
// Method:	Run
// Class:	CCamera
// Purpose:	Start to run my camera thread
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
int CCamera::Run( ) 
{
    if( !m_isPause )
    {
        // Get my next frame
        this->GetNextFrame( NULL );
    } else
    {
        if( m_bIsChange == 1 )
        {
            // check size
            Stop( );
            Start( );
            m_bIsChange = 0;
            m_isPause = 0;
        }
    }

    return( 0 );
}

//---------------------------------------------------------------------------
//called after the image had been grabbed
void CCamera::OnImageGrabbed(PMCSIGNALINFO pSigInfo)
{
    //ignore if not yet started
    if(!m_isRunning || m_isPause )
    {
        return;
    }

    m_isImageReady = false;

    //look what happened
    bool Result = false;

    void* pPixels = NULL;
    int ImageWidth, ImageHeight, ColorFormat;
    MCSTATUS Status;

    switch(pSigInfo->Signal)
    {
    case MC_SIG_SURFACE_PROCESSING:
        Result = true;
        break;
    case MC_SIG_ACQUISITION_FAILURE:
        break;
    default:
        break;
    }

    if( false != Result )
    {
        //get the source of pixels
        Status = McGetParamInt(pSigInfo->SignalInfo, MC_SurfaceAddr, (PINT32)&pPixels);
        if(Status != MC_OK)
        {
            //FormatMulticamErrorText(Status, _T("MC_SurfaceAddr"), strError);
            Result = false;
        }
        else    
        //image parameters
            Status = McGetParamInt(m_Channel, MC_ImageSizeX, &ImageWidth); 

        if(Status != MC_OK)
        {
            //Str = _T("McGetParamInt: MC_ImageSizeX");
            //FormatMulticamErrorText(Status, Str, strError);
            Result = false;
        }
        else
            Status = McGetParamInt(m_Channel, MC_ImageSizeY, &ImageHeight); 

        if(Status != MC_OK)
        {
            //Str = _T("McGetParamInt: MC_ImageSizeY");
            //FormatMulticamErrorText(Status, Str, strError);
            Result = false;
        }
        else
            Status = McGetParamInt(m_Channel, MC_ColorFormat, &ColorFormat);

        int ImagePixelSize=0;
        if(Status != MC_OK)
        {
            //Str = _T("McGetParamInt: MC_ColorFormat");
            //FormatMulticamErrorText(Status, Str, strError);
            Result = false;
        }
        else
            Status = McGetParamInt(m_Channel, MC_ImagePixelSize, &ImagePixelSize);

        int iLinePitch = 1;
        if(Status != MC_OK)
        {
            //Str = _T("McGetParamInt: MC_ColorFormat");
            //FormatMulticamErrorText(Status, Str, strError);
            Result = false;
        }

/*	    if(ColorFormat == MC_ColorFormat_Y8)
        {
            ImageFmt = IDT_BW08;
        }
        else if(ColorFormat == MC_ColorFormat_Y12)
        {
            ImageFmt = IDT_BW12;
        }
        else
        {
            ImageWidth = 0;
            ImageHeight = 0;
            ImageFmt = 0;
            strError.Format(IDS_ERROR_UNSUPPORTED_IMAGE_TYPE, ColorFormat);
            Result = false;
            goto end;
        }
*/
        if( ImageHeight != m_nHeight || ImageWidth != m_nWidth )
        {
            //GetSize();

            m_nWidth = ImageWidth;
            m_nHeight = ImageHeight;

            if( m_pCameraView )
                m_pCameraView->SetSize( m_nWidth, m_nHeight );
            if( m_pFrame )
                m_pFrame->ResetLayout( );
        }
        //copy
        int ImageSizeBytes = ImageWidth*ImageHeight*ImagePixelSize;
        if( NULL == m_pVideoImg )
            m_pVideoImg = new BYTE [ImageSizeBytes];
        
        if( NULL != m_pVideoImg )
        {
            memcpy(m_pVideoImg, pPixels, ImageSizeBytes);

            m_isImageReady = true;
        }
    }

// end:
// 	m_GrabbResultIntermediateCS.BeginWrite();
// 	m_GrabbingResult = Result;
// 	m_GrabbingError = strError;
// 	m_GrabbResultIntermediateCS.EndWrite();
// 	if(!Result)
// 	{
// 		StopGrab(strFakeErrorStr);
// 	}
// 	//prepare the image
// 	SetEvent(m_ImageGrabbedEvent);
}
