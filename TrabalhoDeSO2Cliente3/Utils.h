#pragma once
#include <vector>
#include <iostream>
#include <string>

using namespace std;

class Jogador
{
private:

public:

};

class Labirinto
{
private:

public:

};

class Jogo {
private:
	int estado = 1;
	Labirinto lab;
	vector<Jogador> jogadores;
public:
	Jogo();
	~Jogo();

	string lastCommand;

	const int A_PROCURAR_CLIENTES = 1;
	const int A_INICIAR_JOGO = 2;
	const int A_JOGAR = 3;

	void setEstado(int e);
	int getEstado();
};

struct Mensagem {
	int pid;
	char msg[BUFSIZ];
};