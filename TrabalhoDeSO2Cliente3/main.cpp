#pragma once
#include <iostream>
#include <io.h>
#include <fcntl.h>
#include "Consola.h"
#include "Cliente.h"
#include "Utils.h"

#define cout wcout

using namespace std;

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