#pragma once
#include <iostream>
#include <io.h>
#include <fcntl.h>
#include <windows.h>
#include <tchar.h>		// Para chamada à função "sprintf" 
#include "Consola.h"
#include "Cliente.h"
#include "Utils.h"

#define TAM 255
#define cout wcout

using namespace std;

TCHAR szProgName[] = TEXT("MostrarMessageBox");

int main() {
	_setmode(_fileno(stdout), _O_U16TEXT);
	int pid = GetCurrentProcessId();

	Consola consola;
	Cliente cliente;
	cliente.connect();
	
	while (1) {
		if (cliente.getModoJogo() == 0) {
			string temp;
			Mensagem mensa;
			cout << "\n>> ";
			getline(cin, temp);

			mensa.pid = pid;
			strcpy_s(mensa.msg, temp.c_str());

			if (temp == "exit") {
				cliente.disconect();
				break;
			}
			else
				cliente.enviarMensagem(mensa);
		}
		if (cliente.getModoJogo() == 1) {
			//do game stuff here...
			consola.clrscr();
			cout << "Modo de jogo";
		}
		if (cliente.getModoJogo() == 2) {
			string temp;
			consola.clrscr();
			cout << "Fase de jogo terminada" << endl << "Pressione qualquer tecla para sair" << endl;
			cin >> temp;
			exit(0);
		}
	}

	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;				// handler para um Device Context
	TCHAR str[TAM];			// Buffer para caracteres recolhidos do teclado
	static int resposta;	// Resultado de resposta à MsgBox
	PAINTSTRUCT PtStc;		// Ponteiro para estrutura de WM_PAINT

							// Processamento das mensagens
	switch (messg) {
		//==============================================================================
		// Recolher caracteres digitados num buffer e mostrar numa caixa de mensagens
		// int resposta= MessageBox(handler_janela, string_texto, Título, Estilo)
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
		// Limpar janela e forçar WM_PAINT
		InvalidateRect(hWnd, NULL, 1);

		break;
		//==============================================================================
		// Mostrar resposta dada à MsgBox
		//==============================================================================
	case WM_PAINT:
		// Afixar na janela a resposta que foi dada à MsgBox
		hdc = BeginPaint(hWnd, &PtStc);
		switch (resposta) {
		case IDYES:
			TextOut(hdc, 0, 0, TEXT("Confirmado     "), 15);
			break;
		case IDNO:
			TextOut(hdc, 0, 0, TEXT("Não confirmado "), 15);
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
		// Restantes mensagens têm processamento default
	default:
		return(DefWindowProc(hWnd, messg, wParam, lParam));
		break;
	}
	return(0);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow)
{
	HWND hWnd;			// handler da janela (a gerar por CreateWindow())
	MSG lpMsg;			// Estrutura das mensagens
	WNDCLASSEX wcApp;	// Estrutura que define a classe da janela

						// ============================================================================
						// Características da janela "wcApp" 
						// ============================================================================

	wcApp.cbSize = sizeof(WNDCLASSEX);	// Tamanho da estrutura WNDCLASSEX
	wcApp.hInstance = hInst;			// Instância da janela actualmente exibida 
										// ("hInst" é parâmetro de WinMain)
	wcApp.lpszClassName = szProgName;	// Nome da janela 
	wcApp.lpfnWndProc = WndProc;		// Endereço da função de processamento da janela 
	wcApp.style = CS_HREDRAW | CS_VREDRAW;			// Fazer o redraw hor. e vert. se mudar
	wcApp.hIcon = LoadIcon(NULL, IDI_APPLICATION);	// ícon normal=Aplicação do Windows
	wcApp.hIconSm = LoadIcon(NULL, IDI_WINLOGO);	// ícon pequeno=Ícon WinLogo
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
		TEXT("Mostrar Message Box"),	// Título da janela
		WS_OVERLAPPEDWINDOW,	// Estilo da janela 
		CW_USEDEFAULT,			// Posição x 
		CW_USEDEFAULT,			// Posição y 
		CW_USEDEFAULT,			// Largura 
		CW_USEDEFAULT,			// Altura 
		(HWND)HWND_DESKTOP,	// handle da janela pai (HWND_DESKTOP para 1ª)
		(HMENU)NULL,			// handle do menu (se tiver menu)
		(HINSTANCE)hInst,			// handle da instância actual (vem de WinMain())
		(LPSTR)NULL);			// Não há parâmetros adicionais 

								// ============================================================================
								// Mostrar a janela
								// ============================================================================
	ShowWindow(hWnd, nCmdShow);	// "hWnd"= handler da janela
								// "nCmdShow"= modo, parâmetro de WinMain()
	UpdateWindow(hWnd);			// Refrescar a janela 

								// ============================================================================
								// Loop de Mensagens
								// ============================================================================
								// Parâmetros de "getMessage":
								//  1)	"&lpMsg"=Endereço de uma estrutura do tipo MSG ("MSG lpMsg" ja foi 
								//		declarada no início de WinMain()):

	while (GetMessage(&lpMsg, NULL, 0, 0)) {
		TranslateMessage(&lpMsg);			// Pré-processamento da mensagem
		DispatchMessage(&lpMsg);			// Enviar a mensagem traduzida de volta
											// ao Windows
	}

	// ============================================================================
	// Fim do programa
	// ============================================================================
	return((int)lpMsg.wParam);	// Status = Parâmetro "wParam" da estrutura "lpMsg"
}

// ============================================================================
// FUNÇÂO DE PROCESSAMENTO DA JANELA
// Neste exemplo vamos processar as mensagens:
//		WM_CHAR				Guardar o caracter digitado nu buffer (sem mostrar)
//							Chamar InvalidateRect() para originar WM_PAINT
//		WM_PAINT			Mostrar o conteúdo do buffer com TextOut()
// ============================================================================
