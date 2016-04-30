#pragma once
#include <windows.h> 
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include <atlstr.h>
#include <string>

#define BUFSIZE 512

using namespace std;

class Cliente
{
public:
	Cliente();
	~Cliente();

	int connect();
	int connect(string texto);
};

