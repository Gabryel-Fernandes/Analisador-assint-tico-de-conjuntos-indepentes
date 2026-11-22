# Analisador Assintótico e Empírico — Conjunto Independente (Independent Set)

Carteira de testes interativa desenvolvida em linguagem **C** para análise de complexidade, contagem de instruções e comparação empírica de seis paradigmas algorítmicos aplicados ao problema clássico do **Conjunto Independente Máximo** (*Maximum Independent Set* — MIS), um problema **NP-Completo** da teoria da computação.

---

## 📋 Sobre o Problema

Dado um grafo não-direcionado $G = (V, E)$, um **Conjunto Independente** é um subconjunto de vértices $S \subseteq V$ tal que nenhum par de vértices em $S$ é adjacente (ou seja, não existe aresta ligando dois vértices pertencentes a $S$).

* **Versão de Decisão (NP-Completo):** Dado um grafo $G$ e um inteiro $k$, existe um conjunto independente de tamanho pelo menos $k$?
* **Versão de Otimização (NP-Difícil):** Encontrar o maior conjunto independente possível em $G$.

---

## 🏗️ Arquitetura do Projeto

O projeto foi modularizado seguindo boas práticas de engenharia de software em C, separando definições de interface (`.h`) da lógica de implementação (`.c`):

```text
├── include/
│   ├── grafo.h          # Definição da estrutura do Grafo, constantes (MAX_V, MAX_DP_V) e assinaturas
│   └── algoritmos.h     # Assinaturas das 6 estratégias e declaração externa do contador de instruções
├── src/
│   ├── grafo.c          # Inicialização da matriz de adjacência, geradores de grafos e exibição
│   ├── algoritmos.c     # Implementação completa dos 6 paradigmas algorítmicos
│   └── main.c           # Menu interativo, medição de tempo (clock) e bateria comparativa
├── .gitignore           # Ignora binários compilados (*.exe, *.o, *.out)
└── README.md            # Documentação do projeto
```

---

## 🧠 Estratégias Implementadas e Complexidade

| Nº | Estratégia | Tipo de solução | Complexidade de tempo | Complexidade de espaço | Limite prático (V) |
|---:|---|---|---|---|---|
| 1 | **Força Bruta** | Exata | $O(2^V \cdot V^2)$ | $O(V)$ | $\le 27$ vértices |
| 2 | **Backtracking com Poda** | Exata | $O(2^V)$ *(pior caso)* | $O(V)$ | $\le 60$ vértices |
| 3 | **Divisão e Conquista** | Exata | $O(2^V)$ *(pior caso)* | $O(V^2)$ | $\le 55$ vértices |
| 4 | **Programação Dinâmica (Bitmask)** | Exata | $O(2^V \cdot V)$ | $O(2^V)$ | $\le 28$ vértices *(RAM)* |
| 5 | **Algoritmo Guloso (Menor Grau)** | Aproximada | $O(V^2)$ | $O(V)$ | Até $500+$ vértices |
| 6 | **Heurística (Busca Local 1-2 Swap)** | Meta-heurística | $O(I \cdot V^3)$ | $O(V)$ | Até $500+$ vértices |

### Detalhamento Técnico das Abordagens
1. **Força Bruta:** Gera exaustivamente todos os $2^V$ subconjuntos possíveis utilizando manipulação de bits (`1ULL << i`) e verifica a independência de cada par de vértices.
2. **Backtracking com Poda:** Constrói a árvore de decisão recursivamente e aplica poda por limitante superior (*branch-and-bound*): se `tam_atual + vértices_restantes <= max_tam`, o ramo é descartado imediatamente.
3. **Divisão e Conquista:** Identifica componentes conexos via Busca em Profundidade (DFS) para resolver subproblemas independentes e aplica redução recursiva escolhendo ou descartando vértices e seus vizinhos.
4. **Programação Dinâmica (Bitmask DP):** Utiliza programação dinâmica sobre subconjuntos representados por máscaras de bits (`unsigned int`), reaproveitando estados válidos anteriores em tempo $O(1)$ por transição com operações bit a bit (`__builtin_ctz`, `__builtin_popcount`).
5. **Algoritmo Guloso:** Seleciona iterativamente o vértice ativo de **menor grau** no grafo residual, adiciona-o à solução e remove ele e seus vizinhos, garantindo resposta em tempo polinomial.
6. **Heurística (Busca Local / Hill Climbing 1-por-2):** Parte da solução gerada pelo Guloso e tenta trocar **1 vértice** da solução atual por **2 vértices** não-adjacentes externos até atingir um ótimo local.

---

## ⚙️ Como Compilar e Executar

### Pré-requisitos
* Compilador **GCC** (MinGW no Windows, ou GCC nativo no Linux/macOS).

### Compilação (com otimização `-O3`)
Abra o terminal na pasta raiz do projeto e execute:

```bash
gcc -O3 -Iinclude src/main.c src/grafo.c src/algoritmos.c -o tester.exe
```

### Execução
* **Windows (Git Bash / CMD / PowerShell):**
  ```bash
  ./tester.exe
  ```
* **Linux / macOS:**
  ```bash
  ./tester
  ```

---

## 🖥️ Funcionalidades da Carteira de Testes

Ao iniciar o programa, o usuário tem acesso a um menu dividido em três blocos:

1. **Configuração do Grafo:**
   * **Opção 1:** Carrega um grafo didático cíclico ($C_4$, resposta ótima = 2).
   * **Opção 2:** Carrega um grafo desconexo com 2 componentes independentes ($8$ vértices, resposta ótima = 4), ideal para evidenciar o ganho da Divisão e Conquista.
   * **Opção 3:** Gera grafos aleatórios configuráveis (até `MAX_V = 500` vértices e densidade de arestas de `0%` a `100%`).
   * **Opção 4 e 5:** Permite alterar a meta $k$ (problema de decisão) e visualizar a matriz de adjacência (para $V \le 25$).
2. **Execução Individual (Opções 6 a 11):**
   * Executa cada paradigma isoladamente, exibindo o subconjunto encontrado, a resposta de decisão (`SIM`/`NAO`), o número exato de instruções/comparações executadas e o tempo em milissegundos.
3. **Bateria Comparativa (Opção 12):**
   * Roda todas as estratégias sobre o mesmo grafo e gera uma tabela consolidada no terminal para coleta de métricas experimentais, pulando automaticamente algoritmos inviáveis quando $V$ ultrapassa o limite seguro de tempo ou memória.

---

## 📊 Sugestão de Roteiro de Testes (Benchmark)

Para reproduzir a curva de crescimento assintótico entre algoritmos exatos e polinomiais:

* **Validação de Corretude ($V = 10$, densidade $30\%$):** Todas as 6 estratégias executam em $< 1\text{ ms}$; os 4 algoritmos exatos devem convergir para exatamente o mesmo tamanho máximo.
* **Ponto de Ruptura Exponencial ($V = 27$, densidade $10\%$):** Limite máximo da Bateria Comparativa onde todos os 6 métodos ainda rodam juntos; evidencia a explosão de instruções da Força Bruta frente à Programação Dinâmica e ao Backtracking.
* **Escala Polinomial ($V = 100$ a $500$, densidade $20\%$):** Demonstra a inviabilidade dos métodos exatos e a eficiência imediata ($< 5\text{ ms}$) do Algoritmo Guloso e da Heurística de Busca Local.