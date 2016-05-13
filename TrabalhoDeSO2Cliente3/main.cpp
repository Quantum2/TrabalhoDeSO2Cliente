#include <iostream>
#include <io.h>
#include <fcntl.h>
#include "Cliente.h"

#define cout wcout

using namespace std;

int main() {
	//_setmode(_fileno(stdout), _O_U16TEXT);

	Cliente cliente;
	cliente.connect();
	
	while (1) {
		string temp;
		cout << "Mensagem a enviar: ";
		cin >> temp;
		if (temp == "exit") {
			cliente.disconect();
			break;
		}
		else
			cliente.enviarMensagem(temp);
	}

	return 0;
}