#include "upload_file.h"
using namespace std;


void upload_file(HWND hwnd, UPLOADFILE * uploadData) {
	OPENFILENAME ofn;
	char file_name[100];
	ZeroMemory(&ofn, sizeof(OPENFILENAME));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFile = file_name;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = 100;
	ofn.lpstrFilter = "All files\0*.*\0Text\0*.TXT\0";
	ofn.nFilterIndex = 1;

	GetOpenFileNameA(&ofn);
	uploadData->filePath = ofn.lpstrFile;
	strcpy(uploadData->filePath, ofn.lpstrFile);
	MessageBox(NULL, uploadData->filePath, "", MB_OK);
	ifstream infile(ofn.lpstrFile);
	string line;
	stringstream stream;
	string indexTxt;
	LPSTR sti;

	if (infile.is_open()) {
		while (!infile.eof()) {
			stream.clear();
			getline(infile, line);
		}
	}
	sti = (LPSTR) line.c_str();
	uploadData->data = sti;
	MessageBox(NULL, uploadData->data, "", MB_OK);
	//OutputDebugString(indexTxt);
	//return data;
}

