// MULTICAM.H -- MULTICAM DRIVER API

#ifndef _MULTICAM_
#define _MULTICAM_

#if defined(WINAPI)
#define MCAPI WINAPI
#elif defined(__BORLANDC__)
#define MCAPI stdcall
#elif defined(__GNUC__)
#define MCAPI
#else
#define MCAPI _stdcall
#endif

#if defined(DLL_EXPORTS)
#define DLL_API __declspec(dllexport) MCAPI
#elif defined(__BORLANDC__)
#define DLL_API _import MCAPI
#elif defined(__GNUC__)
#define DLL_API
#else
#define DLL_API __declspec(dllimport) MCAPI
#endif

// MULTICAM PARAMETERS AND DEFINITIONS
#include "McParams.h"
#include "McDef.h"
#include "McIo.h"
#if !defined(DLL_EXPORTS)
#include "McParamsCompat1x.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

// DATA TYPES
#ifdef __GNUC__
typedef unsigned long long int UINT64, *PUINT64;
typedef signed long long int INT64, *PINT64;
#else
typedef unsigned __int64 UINT64, *PUINT64;
typedef signed __int64 INT64, *PINT64;
#endif // __GNUC__

typedef unsigned int UINT32, *PUINT32;
typedef signed int INT32, *PINT32;
typedef unsigned short UINT16, *PUINT16;
typedef signed short INT16, *PINT16;
typedef unsigned char UINT8, *PUINT8;
typedef signed char INT8, *PINT8;
typedef char CHAR, *PCHAR;
typedef const char *PCCHAR;
typedef double FLOAT64, *PFLOAT64;
typedef float FLOAT32, *PFLOAT32;
typedef void *PVOID;

typedef int MCSTATUS;
typedef UINT32 MCHANDLE, *PMCHANDLE;
typedef UINT32 MCPARAMID, *PMCPARAMID;
typedef int MCEVENT, MCSIGNAL;


// SIGNALING
typedef struct _MC_CALLBACK_INFO {
	PVOID Context;
	MCHANDLE Instance;
	MCSIGNAL Signal;
	UINT32 SignalInfo;
	UINT32 Reserved;
} MCSIGNALINFO, *PMCSIGNALINFO, *PMCCALLBACKINFO, MCCALLBACKINFO;

typedef void (MCAPI *PMCCALLBACK)(PMCSIGNALINFO CbInfo);
typedef void (MCAPI *PMCCALLBACKEX)(PVOID Context);

// OBJECT CLASSES
#define MC_GRABBER_CLASS		0x6
#define MC_SURFACE_CLASS		0x4
#define MC_CHANNEL_CLASS		0x8
#define MC_VIEWER_CLASS			0xA
#define MC_CONFIG_CLASS			0x2
#define MC_PROCESSOR_CLASS		0xC
#define MC_BOARD_CLASS			0xE


// DEFAULT INSTANCE TEMPLATES
#define MC_DEFAULT_GRABBER_HANDLE ((MC_GRABBER_CLASS << 28) | 0x0FFFFFFF)
#define MC_DEFAULT_SURFACE_HANDLE ((MC_SURFACE_CLASS << 28) | 0x0FFFFFFF)
#define MC_DEFAULT_CHANNEL_HANDLE ((MC_CHANNEL_CLASS << 28) | 0x0FFFFFFF)
#define MC_DEFAULT_VIEWER_HANDLE  ((MC_VIEWER_CLASS << 28) | 0x0FFFFFFF)
#define MC_DEFAULT_PROCESSOR_HANDLE ((MC_PROCESSOR_CLASS << 28) | 0x0FFFFFFF)
#define MC_PROCESSOR_MULTI_HANDLE   ((MC_PROCESSOR_CLASS << 28) | 0x0FFFFFFF)
#define MC_PROCESSOR_PRESTISSIMO_HANDLE ((MC_PROCESSOR_CLASS << 28) | 0x0FFEFFFF)

#define MC_CONFIGURATION			((MC_CONFIG_CLASS << 28) | 0)

