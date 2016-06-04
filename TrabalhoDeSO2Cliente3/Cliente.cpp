#include "Cliente.h"

#define cout wcout
#define printf wprintf

static string anteriorM;
static int modoJogo = 0;

void Cliente::compareBuffer(Mensagem buffer)
{
	string temp = buffer.msg;
	string entrada;

	if (temp == "EM FASE DE JOGO!!!" && anteriorM != temp)
	{
		cout << "Modo de jogo";
		modoJogo = 1;
	}
	if (temp == "EM FASE DE SAIR!!!" && anteriorM != temp) {
		modoJogo = 2;
	}
	anteriorM = buffer.msg;
}

Cliente::Cliente()
{
	modoJogo = 0;
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
			printf(L"Could not open pipe: 20 second wait timed out.");
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

	printf(L"\nMessage sent to server, receiving reply as follows:\n");

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

		printf(L"\"%s\"\n", chBuf.msg);
	} while (!fSuccess);  // repeat loop if ERROR_MORE_DATA 

	if (!fSuccess)
	{
		_tprintf(TEXT("ReadFile from pipe failed. GLE=%d\n"), GetLastError());
		return -1;
	}

	printf(L"\n<End of message, press ENTER to terminate connection and exit>");
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
			printf(L"Could not open pipe: 20 second wait timed out.");
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

	return 0;
}

void Cliente::enviarMensagem(Mensagem m) {
	// Send a message to the pipe server. 
	cbToWrite = sizeof(m);
	cout << "Sending " << cbToWrite << " byte message: " << m.msg << endl;

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
	printf(L"\nMessage sent to server, receiving reply as follows:\n");

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
		cout << chBuf.msg << endl;
	} while (!fSuccess);  // repeat loop if ERROR_MORE_DATA 

	if (!fSuccess)
	{
		_tprintf(TEXT("ReadFile from pipe failed. GLE=%d\n"), GetLastError());
		return;
	}
}

void Cliente::disconect() {
	printf(L"\n<End of message, press ENTER to terminate connection and exit>");
	_getch();

	CloseHandle(hPipe);
}

int Cliente::getModoJogo()
{
	return modoJogo;
}
