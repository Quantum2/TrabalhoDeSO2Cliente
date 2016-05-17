#include "Cliente.h"

static string anteriorM;

void Cliente::compareBuffer(Mensagem buffer)
{
	Consola consola;
	string temp = buffer.msg;
	string entrada;

	if (temp == "EM FASE DE JOGO!!!" && anteriorM != temp)
	{
		consola.clrscr();
		cout << "Modo de jogo";
	}
	anteriorM = buffer.msg;
}

void Cliente::iniciarListener()
{
	lpvMessage = TEXT("Default message from client.");
	fSuccess = FALSE;

	// Try to open a named pipe; wait for it, if necessary. 
	while (1)
	{
		hPipeGeral = CreateFile(
			lpszPipenameListener,   // pipe name 
			GENERIC_READ |  // read and write access 
			GENERIC_WRITE,
			0,              // no sharing 
			NULL,           // default security attributes
			OPEN_EXISTING,  // opens existing pipe 
			0,              // default attributes 
			NULL);          // no template file 
							// Break if the pipe handle is valid. 

		if (hPipeGeral != INVALID_HANDLE_VALUE)
			break;

		// Exit if an error other than ERROR_PIPE_BUSY occurs. 

		if (GetLastError() != ERROR_PIPE_BUSY)
		{
			_tprintf(TEXT("Could not open pipe. GLE=%d\n"), GetLastError());
			return;
		}

		// All pipe instances are busy, so wait for 20 seconds. 

		if (!WaitNamedPipe(lpszPipenameListener, 20000))
		{
			printf("Could not open pipe: 20 second wait timed out.");
			return;
		}
	}

	// The pipe connected; change to message-read mode. 
	dwMode = PIPE_READMODE_MESSAGE;
	fSuccess = SetNamedPipeHandleState(
		hPipeGeral,    // pipe handle 
		&dwMode,       // new pipe mode 
		NULL,          // don't set maximum bytes 
		NULL);         // don't set maximum time 
	if (!fSuccess)
	{
		_tprintf(TEXT("SetNamedPipeHandleState failed. GLE=%d\n"), GetLastError());
		return;
	}
}

Cliente::Cliente()
{
}

Cliente::~Cliente()
{
}

int Cliente::connect(string texto) {
	lpvMessage = TEXT("Default message from client.");
	fSuccess = FALSE;

	TCHAR temp[BUFSIZE];
	_tcscpy_s(temp, CA2T(texto.c_str()));                                //Nem sei o que fiz aqui, so sei que trabalha
	lpvMessage = temp;

	// Try to open a named pipe; wait for it, if necessary. 
	while (1)
	{
		hPipe = CreateFile(
			lpszPipename,   // pipe name 
			GENERIC_READ |  // read and write access 
			GENERIC_WRITE,
			0,              // no sharing 
			NULL,           // default security attributes
			OPEN_EXISTING,  // opens existing pipe 
			0,              // default attributes 
			NULL);          // no template file 

							// Break if the pipe handle is valid. 

		if (hPipe != INVALID_HANDLE_VALUE)
			break;

		// Exit if an error other than ERROR_PIPE_BUSY occurs. 

		if (GetLastError() != ERROR_PIPE_BUSY)
		{
			_tprintf(TEXT("Could not open pipe. GLE=%d\n"), GetLastError());
			return -1;
		}

		// All pipe instances are busy, so wait for 20 seconds. 

		if (!WaitNamedPipe(lpszPipename, 20000))
		{
			printf("Could not open pipe: 20 second wait timed out.");
			return -1;
		}
	}

	// The pipe connected; change to message-read mode. 

	dwMode = PIPE_READMODE_MESSAGE;
	fSuccess = SetNamedPipeHandleState(
		hPipe,    // pipe handle 
		&dwMode,  // new pipe mode 
		NULL,     // don't set maximum bytes 
		NULL);    // don't set maximum time 
	if (!fSuccess)
	{
		_tprintf(TEXT("SetNamedPipeHandleState failed. GLE=%d\n"), GetLastError());
		return -1;
	}

	// Send a message to the pipe server. 

	cbToWrite = (lstrlen(lpvMessage) + 1)*sizeof(TCHAR);
	_tprintf(TEXT("Sending %d byte message: \"%s\"\n"), cbToWrite, lpvMessage);

	fSuccess = WriteFile(
		hPipe,                  // pipe handle 
		lpvMessage,             // message 
		cbToWrite,              // message length 
		&cbWritten,             // bytes written 
		NULL);                  // not overlapped 

	if (!fSuccess)
	{
		_tprintf(TEXT("WriteFile to pipe failed. GLE=%d\n"), GetLastError());
		return -1;
	}

	printf("\nMessage sent to server, receiving reply as follows:\n");

	do
	{
		// Read from the pipe. 

		fSuccess = ReadFile(
			hPipe,    // pipe handle 
			&chBuf,    // buffer to receive reply 
			BUFSIZE*sizeof(TCHAR),  // size of buffer 
			&cbRead,  // number of bytes read 
			NULL);    // not overlapped 

		if (!fSuccess && GetLastError() != ERROR_MORE_DATA)
			break;

		printf("\"%s\"\n", chBuf.msg);
	} while (!fSuccess);  // repeat loop if ERROR_MORE_DATA 

	if (!fSuccess)
	{
		_tprintf(TEXT("ReadFile from pipe failed. GLE=%d\n"), GetLastError());
		return -1;
	}

	printf("\n<End of message, press ENTER to terminate connection and exit>");
	_getch();

	CloseHandle(hPipe);

	return 0;
}