#define MC_BOARD					((MC_BOARD_CLASS << 28) | 0)

// CHANNEL CREATION MODELS
#define MC_CHANNEL					((MC_CHANNEL_CLASS << 28) | 0x0000FFFF)
#define MC_CHANNEL_GAMMA_X			((MC_CHANNEL_CLASS << 28) | 0x0001FFFF)
#define MC_CHANNEL_GAMMA_Y			((MC_CHANNEL_CLASS << 28) | 0x0002FFFF)
#define MC_CHANNEL_GAMMA_Z			((MC_CHANNEL_CLASS << 28) | 0x0003FFFF)
#define MC_CHANNEL_GAMMA_XBIS		((MC_CHANNEL_CLASS << 28) | 0x0004FFFF)
#define MC_CHANNEL_GAMMA_YBIS		((MC_CHANNEL_CLASS << 28) | 0x0005FFFF)
#define MC_CHANNEL_GAMMA_ZBIS		((MC_CHANNEL_CLASS << 28) | 0x0006FFFF)
#define MC_CHANNEL_ALPHA_X			((MC_CHANNEL_CLASS << 28) | 0x0007FFFF)
#define MC_CHANNEL_ALPHA_Y			((MC_CHANNEL_CLASS << 28) | 0x0008FFFF)
#define MC_CHANNEL_ALPHA_XBIS		((MC_CHANNEL_CLASS << 28) | 0x0015FFFF)
#define MC_CHANNEL_ALPHA_YBIS		((MC_CHANNEL_CLASS << 28) | 0x0016FFFF)
#define MC_CHANNEL_ALPHA_STEREOXY	((MC_CHANNEL_CLASS << 28) | 0x001CFFFF)
#define MC_CHANNEL_VALUE_M			((MC_CHANNEL_CLASS << 28) | 0x0009FFFF)
#define MC_CHANNEL_EXPERT_M			((MC_CHANNEL_CLASS << 28) | 0x000AFFFF)
#define MC_CHANNEL_EXPERT_A			((MC_CHANNEL_CLASS << 28) | 0x000BFFFF)
#define MC_CHANNEL_EXPERT_B			((MC_CHANNEL_CLASS << 28) | 0x000CFFFF)
#define MC_CHANNEL_ALPHA_CPCI_CAM0	((MC_CHANNEL_CLASS << 28) | 0x000DFFFF)
#define MC_CHANNEL_ALPHA_CPCI_CAM1	((MC_CHANNEL_CLASS << 28) | 0x000EFFFF)
#define MC_CHANNEL_ALPHA_CPCI_CAM2	((MC_CHANNEL_CLASS << 28) | 0x000FFFFF)
#define MC_CHANNEL_ALPHA_CPCI_CAM3	((MC_CHANNEL_CLASS << 28) | 0x0010FFFF)
#define MC_CHANNEL_IOTA_X			((MC_CHANNEL_CLASS << 28) | 0x0017FFFF)
#define MC_CHANNEL_COLORSCAN		((MC_CHANNEL_CLASS << 28) | 0x001DFFFF)
#define MC_CHANNEL_HARMONY			((MC_CHANNEL_CLASS << 28) | 0x001EFFFF)
#define MC_CHANNEL_MELODY			((MC_CHANNEL_CLASS << 28) | 0x001FFFFF)
#define MC_CHANNEL_DELTA_X1 		((MC_CHANNEL_CLASS << 28) | 0x0020FFFF)
#define MC_CHANNEL_DELTA_X2 		((MC_CHANNEL_CLASS << 28) | 0x0021FFFF)
#define MC_CHANNEL_DELTA_Y1 		((MC_CHANNEL_CLASS << 28) | 0x0022FFFF)
#define MC_CHANNEL_DELTA_Y2 		((MC_CHANNEL_CLASS << 28) | 0x0023FFFF)
#define MC_CHANNEL_AVENUE			((MC_CHANNEL_CLASS << 28) | 0x0024FFFF)


// DEFAULT PROCESSOR FUNCTION
#define MC_ProcessType_BYPASS		0

