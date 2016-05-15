#include <iostream>
#include <io.h>
#include <fcntl.h>
#include <signal.h>
#include "Cliente.h"
#include "Utils.h"

#define cout wcout

using namespace std;

void SignalHandler(int signal)
{
	if (signal == SIGABRT) {
		// abort signal handler code
	}
	if (signal == SIGFPE) {
		// stuff here
	}
	if (signal == SIGINT) {

	}
	else {
		// ...
	}
}

int main() {
	//_setmode(_fileno(stdout), _O_U16TEXT);

	signal(SIGABRT, SignalHandler);
	signal(SIGFPE, SignalHandler);
	signal(SIGINT, SignalHandler);

	int pid = GetCurrentProcessId();

	Cliente cliente;
	cliente.connect();
	
	while (1) {
		string temp;
		Mensagem mensa;
		cout << "Mensagem a enviar: ";
		cin >> temp;

		mensa.pid = pid;
		strcpy(mensa.msg, temp.c_str());

		if (temp == "exit") {
			cliente.disconect();
			break;
		}
		else
			cliente.enviarMensagem(temp);
	}

	return 0;
}