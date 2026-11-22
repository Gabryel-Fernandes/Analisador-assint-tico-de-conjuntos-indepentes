#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define MAX_V 100       
#define MAX_DP_V 22     

typedef struct {
    int V;
    int adj[MAX_V][MAX_V];
} Grafo;

long long contador_instrucoes = 0;

// ============================================================================
// 1. FUNÇÕES AUXILIARES E GERADORES DE GRAFO
// ============================================================================

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

// Melhor Grafo (Ciclo de 4 vértices) -> Aqui a gente vai mostar a otima resposta: 2 ({0,2} ou {1,3})
void carregarGrafoDidatico(Grafo *g) {
    inicializarGrafo(g, 4);
    adicionarAresta(g, 0, 1);
    adicionarAresta(g, 1, 2);
    adicionarAresta(g, 2, 3);
    adicionarAresta(g, 3, 0);
    printf("\n[INFO] Grafo didatico carregado (4 vertices em ciclo: 0-1-2-3-0).\n");
}

// Grafo quebrado (2 componentes de 4 vértices) -> Aqui a gente vai mostrar divisão e conquista
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
            contador_instrucoes++;
            if (g->adj[subconjunto[i]][subconjunto[j]] == 1) {
                return false;
            }
        }
    }
    return true;
}


// MÉTODO 1: FORÇA BRUTA - Complexidade: O(2^V * V^2)

int forcaBruta(Grafo *g, int melhor_conjunto[]) {
    int max_tam = 0;
    int atual[MAX_V];
    long long total_combinacoes = 1LL << g->V;

    for (long long mask = 0; mask < total_combinacoes; mask++) {
        int tam_atual = 0;
        for (int i = 0; i < g->V; i++) {
            contador_instrucoes++;
            if ((mask & (1LL << i)) != 0) {
                atual[tam_atual++] = i;
            }
        }
        if (ehIndependente(g, atual, tam_atual)) {
            if (tam_atual > max_tam) {
                max_tam = tam_atual;
                for (int i = 0; i < tam_atual; i++) melhor_conjunto[i] = atual[i];
            }
        }
    }
    return max_tam;
}


// MÉTODO 2: BACKTRACKING COM PODA - Complexidade: O(2^V) no pior caso

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


// MÉTODO 3: DIVISÃO E CONQUISTA

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

// METÓDO 4: PROGRAMAÇÃO DINÂMICA - Complexidade: O(2^V * V)

int programacaoDinamica(Grafo *g, int melhor_conjunto[]) {
    if (g->V > MAX_DP_V) {
        printf("\n[ERRO] PD com Bitmask exige muita memoria (limite: %d vertices).\n", MAX_DP_V);
        return -1;
    }

    int total_estados = 1 << g->V;
    char *dp = (char *)calloc(total_estados, sizeof(char));
    if (!dp) {
        printf("\n[ERRO] Falha ao alocar memoria para tabela DP.\n");
        return -1;
    }

    int vizinhos_mask[MAX_DP_V] = {0};
    for (int i = 0; i < g->V; i++) {
        for (int j = 0; j < g->V; j++) {
            contador_instrucoes++;
            if (g->adj[i][j] == 1) {
                vizinhos_mask[i] |= (1 << j);
            }
        }
    }

    dp[0] = 1;
    int max_tam = 0;
    int melhor_mask = 0;

    for (int mask = 1; mask < total_estados; mask++) {
        contador_instrucoes++;
        
        int v = __builtin_ctz(mask);
        int sub_anterior = mask ^ (1 << v);


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
        if ((melhor_mask & (1 << i)) != 0) {
            melhor_conjunto[idx++] = i;
        }
    }

    free(dp);
    return max_tam;
}


// MÉTODO 5: ALGORITMO GULOSO - Complexidade: O(V^2)

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

// MÉTODO 6: HEURÍSTICA

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


//  MENU INTERATIVO: EXIBIÇÃO DE RELATÓRIOS

void exibirRelatorio(const char *nome_algoritmo, int tamanho_encontrado, int k, int conjunto[], double tempo_ms) {
    if (tamanho_encontrado < 0) return;
    printf("\n----------------------------------------------------------------\n");
    printf(" ESTRATEGIA: %s\n", nome_algoritmo);
    printf("------------------------------------------------------------------\n");
    printf(" -> Meta (k): %d | Tamanho Encontrado: %d\n", k, tamanho_encontrado);
    printf(" -> Resposta Decisao (Existe >= k?): %s\n", (tamanho_encontrado >= k) ? "SIM" : "NAO");
    printf(" -> Subconjunto: { ");
    for (int i = 0; i < tamanho_encontrado; i++) {
        printf("%d ", conjunto[i]);
    }
    printf("}\n");
    printf(" -> Instrucoes/Comparacoes: %lld\n", contador_instrucoes);
    printf(" -> Tempo de execucao: %.4f ms\n", tempo_ms);
    printf("------------------------------------------------------------------\n");
}

