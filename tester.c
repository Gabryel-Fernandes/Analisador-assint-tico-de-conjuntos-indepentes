#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define MAX_V 100 // Limite máximo de vértices para a matriz

// Estrutura do Grafo
typedef struct {
    int V;
    int adj[MAX_V][MAX_V];
} Grafo;

// Variável global para contagem de instruções (requisito do trabalho)
long long contador_instrucoes = 0;

// --- FUNÇÕES AUXILIARES DO GRAFO ---

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
        g->adj[v][u] = 1; // Grafo não direcionado
    }
}

// Carrega o exemplo do "Quadrado" (4 vértices) para demonstração didática
void carregarGrafoDidatico(Grafo *g) {
    inicializarGrafo(g, 4);
    adicionarAresta(g, 0, 1); // A - B
    adicionarAresta(g, 1, 2); // B - C
    adicionarAresta(g, 2, 3); // C - D
    adicionarAresta(g, 3, 0); // D - A
    printf("\n[INFO] Grafo didatico carregado (4 vertices em ciclo: 0-1-2-3-0).\n");
    printf("[INFO] Maior conjunto independente esperado: tamanho 2 ({0, 2} ou {1, 3}).\n");
}

// Gera grafo aleatório para testes de estresse/tempo
void gerarGrafoAleatorio(Grafo *g, int V, int probabilidade_aresta) {
    inicializarGrafo(g, V);
    srand(time(NULL));
    for (int i = 0; i < V; i++) {
        for (int j = i + 1; j < V; j++) {
            if ((rand() % 100) < probabilidade_aresta) {
                adicionarAresta(g, i, j);
            }
        }
    }
    printf("\n[INFO] Grafo aleatorio gerado com %d vertices e %d%% de densidade.\n", V, probabilidade_aresta);
}

void imprimirMatriz(Grafo *g) {
    if (g->V > 20) {
        printf("\n[AVISO] Grafo muito grande (%d vertices) para imprimir matriz no terminal.\n", g->V);
        return;
    }
    printf("\nMatriz de Adjacencia (%d vertices):\n   ", g->V);
    for (int i = 0; i < g->V; i++) printf("%2d ", i);
    printf("\n");
    for (int i = 0; i < g->V; i++) {
        printf("%2d ", i);
        for (int j = 0; j < g->V; j++) {
            printf("%2d ", g->adj[i][j]);
        }
        printf("\n");
    }
}

// Verifica se um subconjunto de vértices é independente
bool ehIndependente(Grafo *g, int subconjunto[], int tamanho) {
    for (int i = 0; i < tamanho; i++) {
        for (int j = i + 1; j < tamanho; j++) {
            contador_instrucoes++; // Conta verificação de aresta
            if (g->adj[subconjunto[i]][subconjunto[j]] == 1) {
                return false; // Existe conflito (aresta)
            }
        }
    }
    return true;
}

