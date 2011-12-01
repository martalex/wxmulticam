////////////////////////////////////////////////////////////////////
// Package:		worker definition
// File:		worker.h
// Purpose:		manage checking, switching tasks
//
// Based on source code of Larry Lart
//
////////////////////////////////////////////////////////////////////

#ifndef _WORKER_H
#define _WORKER_H

//#include "wx/wxprec.h"
#include "wx/thread.h"

class CCamera;
class CGUIFrame;

//////////////////////////////////////////////////
// class:	CCameraWorker
/////////////////////////////////////////////////
class CCameraWorker : public wxThread
{
// methods
public:
    CCameraWorker( CGUIFrame *pFrame );
    virtual ~CCameraWorker( );

    // thread entry point
    virtual void *Entry();
    virtual void OnExit();
    double GetTime();

// data
public:
    CGUIFrame*      m_pFrame;
    CCamera*        m_pCamera;

    // timers
    //double          m_nCpuFreq;
    unsigned char   m_bLife;

// private data
private:
    // process synchronizer/locker
    wxMutex*        m_pMutex;
};

#endif