void rodarBateriaComparativa(Grafo *g, int k) {
    int sol[MAX_V];
    clock_t ini, fim;
    double ms;
    int res;

    printf("\n--------------------- BATERIA COMPARATIVA (V=%d, k=%d) -------------------------=\n", g->V, k);
    printf("%-25s | %-8s | %-10s | %-18s | %-12s\n", "Estrategia", "Tam.", "Resp(>=k)", "Instrucoes", "Tempo (ms)");
    printf("--------------------------------------------------------------------------\n");

    // 1. Força Bruta (apenas se V <= 24 para não travar a apresentação)
    if (g->V <= 24) {
        contador_instrucoes = 0; ini = clock();
        res = forcaBruta(g, sol);
        fim = clock(); ms = ((double)(fim - ini) / CLOCKS_PER_SEC) * 1000.0;
        printf("%-25s | %-8d | %-10s | %-18lld | %-12.4f\n", "1. Forca Bruta", res, (res >= k ? "SIM" : "NAO"), contador_instrucoes, ms);
    } else {
        printf("%-25s | %-8s | %-10s | %-18s | %-12s\n", "1. Forca Bruta", "N/A", "PULADO", "V > 24 (Inviavel)", "-");
    }

    // 2. Backtracking
    if (g->V <= 45) {
        contador_instrucoes = 0; ini = clock();
        res = backtracking(g, sol);
        fim = clock(); ms = ((double)(fim - ini) / CLOCKS_PER_SEC) * 1000.0;
        printf("%-25s | %-8d | %-10s | %-18lld | %-12.4f\n", "2. Backtracking", res, (res >= k ? "SIM" : "NAO"), contador_instrucoes, ms);
    } else {
        printf("%-25s | %-8s | %-10s | %-18s | %-12s\n", "2. Backtracking", "N/A", "PULADO", "V > 45 (Inviavel)", "-");
    }

    // 3. Divisão e Conquista
    if (g->V <= 40) {
        contador_instrucoes = 0; ini = clock();
        res = divisaoEConquista(g, sol);
        fim = clock(); ms = ((double)(fim - ini) / CLOCKS_PER_SEC) * 1000.0;
        printf("%-25s | %-8d | %-10s | %-18lld | %-12.4f\n", "3. Divisao e Conquista", res, (res >= k ? "SIM" : "NAO"), contador_instrucoes, ms);
    } else {
        printf("%-25s | %-8s | %-10s | %-18s | %-12s\n", "3. Divisao e Conquista", "N/A", "PULADO", "V > 40 (Inviavel)", "-");
    }

    // 4. Programação Dinâmica
    if (g->V <= MAX_DP_V) {
        contador_instrucoes = 0; ini = clock();
        res = programacaoDinamica(g, sol);
        fim = clock(); ms = ((double)(fim - ini) / CLOCKS_PER_SEC) * 1000.0;
        printf("%-25s | %-8d | %-10s | %-18lld | %-12.4f\n", "4. Prog. Dinamica", res, (res >= k ? "SIM" : "NAO"), contador_instrucoes, ms);
    } else {
        printf("%-25s | %-8s | %-10s | %-18s | %-12s\n", "4. Prog. Dinamica", "N/A", "PULADO", "V > 22 (Memoria)", "-");
    }

    // 5. Guloso
    contador_instrucoes = 0; ini = clock();
    res = guloso(g, sol);
    fim = clock(); ms = ((double)(fim - ini) / CLOCKS_PER_SEC) * 1000.0;
    printf("%-25s | %-8d | %-10s | %-18lld | %-12.4f\n", "5. Algoritmo Guloso", res, (res >= k ? "SIM" : "NAO"), contador_instrucoes, ms);

    // 6. Heurística (Busca Local)
    contador_instrucoes = 0; ini = clock();
    res = heuristicaBuscaLocal(g, sol);
    fim = clock(); ms = ((double)(fim - ini) / CLOCKS_PER_SEC) * 1000.0;
    printf("%-25s | %-8d | %-10s | %-18lld | %-12.4f\n", "6. Heuristica (1-2 Swap)", res, (res >= k ? "SIM" : "NAO"), contador_instrucoes, ms);
    printf("------------------------------------------------------------------------------\n");
}

