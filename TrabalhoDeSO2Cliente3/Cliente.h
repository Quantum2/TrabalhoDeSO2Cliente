#pragma once
#include <windows.h> 
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include <atlstr.h>
#include <string>

#define BUFSIZE 512

using namespace std;

class Cliente
{
private:
	HANDLE hPipe;
	LPTSTR lpvMessage;
	TCHAR  chBuf[BUFSIZE];
	BOOL   fSuccess;
	DWORD  cbRead, cbToWrite, cbWritten, dwMode;
	LPTSTR lpszPipename = TEXT("\\\\.\\pipe\\mynamedpipe");
public:
	Cliente();
	~Cliente();

	int connect();
	int connect(string texto);

	void enviarMensagem(string texto);

	void disconect();
};

