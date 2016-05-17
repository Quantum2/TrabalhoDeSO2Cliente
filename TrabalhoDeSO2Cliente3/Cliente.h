#pragma once
#include <windows.h> 
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include <atlstr.h>
#include <string>
#include "Consola.h"
#include "Utils.h"

#define BUFSIZE 512

using namespace std;

class Cliente
{
private:
	HANDLE hPipe;
	HANDLE hPipeGeral;
	LPTSTR lpvMessage;
	Mensagem chBuf;
	BOOL   fSuccess;
	DWORD  cbRead, cbToWrite, cbWritten, dwMode;
	LPTSTR lpszPipename = TEXT("\\\\.\\pipe\\mynamedpipe");
	LPTSTR lpszPipenameListener = TEXT("\\\\.\\pipe\\pipeGeral");

	Jogo jogo;

	static void compareBuffer(Mensagem buffer);
	void iniciarListener();
	static DWORD WINAPI threadListener(LPVOID lpvParam);
public:
	Cliente();
	~Cliente();

	int connect();
	int connect(string texto);
	void enviarMensagem(Mensagem m);
	void disconect();
};
