#include <iostream>
#include <io.h>
#include <fcntl.h>
#include "Cliente.h"
#include "Utils.h"

#define cout wcout

using namespace std;

int main() {
	//_setmode(_fileno(stdout), _O_U16TEXT);
	int pid = GetCurrentProcessId();

	Cliente cliente;
	cliente.connect();
	
	while (1) {
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

	return 0;
}