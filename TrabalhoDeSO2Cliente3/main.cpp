#pragma once
#include <iostream>
#include <io.h>
#include <sstream>
#include <fcntl.h>
#include <windows.h>
#include <mutex>
#include <cstdlib>
#include <tchar.h>		
#include "Cliente.h"
#include "Utils.h"
#include "resource.h"
#include "Jogo.h"
#pragma comment(lib, "Msimg32.lib")

#define TAM 255
#define RES_X 800
#define RES_Y 600
#define TIME_TO_REFRESH 250
#define cout wcout

using namespace std;

enum { ID_PLAY, ID_EXIT };

mutex mtx;

int coordCantoX = 0;
int coordCantoY = 0;

TCHAR szProgName[] = TEXT("MostrarMessageBox");
Cliente cliente;
HWND hwndButton;
HWND hwndButton2;

HBITMAP hBitmap = NULL; 
HBITMAP hBitmap2 = NULL; 
HBITMAP hBitmap3 = NULL; 
HBITMAP hBitmap4 = NULL; 

HINSTANCE inst = NULL;

void actualizarMapa(HWND hw) {
	PAINTSTRUCT PtStc;
	BITMAP 		bitmap;
	HDC			hdc;
	HDC 		hdcMem;
	HDC			hdcBuffer;
	HGDIOBJ 	oldBitmap;
	Mensagem mensa;
	Mapa mapa;
	int x, y;

	mensa.pid = _getpid();
	strcpy(mensa.msg, "actualizar");

	mtx.lock();
	cliente.enviarMensagem(mensa);
	mapa = cliente.getMapa();
	mtx.unlock();

	InvalidateRect(hw, NULL, 1);
	hdc = BeginPaint(hw, &PtStc);
	hdcMem = CreateCompatibleDC(hdc);
	hdcBuffer = CreateCompatibleDC(hdc);
	x = 0;

	for (size_t i = coordCantoX; i < TAM_LABIRINTO; i++)
	{
		int counter = 0;
		char* temp;

		y = 0;
		temp = mapa.mapaEnv[i];
		string tempS(temp);
		string buf; // Have a buffer string
		stringstream ss(tempS); // Insert the string into a stream
		vector<string> tokens; // Create vector to hold our words

		while (ss >> buf)
			tokens.push_back(buf);

		for (size_t j = coordCantoY; j < tokens.size(); j++)
		{
			if (tokens[j] == "_")
			{
				//desenhar bitmaps a partir daqui
				oldBitmap = SelectObject(hdcMem, hBitmap2);
				GetObject(hBitmap2, sizeof(bitmap), &bitmap);
				BitBlt(hdc, x, y, bitmap.bmWidth, bitmap.bmHeight, hdcMem, 0, 0, SRCCOPY);
			}
			if (tokens[j] == "M")
			{
				//desenhar bitmaps a partir daqui
				oldBitmap = SelectObject(hdcMem, hBitmap3);
				GetObject(hBitmap3, sizeof(bitmap), &bitmap);
				TransparentBlt(hdc, x, y, bitmap.bmWidth, bitmap.bmHeight, hdcMem, 0, 0, 50, 50, RGB(251, 251, 251));
			}
			if (tokens[j] == "J")
			{
				//desenhar bitmaps a partir daqui
				oldBitmap = SelectObject(hdcMem, hBitmap);
				GetObject(hBitmap, sizeof(bitmap), &bitmap);
				TransparentBlt(hdc, x, y, bitmap.bmWidth, bitmap.bmHeight, hdcMem, 0, 0, 50, 50, RGB(255,255,255));
			}
			y = y + 50;
		}
		x = x + 50;
	}

	//Imprimir topo
	wstring outCoords = L"Coordenadas X: ";
	outCoords.append(to_wstring(coordCantoX));
	outCoords.append(L"  Y: ");
	outCoords.append(to_wstring(coordCantoY));
	TextOut(hdc, 0, 0, outCoords.c_str(), outCoords.size());

	wstring vidaJog = L"Vida do jogador: ";
	vidaJog.append(to_wstring(cliente.vida));
	TextOut(hdc, 0, 16, vidaJog.c_str(), vidaJog.size());

	// Transfer the off-screen DC to the screen
	BitBlt(hdc, 0, 0, RES_X, RES_Y, hdcBuffer, 0, 0, SRCCOPY);
	
	// Free-up the off-screen DC
	SelectObject(hdcMem, oldBitmap);
	DeleteDC(hdcMem);
	DeleteDC(hdcBuffer);
	EndPaint(hw, &PtStc);
}