#if !defined(MC_DYNAMIC_BINDING)

// STATIC INSTANCE OBJECTS
// FUNCTIONS

// Driver connections
MCSTATUS DLL_API McOpenDriver(PCCHAR InstanceName);
MCSTATUS DLL_API McCloseDriver(void);

// Object creation / deletion
MCSTATUS DLL_API McCreate(MCHANDLE ModelInstance, PMCHANDLE Instance);
MCSTATUS DLL_API McCreateNm(PCCHAR ModelInstanceName, PMCHANDLE Instance);
MCSTATUS DLL_API McDelete(MCHANDLE Instance);

// Acquisition control
MCSTATUS DLL_API McStartAcq(void);
MCSTATUS DLL_API McStopAcq(void);

// Parameter handling
MCSTATUS DLL_API McSetParamInt(MCHANDLE Instance, MCPARAMID Param, INT32 ValueInt);
MCSTATUS DLL_API McSetParamNmInt(MCHANDLE Instance, PCCHAR ParamName, INT32 ValueInt);
MCSTATUS DLL_API McSetParamStr(MCHANDLE Instance, MCPARAMID Param, PCCHAR ValueStr);
MCSTATUS DLL_API McSetParamNmStr(MCHANDLE Instance, PCCHAR ParamName, PCCHAR ValueStr);
MCSTATUS DLL_API McSetParamFloat(MCHANDLE Instance, MCPARAMID Param, FLOAT64 ValueFloat);
MCSTATUS DLL_API McSetParamNmFloat(MCHANDLE Instance, PCCHAR ParamName, FLOAT64 ValueFloat);
MCSTATUS DLL_API McSetParamInst(MCHANDLE Instance, MCPARAMID Param, MCHANDLE ValueInst);
MCSTATUS DLL_API McSetParamNmInst(MCHANDLE Instance, PCCHAR ParamName, MCHANDLE ValueInst);

MCSTATUS DLL_API McGetParamInt(MCHANDLE Instance, MCPARAMID Param, PINT32 ValueInt);
MCSTATUS DLL_API McGetParamNmInt(MCHANDLE Instance, PCCHAR ParamName, PINT32 ValueInt);
MCSTATUS DLL_API McGetParamStr(MCHANDLE Instance, MCPARAMID Param, PCHAR ValueStr, UINT32 MaxLength);
MCSTATUS DLL_API McGetParamNmStr(MCHANDLE Instance, PCCHAR ParamName, PCHAR ValueStr, UINT32 MaxLength);
MCSTATUS DLL_API McGetParamFloat(MCHANDLE Instance, MCPARAMID Param, PFLOAT64 ValueFloat);
MCSTATUS DLL_API McGetParamNmFloat(MCHANDLE Instance, PCCHAR ParamName, PFLOAT64 ValueFloat);
MCSTATUS DLL_API McGetParamInst(MCHANDLE Instance, MCPARAMID Param, PMCHANDLE ValueInst);
MCSTATUS DLL_API McGetParamNmInst(MCHANDLE Instance, PCCHAR ParamName, PMCHANDLE ValueInst);

// Callback registration for an object
MCSTATUS DLL_API McRegisterCallback(MCHANDLE Instance, PMCCALLBACK CallBackFunction, PVOID Context);
MCSTATUS DLL_API McRegisterCallbackEx(MCHANDLE Instance, PMCCALLBACK CallBackFunction, PMCCALLBACKEX ThreadStart, PMCCALLBACKEX ThreadEnd, PVOID Context);

// Event Management
MCSTATUS DLL_API McWaitSignal(MCHANDLE Instance, MCSIGNAL Signal, UINT32 Timeout, PMCSIGNALINFO Info);
MCSTATUS DLL_API McGetSignalInfo(MCHANDLE Instance, MCSIGNAL Signal, PMCSIGNALINFO Info);

#endif	// MC_DYNAMIC_BINDING

#ifdef __cplusplus
};	// extern "C"
#endif // __cplusplus

#endif // _MULTICAM_