int Cliente::connect() {
	HANDLE hThread = NULL;
	DWORD  dwThreadId = 0;
	fSuccess = FALSE;

	// Try to open a named pipe; wait for it, if necessary. 
	while (1)
	{
		hPipe = CreateFile(
			lpszPipename,   // pipe name 
			GENERIC_READ |  // read and write access 
			GENERIC_WRITE,
			0,              // no sharing 
			NULL,           // default security attributes
			OPEN_EXISTING,  // opens existing pipe 
			0,              // default attributes 
			NULL);          // no template file 
							// Break if the pipe handle is valid. 

		if (hPipe != INVALID_HANDLE_VALUE)
			break;

		// Exit if an error other than ERROR_PIPE_BUSY occurs. 

		if (GetLastError() != ERROR_PIPE_BUSY)
		{
			_tprintf(TEXT("Could not open pipe. GLE=%d\n"), GetLastError());
			return -1;
		}

		// All pipe instances are busy, so wait for 20 seconds. 

		if (!WaitNamedPipe(lpszPipename, 20000))
		{
			printf("Could not open pipe: 20 second wait timed out.");
			return -1;
		}
	}

	// The pipe connected; change to message-read mode. 
	dwMode = PIPE_READMODE_MESSAGE;
	fSuccess = SetNamedPipeHandleState(
		hPipe,    // pipe handle 
		&dwMode,  // new pipe mode 
		NULL,     // don't set maximum bytes 
		NULL);    // don't set maximum time 
	if (!fSuccess)
	{
		_tprintf(TEXT("SetNamedPipeHandleState failed. GLE=%d\n"), GetLastError());
		return -1;
	}

	iniciarListener();
	// Create a thread for this client. 
	hThread = CreateThread(
		NULL,              // no security attribute 
		0,                 // default stack size 
		threadListener,    // thread proc
		(LPVOID)hPipeGeral,// thread parameter 
		0,                 // not suspended 
		&dwThreadId);      // returns thread ID 

	return 0;
}

DWORD WINAPI Cliente::threadListener(LPVOID lpvParam)
{
	bool fSucess;
	Mensagem buf;
	DWORD readSize;

	
	while (true)
	{
		// Read from the pipe. 
		fSucess = ReadFile(
			(HANDLE)lpvParam,    // pipe handle 
			&buf,    // buffer to receive reply 
			sizeof(Mensagem),  // size of buffer 
			&readSize,  // number of bytes read 
			NULL);    // not overlapped 

		if (!fSucess && GetLastError() != ERROR_MORE_DATA)
			break;

		compareBuffer(buf);
	}

	return 1;
}

void Cliente::enviarMensagem(Mensagem m) {
	// Send a message to the pipe server. 
	cbToWrite = sizeof(m);
	printf("Sending %d byte message: \"%s\"\n", cbToWrite, m.msg);

	fSuccess = WriteFile(
		hPipe,                  // pipe handle 
		&m,                      // message 
		cbToWrite,              // message length 
		&cbWritten,             // bytes written 
		NULL);                  // not overlapped 

	if (!fSuccess)
	{
		_tprintf(TEXT("WriteFile to pipe failed. GLE=%d\n"), GetLastError());
		return;
	}
	printf("\nMessage sent to server, receiving reply as follows:\n");

	do
	{
		// Read from the pipe. 
		fSuccess = ReadFile(
			hPipe,    // pipe handle 
			&chBuf,    // buffer to receive reply 
			sizeof(Mensagem),  // size of buffer 
			&cbRead,  // number of bytes read 
			NULL);    // not overlapped 

		if (!fSuccess && GetLastError() != ERROR_MORE_DATA)
			break;

		compareBuffer(chBuf);
		printf("\%s\n", chBuf.msg);
	} while (!fSuccess);  // repeat loop if ERROR_MORE_DATA 

	if (!fSuccess)
	{
		_tprintf(TEXT("ReadFile from pipe failed. GLE=%d\n"), GetLastError());
		return;
	}
}

void Cliente::disconect() {
	printf("\n<End of message, press ENTER to terminate connection and exit>");
	_getch();

	CloseHandle(hPipe);
}