DWORD WINAPI thread_func(LPVOID lpParameter)
{
	HWND hw = (HWND)lpParameter;
	Mensagem mensa;
	Mapa temp1, temp2;
	mensa.pid = _getpid();
	strcpy_s(mensa.msg, "actualizar");

	while (true)
	{
		mtx.lock();
		cliente.enviarMensagem(mensa);
		mtx.unlock();
		temp1 = cliente.getMapa();
		_sleep(TIME_TO_REFRESH);
		mtx.lock();
		cliente.enviarMensagem(mensa);
		mtx.unlock();
		temp2 = cliente.getMapa();

		for (size_t i = 0; i < TAM_LABIRINTO; i++)
		{
			if (strcmp(temp1.mapaEnv[i], temp2.mapaEnv[i]) != 0)
			{
				actualizarMapa(hw);
			}
		}
	}
}

void configurarMenuInicial(HWND hw) {
	DestroyWindow(hwndButton);
	DestroyWindow(hwndButton2);

	PAINTSTRUCT PtStc;
	InvalidateRect(hw, NULL, 1);
	HDC hdc = BeginPaint(hw, &PtStc);
	HBRUSH NewBrush = CreateSolidBrush(RGB(0, 0, 0));

	SelectObject(hdc, NewBrush);
	Rectangle(hdc, 0, 0, 800, 600);
	DeleteObject(NewBrush);
	EndPaint(hw, &PtStc);

	CreateThread(NULL, 0, thread_func, hw, 0, 0);
	actualizarMapa(hw);
}

// ============================================================================
// FUN��O DE PROCESSAMENTO DA JANELA
// Neste exemplo vamos processar as mensagens:
//		WM_CHAR				Guardar o caracter digitado no buffer (sem mostrar)
//							Chamar InvalidateRect() para originar WM_PAINT
//		WM_PAINT			Mostrar o conte�do do buffer com TextOut()
// ============================================================================
LRESULT CALLBACK WndProc(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam)   //trata da janela principal 
{
	HDC hdc;				// handler para um Device Context - superficie de desenho
	TCHAR str[TAM];			// Buffer para caracteres recolhidos do teclado
	static int resposta;	// Resultado de resposta � MsgBox
	PAINTSTRUCT PtStc;		// Ponteiro para estrutura de WM_PAINT
	Mensagem mensa;         // Stuff to send
	mensa.pid = _getpid();  // Moar

							// Processamento das mensagens
	switch (messg) {
		//==============================================================================
		// Recolher caracteres digitados num buffer e mostrar numa caixa de mensagens
		// int resposta= MessageBox(handler_janela, string_texto, T�tulo, Estilo)
		// Estilos das caixas de mensagens (podem ser ORED (|) entre si)
		//		MB_ABORTRETRYIGINORE, MB_ICONEXCLAMATION, MB_ICONINFORMATION, 
		//		MB_ICONQUESTION, MB_OK, MB_OKCANCEL, MB_RETRYCANCEL, MB_YESNO
		//		MB_YESNOCANCEL
		// Valores de retorno:
		//		IDABORT, IDRETRY, IDIGNORE, IDCANCEL, IDNO, IDYES, IDOK
		//==============================================================================
		// Mensagem WM_CHAR: Caracter (normal) foi digitado no teclado
		//WM_KEYDOWN -  setas do teclado 
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_LEFT:
			// Process the LEFT ARROW key. 
			strcpy_s(mensa.msg, "esquerda");
			cliente.enviarMensagem(mensa);
			actualizarMapa(hWnd);
			break;
		case VK_RIGHT:
			strcpy(mensa.msg, "direita");
			cliente.enviarMensagem(mensa);
			actualizarMapa(hWnd);
			break;
		case VK_UP:
			strcpy(mensa.msg, "cima");
			cliente.enviarMensagem(mensa);
			actualizarMapa(hWnd);
			break;
		case VK_DOWN:
			strcpy(mensa.msg, "baixo");
			cliente.enviarMensagem(mensa);
			actualizarMapa(hWnd);
			break;
		case VK_ESCAPE:
			strcpy_s(mensa.msg, "logout");
			cliente.enviarMensagem(mensa);
			exit(0);
			break;
		default:
			break;
		}
	case WM_CHAR:
		//TOGGLES DO JOGADOR - armas, atacar - ficam aqui  
		/*if (wParam=='P' || wParama=='p') 
		{
			strcpy(mensa.msg, "pedra");
			cliente.enviarMensagem(mensa);
		}
		 else if (wParam == 'M' || wParam == 'm')
		 {
			strcpy(mensa.msg, "machado");
			cliente.enviarMensagem(mensa);
		 }*/
		  if (wParam == ' ')  {
			strcpy(mensa.msg, "atacar");
			cliente.enviarMensagem(mensa);
		 }
		 
		if (wParam == 'W' || wParam == 'w')
		{
			if (coordCantoY > 0)
			{
				coordCantoY--;
				actualizarMapa(hWnd);
			}
		}
		if (wParam == 'S' || wParam == 's')
		{
			if (coordCantoY < TAM_LABIRINTO - 12)
			{
				coordCantoY++;
				actualizarMapa(hWnd);
			}
		}
		if (wParam == 'A' || wParam == 'a')
		{
			if (coordCantoX > 0)
			{
				coordCantoX--;
				actualizarMapa(hWnd);
			}
		}
		if (wParam == 'D' || wParam == 'd')
		{
			if (coordCantoX < TAM_LABIRINTO - 16)
			{
				coordCantoX++;
				actualizarMapa(hWnd);
			}
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &PtStc);
		break;
	case WM_DESTROY:
		DeleteObject(hBitmap);
		PostQuitMessage(0);
		break;
		// Restantes mensagens t�m processamento default
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_PLAY:
			Mensagem mensa;
			mensa.pid = _getpid();
			strcpy(mensa.msg, "jogar");
			configurarMenuInicial(hWnd);
			break;
		case ID_EXIT:
			exit(0);
			break;
		default:
			break;
		}
		break;
	default:
		return(DefWindowProc(hWnd, messg, wParam, lParam));
		break;
	}
	return(0);
}