int main() {
    Grafo g;
    int k = 2;
    int opcao;
    int solucao[MAX_V];

    carregarGrafoDidatico(&g);

    do {
        printf("\n-------------------- CARTEIRA DE TESTES - CONJUNTO INDEPENDENTE ----------------------------\n");
        printf(" Grafo Atual: %d vertices | Meta k = %d\n", g.V, k);
        printf("----------------------------------------------------------------------------\n");
        printf(" [CONFIGURACAO DO GRAFO]\n");
        printf("   1. Carregar Grafo Didatico (4 vertices em ciclo)\n");
        printf("   2. Carregar Grafo Desconexo (8 vertices - ideal p/ Divisao e Conquista)\n");
        printf("   3. Gerar Grafo Aleatorio (definir V e densidade)\n");
        printf("   4. Alterar valor de k\n");
        printf("   5. Mostrar Matriz de Adjacencia\n");
        printf("----------------------------------------------------------------------------\n");
        printf(" [EXECUTAR ESTRATEGIAS INDIVIDUALMENTE]\n");
        printf("   6. Forca Bruta\n");
        printf("   7. Backtracking com Poda\n");
        printf("   8. Divisao e Conquista (Componentes + Reducao)\n");
        printf("   9. Programacao Dinamica (Bitmask DP - max 22 vertices)\n");
        printf("  10. Algoritmo Guloso (Menor Grau)\n");
        printf("  11. Heuristica (Busca Local / Hill Climbing 1-por-2)\n");
        printf("----------------------------------------------------------------------------\n");
        printf(" [RELATORIO GERAL]\n");
        printf("  12. Rodar Bateria Comparativa (Tabela com todas as estrategias)\n");
        printf("   0. Sair\n");
        printf("----------------------------------------------------------------------------- \n");
        printf("Escolha uma opcao: ");
        if (scanf("%d", &opcao) != 1) break;

        clock_t inicio, fim;
        double tempo_ms;
        int tam_res;
        contador_instrucoes = 0;

        switch (opcao) {
            case 1:
                carregarGrafoDidatico(&g);
                k = 2;
                break;
            case 2:
                carregarGrafoDesconexo(&g);
                k = 4;
                break;
            case 3: {
                int v, dens;
                printf("Numero de vertices (max %d): ", MAX_V);
                scanf("%d", &v);
                if (v < 1) v = 1;
                if (v > MAX_V) v = MAX_V;
                printf("Densidade de arestas (0 a 100%%): ");
                scanf("%d", &dens);
                gerarGrafoAleatorio(&g, v, dens);
                break;
            }
            case 4:
                printf("Novo valor de k: ");
                scanf("%d", &k);
                break;
            case 5:
                imprimirMatriz(&g);
                break;
            case 6:
                if (g.V > 24) {
                    printf("[AVISO] V=%d pode demorar muito. Continuar? (1-Sim / 0-Nao): ", g.V);
                    int c; scanf("%d", &c); if (!c) break;
                }
                inicio = clock();
                tam_res = forcaBruta(&g, solucao);
                fim = clock();
                tempo_ms = ((double)(fim - inicio) / CLOCKS_PER_SEC) * 1000.0;
                exibirRelatorio("1. FORCA BRUTA", tam_res, k, solucao, tempo_ms);
                break;
            case 7:
                inicio = clock();
                tam_res = backtracking(&g, solucao);
                fim = clock();
                tempo_ms = ((double)(fim - inicio) / CLOCKS_PER_SEC) * 1000.0;
                exibirRelatorio("2. BACKTRACKING COM PODA", tam_res, k, solucao, tempo_ms);
                break;
            case 8:
                inicio = clock();
                tam_res = divisaoEConquista(&g, solucao);
                fim = clock();
                tempo_ms = ((double)(fim - inicio) / CLOCKS_PER_SEC) * 1000.0;
                exibirRelatorio("3. DIVISAO E CONQUISTA", tam_res, k, solucao, tempo_ms);
                break;
            case 9:
                inicio = clock();
                tam_res = programacaoDinamica(&g, solucao);
                fim = clock();
                tempo_ms = ((double)(fim - inicio) / CLOCKS_PER_SEC) * 1000.0;
                exibirRelatorio("4. PROGRAMACAO DINAMICA (BITMASK)", tam_res, k, solucao, tempo_ms);
                break;
            case 10:
                inicio = clock();
                tam_res = guloso(&g, solucao);
                fim = clock();
                tempo_ms = ((double)(fim - inicio) / CLOCKS_PER_SEC) * 1000.0;
                exibirRelatorio("5. ALGORITMO GULOSO", tam_res, k, solucao, tempo_ms);
                break;
            case 11:
                inicio = clock();
                tam_res = heuristicaBuscaLocal(&g, solucao);
                fim = clock();
                tempo_ms = ((double)(fim - inicio) / CLOCKS_PER_SEC) * 1000.0;
                exibirRelatorio("6. HEURISTICA (BUSCA LOCAL)", tam_res, k, solucao, tempo_ms);
                break;
            case 12:
                rodarBateriaComparativa(&g, k);
                break;
            case 0:
                printf("Encerrando carteira de testes...\n");
                break;
            default:
                printf("Opcao invalida!\n");
        }
    } while (opcao != 0);

    return 0;
}