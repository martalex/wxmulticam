////////////////////////////////////////////////////////////////////
// Package:		worker implementation
// File:		worker.cpp
// Purpose:		manage checking, switching tasks
//
// Based on source code of Larry Lart
//
////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation
#pragma interface
#endif

#include "stdwx.h"

// system headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/timeb.h>
#include <sys/types.h>
#include <sys/stat.h>

// wxwidgets includes
//#include "wx/wxprec.h"
#include "wx/thread.h"
#include <wx/string.h>
#include <wx/regex.h>

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

#include "wxMulticam.h"
#include "gui/frame.h"
#include "camera/camera.h"

// main header include
#include "worker.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CCameraWorker
// Purpose:	Initialize my worker thread
// Input:	pointer to reference frame
// Output:	nothing
////////////////////////////////////////////////////////////////////
CCameraWorker::CCameraWorker( CGUIFrame *pFrame, CCamera* pCamera  ) : 
    wxThread( wxTHREAD_DETACHED ),
    m_pFrame( pFrame ), // get frame reference
    m_pCamera( pCamera ) // get camera reference
{
    // start life
    m_bLife = 1;

    m_pMutex = new wxMutex( );

    return;
}

////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CCameraWorker
// Purpose:	object destructor
// Input:	nothing
// Output:	nothing	
////////////////////////////////////////////////////////////////////
CCameraWorker::~CCameraWorker( )
{
    m_pMutex = NULL;
    m_pCamera = NULL;
    m_pFrame = NULL;
}

////////////////////////////////////////////////////////////////////
// Method:	On Exit
// Class:	CCameraWorker
// Purpose:	do something on exit
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CCameraWorker::OnExit( )
{
    // destroy - clean my place
    delete( m_pMutex );
}

////////////////////////////////////////////////////////////////////
// Method:	GetTime
// Class:	CCameraWorker
// Purpose:	routine to get time accurate - this would be called by
//			all other components to keep in sync
// Input:	nothing
// Output:	number = accurate time count
////////////////////////////////////////////////////////////////////
double CCameraWorker::GetTime( )
{
    //
    struct timeb timeStamp;
    // get a time stamp
    ftime( &timeStamp );

    double nTime = (double) timeStamp.time*1000 + timeStamp.millitm;

    return( nTime );
    // i removed this for the moment - this was used to calculate time 
    //based on the cpu tick count
    //return (double)cvGetTickCount()*1e-3/(m_nCpuFreq+1e-10);
}

////////////////////////////////////////////////////////////////////
// Method:		Entry
// Class:		CCameraWorker
// Purpose:		the main executable body of my thread
// Input:		nothing
// Output:		void pointer
////////////////////////////////////////////////////////////////////
void *CCameraWorker::Entry( )
{
    int i = 0;
    m_bLife = 1;

#ifdef WIN32
    m_pCamera->Start( );
#endif

    ////////////////////////////////////////////////////////////////
    // Start Life Cycle
    ////////////////////////////////////////////////////////////////
    // loop as long as flag m_bLife = 1
    while( m_bLife )
    {
        if( TestDestroy( ) == 1 ) 
            break;

        ///////////////////////////////////////////////////////////
        // START PROCESING BLOCK
        // first tell camera object to process one frame - if camera
        // is running
        if( m_pCamera->m_isRunning )
        {
#ifdef WIN32
            m_pCamera->Run( );
#endif

        }
        // the here you can insert your extra bit of processing
        // .... code

        // END PROCESSING BLOCK
        ///////////////////////////////////////////////////////////

        wxThread::Sleep( 1 );

    }

#ifdef WIN32
    if( m_pCamera )
        m_pCamera->Stop( );
#endif

    return NULL;
}

void CCameraWorker::Stop()
{
    if( m_pCamera )
#ifdef WIN32
        m_pCamera->PauseResume();
#endif
    
    Delete();
}
