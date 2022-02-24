#include "Socket.h"
#include "Task.h"
#include "Logger.h"

#include <sstream>
#include <map>

#ifdef _WIN32
	#include <conio.h>
#else
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

int kbhit(void){
	struct termios oldt, newt;
	int ch;
	int oldf;

	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

	ch = getchar();

	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	fcntl(STDIN_FILENO, F_SETFL, oldf);

	if(ch != EOF)
	{
	ungetc(ch, stdin);
	return 1;
	}

	return 0;
}

char getch(){
	return (char)getchar();
}

#endif

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
	int frameDataType;
	int frameDataTypeSz;
	size_t frameRows = 0;
	size_t frameCols = 0;
} FrameHeader, *pFrameHeader;

#pragma once
