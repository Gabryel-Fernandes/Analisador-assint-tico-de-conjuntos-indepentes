#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "grafo.h"

void inicializarGrafo(Grafo *g, int V) {
    g->V = V;
    for (int i = 0; i < V; i++) {
        for (int j = 0; j < V; j++) {
            g->adj[i][j] = 0;
        }
    }
}

void adicionarAresta(Grafo *g, int u, int v) {
    if (u >= 0 && u < g->V && v >= 0 && v < g->V && u != v) {
        g->adj[u][v] = 1;
        g->adj[v][u] = 1;
    }
}

// Melhor Grafo (Ciclo de 4 vértices) -> Resposta otima: 2 ({0,2} ou {1,3})
void carregarGrafoDidatico(Grafo *g) {
    inicializarGrafo(g, 4);
    adicionarAresta(g, 0, 1);
    adicionarAresta(g, 1, 2);
    adicionarAresta(g, 2, 3);
    adicionarAresta(g, 3, 0);
    printf("\n[INFO] Grafo didatico carregado (4 vertices em ciclo: 0-1-2-3-0).\n");
}

// Grafo quebrado (2 componentes de 4 vértices) -> Ideal para Divisão e Conquista
void carregarGrafoDesconexo(Grafo *g) {
    inicializarGrafo(g, 8);

    adicionarAresta(g, 0, 1); adicionarAresta(g, 1, 2);
    adicionarAresta(g, 2, 3); adicionarAresta(g, 3, 0);

    adicionarAresta(g, 4, 5); adicionarAresta(g, 5, 6);
    adicionarAresta(g, 6, 7); adicionarAresta(g, 7, 4);
    printf("\n[INFO] Grafo desconexo carregado (2 componentes independentes de 4 vertices). Otima = 4.\n");
}

void gerarGrafoAleatorio(Grafo *g, int V, int probabilidade_aresta) {
    inicializarGrafo(g, V);
    srand((unsigned int)time(NULL));
    for (int i = 0; i < V; i++) {
        for (int j = i + 1; j < V; j++) {
            if ((rand() % 100) < probabilidade_aresta) {
                adicionarAresta(g, i, j);
            }
        }
    }
    printf("\n[INFO] Grafo aleatorio gerado: %d vertices, ~%d%% de densidade de arestas.\n", V, probabilidade_aresta);
}

void imprimirMatriz(Grafo *g) {
    if (g->V > 25) {
        printf("\n[AVISO] Grafo muito grande (%d vertices) para exibir no terminal.\n", g->V);
        return;
    }
    printf("\nMatriz de Adjacencia (%d vertices):\n    ", g->V);
    for (int i = 0; i < g->V; i++) printf("%2d ", i);
    printf("\n");
    for (int i = 0; i < g->V; i++) {
        printf("%2d: ", i);
        for (int j = 0; j < g->V; j++) {
            printf("%2d ", g->adj[i][j]);
        }
        printf("\n");
    }
}

bool ehIndependente(Grafo *g, int subconjunto[], int tamanho) {
    for (int i = 0; i < tamanho; i++) {
        for (int j = i + 1; j < tamanho; j++) {
            if (g->adj[subconjunto[i]][subconjunto[j]] == 1) {
                return false;
            }
        }
    }
    return true;
}