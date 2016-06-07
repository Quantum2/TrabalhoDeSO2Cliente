#pragma once

#include <iostream>
#include <io.h>
#include <fcntl.h>
#include <windows.h>
#include <tchar.h>		// Para chamada � fun��o "sprintf" 
#include "Cliente.h"
#include "Utils.h"
#include "resource.h"

#define TAM 255
#define cout wcout

using namespace std;

TCHAR szProgName[] = TEXT("MostrarMessageBox");
Cliente cliente;

void configurarMenuInicial() {

}

// ============================================================================
// FUN��O DE PROCESSAMENTO DA JANELA
// Neste exemplo vamos processar as mensagens:
//		WM_CHAR				Guardar o caracter digitado nu buffer (sem mostrar)
//							Chamar InvalidateRect() para originar WM_PAINT
//		WM_PAINT			Mostrar o conte�do do buffer com TextOut()
// ============================================================================
LRESULT CALLBACK WndProc(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;				// handler para um Device Context
	TCHAR str[TAM];			// Buffer para caracteres recolhidos do teclado
	static int resposta;	// Resultado de resposta � MsgBox
	PAINTSTRUCT PtStc;		// Ponteiro para estrutura de WM_PAINT

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
	case WM_CHAR:

		_stprintf_s(str, TAM / sizeof(TCHAR), TEXT("O caracter digitado foi '%c'"), wParam);
		// Activar Message Box (Yes, No, Cancel) e receber a resposta
		resposta = MessageBox(hWnd, str, TEXT("Caracter Recebido"), MB_YESNOCANCEL | MB_ICONQUESTION);
		// Limpar janela e for�ar WM_PAINT
		InvalidateRect(hWnd, NULL, 1);

		break;
		//==============================================================================
		// Mostrar resposta dada � MsgBox
		//==============================================================================
	case WM_PAINT:
		// Afixar na janela a resposta que foi dada � MsgBox
		hdc = BeginPaint(hWnd, &PtStc);
		switch (resposta) {
		case IDYES:
			TextOut(hdc, 0, 0, TEXT("Confirmado     "), 15);
			break;
		case IDNO:
			TextOut(hdc, 0, 0, TEXT("N�o confirmado "), 15);
			break;
		case IDCANCEL:
			TextOut(hdc, 0, 0, TEXT("Cancelado      "), 15);
			break;
		}
		EndPaint(hWnd, &PtStc);
		break;
		//==============================================================================
		// Terminar e Processamentos default
		//==============================================================================
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
		// Restantes mensagens t�m processamento default
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
			tempS = "login " + string(ws.begin(), ws.end());
			strcpy_s(mensa.msg, tempS.c_str());

			configurarMenuInicial();
			EndDialog(hWndDlg, NULL);

			return TRUE;
		}
		break;
	}

	return FALSE;
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow)
{
	cliente.connect();
	int pid = GetCurrentProcessId();

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
	wcApp.style = CS_HREDRAW | CS_VREDRAW;			// Fazer o redraw hor. e vert. se mudar
	wcApp.hIcon = LoadIcon(NULL, IDI_APPLICATION);	// �con normal=Aplica��o do Windows
	wcApp.hIconSm = LoadIcon(NULL, IDI_WINLOGO);	// �con pequeno=�con WinLogo
	wcApp.hCursor = LoadCursor(NULL, IDC_ARROW);	// rato = "seta"
	wcApp.lpszMenuName = NULL;						// Janela sem menus
	wcApp.cbClsExtra = 0;							// Livre, para uso particular
	wcApp.cbWndExtra = 0;							// Livre, para uso particular
	wcApp.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);	// Fundo branco

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
		WS_OVERLAPPEDWINDOW,	// Estilo da janela 
		CW_USEDEFAULT,			// Posi��o x 
		CW_USEDEFAULT,			// Posi��o y 
		CW_USEDEFAULT,			// Largura 
		CW_USEDEFAULT,			// Altura 
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

	HWND hwndButton = CreateWindow(
		L"BUTTON",  // Predefined class; Unicode assumed 
		L"INICIAR JOGO",      // Button text 
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
		int(tamJanelaMain.right / 2),         // x position 
		int(tamJanelaMain.bottom / 2),         // y position 
		100,        // Button width
		80,        // Button height
		hWnd,     // Parent window
		NULL,       // No menu.
		(HINSTANCE)GetWindowLong(hWnd, GWLP_HINSTANCE),
		NULL);      // Pointer not needed.
	HWND hwndButton2 = CreateWindow(
		L"BUTTON",  // Predefined class; Unicode assumed 
		L"SAIR",      // Button text 
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
		int(tamJanelaMain.right / 2),         // x position 
		int(tamJanelaMain.bottom / 2) + 100,         // y position 
		100,        // Button width
		80,        // Button height
		hWnd,     // Parent window
		NULL,       // No menu.
		(HINSTANCE)GetWindowLong(hWnd, GWLP_HINSTANCE),
		NULL);      // Pointer not needed.

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