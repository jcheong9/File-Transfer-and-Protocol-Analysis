#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <windows.h>

typedef struct
{
	LPSTR data;
	char * filePath;
} UPLOADFILE;

void upload_file(HWND hwnd, UPLOADFILE * data);
