#include <stdio.h>
#include <time.h>
#include "grafo.h"
#include "algoritmos.h"

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

    // 1. Força Bruta
    if (g->V <= 27) {
        contador_instrucoes = 0; ini = clock();
        res = forcaBruta(g, sol);
        fim = clock(); ms = ((double)(fim - ini) / CLOCKS_PER_SEC) * 1000.0;
        printf("%-25s | %-8d | %-10s | %-18lld | %-12.4f\n", "1. Forca Bruta", res, (res >= k ? "SIM" : "NAO"), contador_instrucoes, ms);
    } else {
        printf("%-25s | %-8s | %-10s | %-18s | %-12s\n", "1. Forca Bruta", "N/A", "PULADO", "V > 27 (Inviavel)", "-");
    }

    // 2. Backtracking
    if (g->V <= 60) {
        contador_instrucoes = 0; ini = clock();
        res = backtracking(g, sol);
        fim = clock(); ms = ((double)(fim - ini) / CLOCKS_PER_SEC) * 1000.0;
        printf("%-25s | %-8d | %-10s | %-18lld | %-12.4f\n", "2. Backtracking", res, (res >= k ? "SIM" : "NAO"), contador_instrucoes, ms);
    } else {
        printf("%-25s | %-8s | %-10s | %-18s | %-12s\n", "2. Backtracking", "N/A", "PULADO", "V > 60 (Inviavel)", "-");
    }

    // 3. Divisão e Conquista
    if (g->V <= 55) {
        contador_instrucoes = 0; ini = clock();
        res = divisaoEConquista(g, sol);
        fim = clock(); ms = ((double)(fim - ini) / CLOCKS_PER_SEC) * 1000.0;
        printf("%-25s | %-8d | %-10s | %-18lld | %-12.4f\n", "3. Divisao e Conquista", res, (res >= k ? "SIM" : "NAO"), contador_instrucoes, ms);
    } else {
        printf("%-25s | %-8s | %-10s | %-18s | %-12s\n", "3. Divisao e Conquista", "N/A", "PULADO", "V > 55 (Inviavel)", "-");
    }

    // 4. Programação Dinâmica
    if (g->V <= MAX_DP_V) {
        contador_instrucoes = 0; ini = clock();
        res = programacaoDinamica(g, sol);
        fim = clock(); ms = ((double)(fim - ini) / CLOCKS_PER_SEC) * 1000.0;
        printf("%-25s | %-8d | %-10s | %-18lld | %-12.4f\n", "4. Prog. Dinamica", res, (res >= k ? "SIM" : "NAO"), contador_instrucoes, ms);
    } else {
        printf("%-25s | %-8s | %-10s | %-18s | %-12s\n", "4. Prog. Dinamica", "N/A", "PULADO", "V > 28 (Memoria)", "-");
    }

    // 5. Guloso
    contador_instrucoes = 0; ini = clock();
    res = guloso(g, sol);
    fim = clock(); ms = ((double)(fim - ini) / CLOCKS_PER_SEC) * 1000.0;
    printf("%-25s | %-8d | %-10s | %-18lld | %-12.4f\n", "5. Algoritmo Guloso", res, (res >= k ? "SIM" : "NAO"), contador_instrucoes, ms);

    // 6. Heurística
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
        printf("   9. Programacao Dinamica (Bitmask DP - max %d vertices)\n", MAX_DP_V);
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
                if (g.V > 27) {
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