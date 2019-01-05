#ifndef _PREDEFINE_H_
#define _PREDEFINE_H_

#include "Common/XDefine.h"
#include "Common/XLog.h"
#include "Common/XAssert.h"
#include "Base/XObject.h"
#include "Base/XPoint.h"
#include "Base/XSize.h"
#include "Base/XRect.h"
#include "Base/XMath.h"
#include "Base/XTime.h"

#ifdef _DEBUG
#define _DEBUG_VALUE          1
#else
#define _DEBUG_VALUE          0
#endif

// platform, indicate the current platform type
#define PLATFORM                    PLATFORM_LINUX

// matrix calculate option
#define MATRIX_USE_16_ELEMENTS	    1

// generate texture with linear option
#define USE_LINEAR_TEXTURE		    1

// show the touch zone
#define SHOW_COLLISITION_ZONE       _DEBUG_VALUE

// render the game with OpenGL, not OpenGLes
#define RENDER_WITH_OPEN_GL         1

// use GLfixed as default OpenGL/es data type, 
// MUST BE 0 when RENDER_WITH_OPEN_GL is defined
#define USE_GL_FIXED                0

// use VOB(Vertex Buffer Object) to accelerate render
// MUST BE 0 when RENDER_WITH_OPEN_GL is defined
#define USE_VBO                     0

// manage sound resource with XResPhase
#define USE_RESPHASE_MANAGE_SOUND   0   

// log level
#ifdef _DEBUG
#define XLOG_LEVEL                 XLOG_LEVEL_ALL
#else
#define XLOG_LEVEL                 XLOG_LEVEL_ALL
#endif

#define STANDALONE_SERVER
//#define DEBUG_SELF_LAUNCH

#pragma warning(disable:4311)
#pragma warning(disable:4291)
#pragma warning(disable:4312)
#pragma warning(disable:4996)

#endif
