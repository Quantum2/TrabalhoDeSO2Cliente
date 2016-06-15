#pragma once
#include <vector>
#include <iostream>
#include <string>
#include <Windows.h>

#define BUFSIZE 2048
#define TAM_LABIRINTO 20

using namespace std;

class Jogador
{
private:
	string name;
	int pid;
public:
	Jogador(string nome, int p);
	~Jogador();
	//int getPid();
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

struct Mapa {
	char mapaEnv[TAM_LABIRINTO][TAM_LABIRINTO * 3];
};

struct Mensagem { //cliente para servidor
	int pid;
	char msg[BUFSIZE];
	int vida;
	int pidsEnviar[128];
	Mapa mapa;
};

wchar_t *convertCharArrayToLPCWSTR(const char* charArray);