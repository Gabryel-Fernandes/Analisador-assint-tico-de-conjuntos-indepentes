#ifndef GRAFO_H
#define GRAFO_H

#include <stdbool.h>

#define MAX_V 500
#define MAX_DP_V 28

typedef struct {
    int V;
    int adj[MAX_V][MAX_V];
} Grafo;

void inicializarGrafo(Grafo *g, int V);
void adicionarAresta(Grafo *g, int u, int v);
void carregarGrafoDidatico(Grafo *g);
void carregarGrafoDesconexo(Grafo *g);
void gerarGrafoAleatorio(Grafo *g, int V, int probabilidade_aresta);
void imprimirMatriz(Grafo *g);
bool ehIndependente(Grafo *g, int subconjunto[], int tamanho);

#endif