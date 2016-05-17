#pragma once
#include <vector>
#include <iostream>
#include <string>

#define BUFSIZE 1024

using namespace std;

class Jogador
{
private:
	string name;
	int pid;
public:
	Jogador(string nome, int p);
	~Jogador();
	int getPid();
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
public:
	Jogo();
	~Jogo();

	string lastCommand;
	vector<Jogador> jogadores;

	const int A_PROCURAR_CLIENTES = 1;
	const int A_INICIAR_JOGO = 2;
	const int A_JOGAR = 3;

	void setEstado(int e);
	int getEstado();
};

struct Mensagem {
	int pid;
	char msg[BUFSIZE];
	int pidsEnviar[128];
};