#pragma once
#include <vector>

using namespace std;

class Jogador {};

class Labirinto {};

class Jogo {
private:
	int estado;
	Labirinto lab;
	vector<Jogador> jogadores;
public:
	Jogo();
	~Jogo();

	const int A_PROCURAR_CLIENTES = 1;
	const int A_INICIAR_JOGO = 2;
	const int A_JOGAR = 3;

	void setEstado(int e);
	int getEstado();
};