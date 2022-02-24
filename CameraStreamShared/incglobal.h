#include "Socket.h"
#include "Task.h"
#include "Logger.h"

#include <sstream>
#include <map>
#include <conio.h>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui_c.h>

#define	BROADCAST_PORT	21123
#define	CONNECT_PORT	21124

#define FRAME_HDR_ID	0
#define FRAME_CHNK_ID	1
#define FRAME_END_ID	2

#ifdef _WIN32
#	define kbhit	_kbhit
#	define getch	_getch
#endif

typedef struct _framehdr {
	char id = FRAME_HDR_ID;
	int frameDataType;
	int frameDataTypeSz;
	size_t frameRows = 0;
	size_t frameCols = 0;
} FrameHeader, *pFrameHeader;

#pragma once