LRESULT CALLBACK DlgProc(HWND hWndDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch (Msg)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		switch (wParam)
		{
		case IDOK:
			EndDialog(hWndDlg, 0);
			return TRUE;
		case IDC_BUTTON1:
			Mensagem mensa;
			TCHAR temp[TAM];
			string tempS;
			GetDlgItemText(hWndDlg, IDC_EDIT1, temp, sizeof(temp));

			mensa.pid = _getpid();
			wstring ws(temp);
			if (ws.empty()) {
				int msgboxID = MessageBox(
					NULL,
					(LPCWSTR)L"T�m de introduzir um username v�lido\nTente novamente",
					(LPCWSTR)L"Username inv�lido",
					MB_ICONWARNING | MB_OK | MB_DEFBUTTON2
					);

				switch (msgboxID)
				{
				case IDOK:
					return FALSE;
					break;
				}
			}

			tempS = "login " + string(ws.begin(), ws.end());
			
			strcpy_s(mensa.msg, tempS.c_str());
			
			cliente.enviarMensagem(mensa);
			EndDialog(hWndDlg, NULL);

			LPWSTR msgAnterior = TEXT("TESTESTESTETSTWTSADSADASDASDASD");
			msgAnterior = convertCharArrayToLPCWSTR(cliente.anteriorM.c_str());

			int msgboxID = MessageBox(
				NULL,
				msgAnterior,
				(LPCWSTR)L"Info de jogador",
				MB_ICONINFORMATION | MB_OK | MB_DEFBUTTON2
				);

			switch (msgboxID)
			{
			case IDOK:
				return TRUE;
				break;
			}
			
			return TRUE;
		}
		break;
	}

	return FALSE;
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow)
 {
	int res = cliente.connect();
	int pid = GetCurrentProcessId();
	inst = hInst;
	hBitmap = (HBITMAP)LoadImage(hInst, MAKEINTRESOURCE(IDB_BITMAP1), IMAGE_BITMAP, 0, 0, LR_SHARED);  //Load bitmaps here
	hBitmap2 = (HBITMAP)LoadImage(hInst, MAKEINTRESOURCE(IDB_BITMAP2), IMAGE_BITMAP, 0, 0, LR_SHARED);  //Load bitmaps here
	hBitmap3 = (HBITMAP)LoadImage(hInst, MAKEINTRESOURCE(IDB_BITMAP3), IMAGE_BITMAP, 0, 0, LR_SHARED);  //Load bitmaps here
	hBitmap4 = (HBITMAP)LoadImage(hInst, MAKEINTRESOURCE(IDB_BITMAP4), IMAGE_BITMAP, 0, 0, LR_SHARED);  //Load bitmaps here

	HWND hWnd;			// handler da janela (a gerar por CreateWindow())
	MSG lpMsg;			// Estrutura das mensagens
	WNDCLASSEX wcApp;	// Estrutura que define a classe da janela

						// ============================================================================
						// Caracter�sticas da janela "wcApp" 
						// ============================================================================

	wcApp.cbSize = sizeof(WNDCLASSEX);	// Tamanho da estrutura WNDCLASSEX
	wcApp.hInstance = hInst;			// Inst�ncia da janela actualmente exibida 
										// ("hInst" � par�metro de WinMain)
	wcApp.lpszClassName = szProgName;	// Nome da janela 
	wcApp.lpfnWndProc = WndProc;		// Endere�o da fun��o de processamento da janela 
	wcApp.style = 0;			// Fazer o redraw hor. e vert. se mudar
	wcApp.hIcon = LoadIcon(NULL, IDI_APPLICATION);	// �con normal=Aplica��o do Windows
	wcApp.hIconSm = LoadIcon(NULL, IDI_WINLOGO);	// �con pequeno=�con WinLogo
	wcApp.hCursor = LoadCursor(NULL, IDC_ARROW);	// rato = "seta"
	wcApp.lpszMenuName = NULL;						// Janela sem menus
	wcApp.cbClsExtra = 0;							// Livre, para uso particular
	wcApp.cbWndExtra = 0;							// Livre, para uso particular
	wcApp.hbrBackground = 0; //(HBRUSH)GetStockObject(WHITE_BRUSH);	// Fundo branco

																// ============================================================================
																// Registar a classe "wcApp" 
																// ============================================================================
	if (!RegisterClassEx(&wcApp))
		return(0);

	// ============================================================================
	// Criar a janela
	// ============================================================================
	hWnd = CreateWindow(
		szProgName,				// Nome da janela e/ou programa
		TEXT("Trabalho de SO2"),	// T�tulo da janela
		WS_OVERLAPPEDWINDOW  ^ WS_THICKFRAME,	// Estilo da janela 
		CW_USEDEFAULT,			// Posi��o x 
		CW_USEDEFAULT,			// Posi��o y 
		RES_X,			// Largura 
		RES_Y,			// Altura 
		(HWND)HWND_DESKTOP,	// handle da janela pai (HWND_DESKTOP para 1�)
		(HMENU)NULL,			// handle do menu (se tiver menu)
		(HINSTANCE)hInst,			// handle da inst�ncia actual (vem de WinMain())
		(LPSTR)NULL);			// N�o h� par�metros adicionais 

								// ============================================================================
								// Mostrar a janela
								// ============================================================================
	ShowWindow(hWnd, nCmdShow);	// "hWnd"= handler da janela
								// "nCmdShow"= modo, par�metro de WinMain()
	UpdateWindow(hWnd);			// Refrescar a janela 

								// ============================================================================
								// Loop de Mensagens
								// ============================================================================
								// Par�metros de "getMessage":
								//  1)	"&lpMsg"=Endere�o de uma estrutura do tipo MSG ("MSG lpMsg" ja foi 
								//		declarada no in�cio de WinMain()):
	DialogBox(hInst, MAKEINTRESOURCE(IDD_LOGIN), hWnd, reinterpret_cast<DLGPROC>(DlgProc));

	RECT tamJanelaMain;
	GetWindowRect(hWnd, &tamJanelaMain);

	hwndButton = CreateWindow(
		L"BUTTON",  // Predefined class; Unicode assumed 
		L"INICIAR JOGO",      // Button text 
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
		350,         // x position 
		260,         // y position 
		100,        // Button width
		80,        // Button height
		hWnd,     // Parent window
		(HMENU)ID_PLAY,       // No menu.
		(HINSTANCE)GetWindowLong(hWnd, GWLP_HINSTANCE),	NULL);      // Pointer not needed.
	hwndButton2 = CreateWindow(
		L"BUTTON",  // Predefined class; Unicode assumed 
		L"SAIR",      // Button text 
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
		350,         // x position 
		260 + 100,         // y position 
		100,        // Button width
		80,        // Button height
		hWnd,     // Parent window
		(HMENU)ID_EXIT,       // No menu.
		(HINSTANCE)GetWindowLong(hWnd, GWLP_HINSTANCE),	NULL);      // Pointer not needed.

	PAINTSTRUCT PtStc;
	InvalidateRect(hWnd, NULL, 1);
	HDC hdc = BeginPaint(hWnd, &PtStc);
	if (res != 0) {
		TextOut(hdc, 0, 0, TEXT("Conex�o ao servidor falhada     "), strlen("Conex�o ao servidor falhada     "));
		EnableWindow(hwndButton, false);
	}
	else
	{
		TextOut(hdc, 0, 0, TEXT("Ligado ao servidor de jogo     "), strlen("Ligado ao servidor de jogo     "));
	}
	EndPaint(hWnd, &PtStc);

	while (GetMessage(&lpMsg, NULL, 0, 0)) {
		TranslateMessage(&lpMsg);			// Pr�-processamento da mensagem
		DispatchMessage(&lpMsg);			// Enviar a mensagem traduzida de volta
											// ao Windows
	}

	// ============================================================================
	// Fim do programa
	// ============================================================================
	return((int)lpMsg.wParam);	// Status = Par�metro "wParam" da estrutura "lpMsg"
}