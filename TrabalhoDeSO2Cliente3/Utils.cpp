#include "Utils.h"

Jogo::Jogo()
{
}

Jogo::~Jogo()
{
}

void Jogo::setEstado(int e)
{
	estado = e;
}

int Jogo::getEstado()
{
	return estado;
}

Jogador::Jogador(string nome, int p)
{
	name = nome;
	pid = p;
}

Jogador::~Jogador()
{
}
