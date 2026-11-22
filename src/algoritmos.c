#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "algoritmos.h"

long long contador_instrucoes = 0;

// ============================================================================
// MÉTODO 1: FORÇA BRUTA - Complexidade: O(2^V * V^2)
// ============================================================================
int forcaBruta(Grafo *g, int melhor_conjunto[]) {
    if (g->V > 63) {
        printf("\n[LIMITE DE HARDWARE] 2^%d estoura o limite de 64 bits da CPU.\n", g->V);
        return -1;
    }

    int max_tam = 0;
    int atual[MAX_V];
    unsigned long long total_combinacoes = 1ULL << g->V;

    for (unsigned long long mask = 0; mask < total_combinacoes; mask++) {
        int tam_atual = 0;
        for (int i = 0; i < g->V; i++) {
            contador_instrucoes++;
            if ((mask & (1ULL << i)) != 0) {
                atual[tam_atual++] = i;
            }
        }

        bool valido = true;
        for (int i = 0; i < tam_atual && valido; i++) {
            for (int j = i + 1; j < tam_atual; j++) {
                contador_instrucoes++;
                if (g->adj[atual[i]][atual[j]] == 1) {
                    valido = false;
                    break;
                }
            }
        }

        if (valido && tam_atual > max_tam) {
            max_tam = tam_atual;
            for (int i = 0; i < tam_atual; i++) melhor_conjunto[i] = atual[i];
        }
    }
    return max_tam;
}

// ============================================================================
// MÉTODO 2: BACKTRACKING COM PODA - Complexidade: O(2^V) no pior caso
// ============================================================================
void backtrackRec(Grafo *g, int v_atual, int atual[], int tam_atual, int *max_tam, int melhor_conjunto[]) {
    contador_instrucoes++;

    if (tam_atual + (g->V - v_atual) <= *max_tam) {
        return;
    }

    if (v_atual == g->V) {
        if (tam_atual > *max_tam) {
            *max_tam = tam_atual;
            for (int i = 0; i < tam_atual; i++) melhor_conjunto[i] = atual[i];
        }
        return;
    }

    bool pode_incluir = true;
    for (int i = 0; i < tam_atual; i++) {
        contador_instrucoes++;
        if (g->adj[v_atual][atual[i]] == 1) {
            pode_incluir = false;
            break;
        }
    }

    if (pode_incluir) {
        atual[tam_atual] = v_atual;
        backtrackRec(g, v_atual + 1, atual, tam_atual + 1, max_tam, melhor_conjunto);
    }

    backtrackRec(g, v_atual + 1, atual, tam_atual, max_tam, melhor_conjunto);
}

int backtracking(Grafo *g, int melhor_conjunto[]) {
    int max_tam = 0;
    int atual[MAX_V];
    backtrackRec(g, 0, atual, 0, &max_tam, melhor_conjunto);
    return max_tam;
}

// ============================================================================
// MÉTODO 3: DIVISÃO E CONQUISTA
// ============================================================================
void dfsComponente(Grafo *g, int u, bool visitado[], int comp[], int *tam_comp) {
    visitado[u] = true;
    comp[(*tam_comp)++] = u;
    for (int v = 0; v < g->V; v++) {
        contador_instrucoes++;
        if (g->adj[u][v] == 1 && !visitado[v]) {
            dfsComponente(g, v, visitado, comp, tam_comp);
        }
    }
}

int resolverSubgrafoDC(Grafo *g, int vertices[], int n, int solucao[]) {
    contador_instrucoes++;
    if (n == 0) return 0;
    if (n == 1) {
        solucao[0] = vertices[0];
        return 1;
    }

    int v_escolhido = vertices[0];

    int sub1[MAX_V], n1 = 0;
    for (int i = 1; i < n; i++) {
        contador_instrucoes++;
        sub1[n1++] = vertices[i];
    }
    int sol1[MAX_V];
    int res1 = resolverSubgrafoDC(g, sub1, n1, sol1);

    int sub2[MAX_V], n2 = 0;
    for (int i = 1; i < n; i++) {
        contador_instrucoes++;
        if (g->adj[v_escolhido][vertices[i]] == 0) {
            sub2[n2++] = vertices[i];
        }
    }
    int sol2[MAX_V];
    int res2 = 1 + resolverSubgrafoDC(g, sub2, n2, sol2);

    if (res2 > res1) {
        solucao[0] = v_escolhido;
        for (int i = 0; i < res2 - 1; i++) solucao[i + 1] = sol2[i];
        return res2;
    } else {
        for (int i = 0; i < res1; i++) solucao[i] = sol1[i];
        return res1;
    }
}

int divisaoEConquista(Grafo *g, int melhor_conjunto[]) {
    bool visitado[MAX_V] = {false};
    int total_solucao = 0;

    for (int i = 0; i < g->V; i++) {
        if (!visitado[i]) {
            int comp[MAX_V], tam_comp = 0;
            dfsComponente(g, i, visitado, comp, &tam_comp);

            int sol_comp[MAX_V];
            int tam_sol_comp = resolverSubgrafoDC(g, comp, tam_comp, sol_comp);
            for (int j = 0; j < tam_sol_comp; j++) {
                melhor_conjunto[total_solucao++] = sol_comp[j];
            }
        }
    }
    return total_solucao;
}

