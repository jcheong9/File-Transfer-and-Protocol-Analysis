#pragma once
#include "common.h"


#define WM_SOCKET (WM_USER + 1)

void serverMainTCP(PVOID portParma);
void disconnectSocketServerTCP(SOCKET siServer);

