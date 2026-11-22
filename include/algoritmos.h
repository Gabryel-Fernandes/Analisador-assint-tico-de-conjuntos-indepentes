#ifndef ALGORITMOS_H
#define ALGORITMOS_H

#include "grafo.h"

extern long long contador_instrucoes;

int forcaBruta(Grafo *g, int melhor_conjunto[]);
int backtracking(Grafo *g, int melhor_conjunto[]);
int divisaoEConquista(Grafo *g, int melhor_conjunto[]);
int programacaoDinamica(Grafo *g, int melhor_conjunto[]);
int guloso(Grafo *g, int melhor_conjunto[]);
int heuristicaBuscaLocal(Grafo *g, int melhor_conjunto[]);

#endif