// ============================================================================
// MÉTODO 4: PROGRAMAÇÃO DINÂMICA - Complexidade: O(2^V * V)
// ============================================================================
int programacaoDinamica(Grafo *g, int melhor_conjunto[]) {
    if (g->V > MAX_DP_V) {
        printf("\n[ERRO] PD com Bitmask exige muita memoria (limite: %d vertices).\n", MAX_DP_V);
        return -1;
    }

    unsigned int total_estados = 1U << g->V;
    char *dp = (char *)calloc(total_estados, sizeof(char));
    if (!dp) {
        printf("\n[ERRO] Falha ao alocar memoria para tabela DP.\n");
        return -1;
    }

    unsigned int vizinhos_mask[MAX_DP_V] = {0};
    for (int i = 0; i < g->V; i++) {
        for (int j = 0; j < g->V; j++) {
            contador_instrucoes++;
            if (g->adj[i][j] == 1) {
                vizinhos_mask[i] |= (1U << j);
            }
        }
    }

    dp[0] = 1;
    int max_tam = 0;
    unsigned int melhor_mask = 0;

    for (unsigned int mask = 1; mask < total_estados; mask++) {
        contador_instrucoes++;
        int v = __builtin_ctz(mask);
        unsigned int sub_anterior = mask ^ (1U << v);

        if (dp[sub_anterior] && ((sub_anterior & vizinhos_mask[v]) == 0)) {
            dp[mask] = 1;
            int tam = __builtin_popcount(mask);
            if (tam > max_tam) {
                max_tam = tam;
                melhor_mask = mask;
            }
        }
    }

    int idx = 0;
    for (int i = 0; i < g->V; i++) {
        if ((melhor_mask & (1U << i)) != 0) {
            melhor_conjunto[idx++] = i;
        }
    }

    free(dp);
    return max_tam;
}

// ============================================================================
// MÉTODO 5: ALGORITMO GULOSO - Complexidade: O(V^2)
// ============================================================================
int guloso(Grafo *g, int melhor_conjunto[]) {
    bool removido[MAX_V] = {false};
    int tam_solucao = 0;
    int restantes = g->V;

    while (restantes > 0) {
        int melhor_v = -1;
        int menor_grau = MAX_V + 1;

        for (int i = 0; i < g->V; i++) {
            if (!removido[i]) {
                int grau = 0;
                for (int j = 0; j < g->V; j++) {
                    contador_instrucoes++;
                    if (!removido[j] && g->adj[i][j] == 1) {
                        grau++;
                    }
                }
                if (grau < menor_grau) {
                    menor_grau = grau;
                    melhor_v = i;
                }
            }
        }

        if (melhor_v == -1) break;

        melhor_conjunto[tam_solucao++] = melhor_v;
        removido[melhor_v] = true;
        restantes--;

        for (int j = 0; j < g->V; j++) {
            contador_instrucoes++;
            if (!removido[j] && g->adj[melhor_v][j] == 1) {
                removido[j] = true;
                restantes--;
            }
        }
    }
    return tam_solucao;
}

// ============================================================================
// MÉTODO 6: HEURÍSTICA (Busca Local 1-por-2)
// ============================================================================
int heuristicaBuscaLocal(Grafo *g, int melhor_conjunto[]) {
    int tam = guloso(g, melhor_conjunto);
    bool na_solucao[MAX_V] = {false};
    for (int i = 0; i < tam; i++) na_solucao[melhor_conjunto[i]] = true;

    bool melhorou = true;
    while (melhorou) {
        melhorou = false;

        for (int i = 0; i < tam && !melhorou; i++) {
            int u = melhor_conjunto[i];
            na_solucao[u] = false;

            int candidatos[MAX_V], num_cand = 0;
            for (int v = 0; v < g->V; v++) {
                if (!na_solucao[v] && v != u) {
                    bool livre = true;
                    for (int j = 0; j < tam; j++) {
                        if (j == i) continue;
                        contador_instrucoes++;
                        if (g->adj[v][melhor_conjunto[j]] == 1) {
                            livre = false;
                            break;
                        }
                    }
                    if (livre) candidatos[num_cand++] = v;
                }
            }

            for (int a = 0; a < num_cand && !melhorou; a++) {
                for (int b = a + 1; b < num_cand && !melhorou; b++) {
                    contador_instrucoes++;
                    if (g->adj[candidatos[a]][candidatos[b]] == 0) {
                        melhor_conjunto[i] = candidatos[a];
                        melhor_conjunto[tam++] = candidatos[b];
                        na_solucao[candidatos[a]] = true;
                        na_solucao[candidatos[b]] = true;
                        melhorou = true;
                    }
                }
            }

            if (!melhorou) {
                na_solucao[u] = true;
            }
        }
    }
    return tam;
}