// --- 1. ESTRATÉGIA: FORÇA BRUTA ---
// Testa todas as 2^V combinações possíveis
int forcaBruta(Grafo *g, int k, int melhor_conjunto[]) {
    int max_tam = 0;
    int atual[MAX_V];
    long long total_combinacoes = 1LL << g->V; // 2^V

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

// --- 2. ESTRATÉGIA: BACKTRACKING ---
void backtrackRec(Grafo *g, int v_atual, int atual[], int tam_atual, int *max_tam, int melhor_conjunto[]) {
    contador_instrucoes++;
    
    // Poda: mesmo pegando todos os vértices restantes, não supera o máximo já achado
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

    // Verifica se pode incluir v_atual (não tem aresta com ninguém já escolhido)
    bool pode_incluir = true;
    for (int i = 0; i < tam_atual; i++) {
        contador_instrucoes++;
        if (g->adj[v_atual][atual[i]] == 1) {
            pode_incluir = false;
            break;
        }
    }

    // Ramo 1: Inclui o vértice atual (se válido)
    if (pode_incluir) {
        atual[tam_atual] = v_atual;
        backtrackRec(g, v_atual + 1, atual, tam_atual + 1, max_tam, melhor_conjunto);
    }

    // Ramo 2: Não inclui o vértice atual
    backtrackRec(g, v_atual + 1, atual, tam_atual, max_tam, melhor_conjunto);
}

int backtracking(Grafo *g, int melhor_conjunto[]) {
    int max_tam = 0;
    int atual[MAX_V];
    backtrackRec(g, 0, atual, 0, &max_tam, melhor_conjunto);
    return max_tam;
}

// --- 5. ESTRATÉGIA: ALGORITMO GULOSO (Menor Grau) ---
int guloso(Grafo *g, int melhor_conjunto[]) {
    bool removido[MAX_V] = {false};
    int tam_solucao = 0;
    int restantes = g->V;

    while (restantes > 0) {
        int melhor_v = -1;
        int menor_grau = MAX_V + 1;

        // Encontra o vértice ativo com menor número de vizinhos ativos
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

        // Adiciona o escolhido na solução
        melhor_conjunto[tam_solucao++] = melhor_v;
        removido[melhor_v] = true;
        restantes--;

        // Elimina todos os vizinhos dele
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

// --- EXIBIÇÃO DOS RESULTADOS ---
void exibirRelatorio(const char *nome_algoritmo, int tamanho_encontrado, int k, int conjunto[], double tempo_ms) {
    printf("\n======================================================\n");
    printf(" ESTRATEGIA: %s\n", nome_algoritmo);
    printf("======================================================\n");
    printf(" -> Meta (k): %d | Maior Conjunto Encontrado: %d\n", k, tamanho_encontrado);
    printf(" -> Resposta do Problema (Existe >= k?): %s\n", (tamanho_encontrado >= k) ? "SIM" : "NAO");
    printf(" -> Vertices escolhidos: { ");
    for (int i = 0; i < tamanho_encontrado; i++) {
        printf("%d ", conjunto[i]);
    }
    printf("}\n");
    printf(" -> Instrucoes/Comparacoes contadas: %lld\n", contador_instrucoes);
    printf(" -> Tempo de execucao: %.4f ms\n", tempo_ms);
    printf("======================================================\n");
}

// --- MENU INTERATIVO (CARTEIRA DE TESTES) ---
int main() {
    Grafo g;
    int k = 2;
    int opcao;
    int solucao[MAX_V];

    carregarGrafoDidatico(&g);

    do {
        printf("\n================ CARTEIRA DE TESTES - APA ================\n");
        printf("Grafo atual: %d vertices | Meta k = %d\n", g.V, k);
        printf("----------------------------------------------------------\n");
        printf(" [CONFIGURACAO DO GRAFO]\n");
        printf("  1. Carregar Grafo Didatico (4 vertices)\n");
        printf("  2. Gerar Grafo Aleatorio (definir V e densidade)\n");
        printf("  3. Alterar valor de k\n");
        printf("  4. Mostrar Matriz de Adjacencia\n");
        printf("----------------------------------------------------------\n");
        printf(" [EXECUTAR ESTRATEGIAS]\n");
        printf("  5. Forca Bruta (O(2^V * V^2)) - Cuidado se V > 22!\n");
        printf("  6. Backtracking com Poda\n");
        printf("  7. Algoritmo Guloso (Menor Grau)\n");
        printf("  0. Sair\n");
        printf("==========================================================\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);

        clock_t inicio, fim;
        double tempo_ms;
        int tam_res;
        contador_instrucoes = 0; // Zera o contador antes de cada teste

        switch (opcao) {
            case 1:
                carregarGrafoDidatico(&g);
                k = 2;
                break;
            case 2: {
                int v, dens;
                printf("Digite o numero de vertices (max %d): ", MAX_V);
                scanf("%d", &v);
                if (v > MAX_V) v = MAX_V;
                printf("Digite a densidade de arestas (0 a 100%%): ");
                scanf("%d", &dens);
                gerarGrafoAleatorio(&g, v, dens);
                break;
            }
            case 3:
                printf("Digite o novo valor de k: ");
                scanf("%d", &k);
                break;
            case 4:
                imprimirMatriz(&g);
                break;
            case 5:
                if (g.V > 25) {
                    printf("\n[ALERTA] V=%d vai demorar muito na Forca Bruta! Deseja continuar? (1-Sim / 0-Nao): ", g.V);
                    int conf;
                    scanf("%d", &conf);
                    if (!conf) break;
                }
                inicio = clock();
                tam_res = forcaBruta(&g, k, solucao);
                fim = clock();
                tempo_ms = ((double)(fim - inicio) / CLOCKS_PER_SEC) * 1000.0;
                exibirRelatorio("FORCA BRUTA", tam_res, k, solucao, tempo_ms);
                break;
            case 6:
                inicio = clock();
                tam_res = backtracking(&g, solucao);
                fim = clock();
                tempo_ms = ((double)(fim - inicio) / CLOCKS_PER_SEC) * 1000.0;
                exibirRelatorio("BACKTRACKING", tam_res, k, solucao, tempo_ms);
                break;
            case 7:
                inicio = clock();
                tam_res = guloso(&g, solucao);
                fim = clock();
                tempo_ms = ((double)(fim - inicio) / CLOCKS_PER_SEC) * 1000.0;
                exibirRelatorio("ALGORITMO GULOSO", tam_res, k, solucao, tempo_ms);
                break;
            case 0:
                printf("Encerrando...\n");
                break;
            default:
                printf("Opcao invalida!\n");
        }
    } while (opcao != 0);

    return 0;
}