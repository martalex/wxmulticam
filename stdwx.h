// wxWidgets precompiled / standard headers 
#include "wx/wxprec.h"

/// Debug memory allocation enhancement 
/// http://www.litwindow.com/Knowhow/wxHowto/wxhowto.html#debug_alloc 
#ifdef _DEBUG
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK ,__FILE__, __LINE__)
#else
#define DEBUG_NEW new
#endif

