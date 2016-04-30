#include <iostream>
#include "Cliente.h"

using namespace std;

int main() {
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