#ifndef D3D_BASE_DEFINE_H_
#define D3D_BASE_DEFINE_H_

#ifdef _DEBUG
#define D3D_BASE_DEBUG
#endif

#ifdef D3D_BASE_DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )

#endif // D3D_BASE_DEBUG

#endif // D3D_BASE_DEFINE_H_
