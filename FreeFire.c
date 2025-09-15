#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>


// ===================== UTIL COMUM =====================

static void limparTela(void) {
    for (int i = 0; i < 12; i++) puts("");
}

static void lerLinha(char *dest, size_t tam) {
    if (fgets(dest, (int)tam, stdin)) {
        size_t n = strlen(dest);
        if (n && dest[n-1] == '\n') dest[n-1] = '\0';
    } else if (tam) {
        dest[0] = '\0';
    }
}

static void pausa(void) {
    printf("\nPressione ENTER para continuar...");
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

static double agora_ms(void) {
    return (double)clock() * 1000.0 / CLOCKS_PER_SEC;
}

// ===================== CONTADORES GLOBAIS (mochila, parte já existente) =====================
static long long comparacoesSequencial = 0;
static long long comparacoesBinaria    = 0;

// ===================== MOCHILA (VETOR + LISTA) — PARTE EXISTENTE =====================

typedef struct {
    char nome[30];
    char tipo[20];     // ex.: "arma", "municao", "cura"
    int  quantidade;
} Item;

typedef struct No {
    Item dados;
    struct No* proximo;
} No;

#define CAPACIDADE 10
static Item mochilaVetor[CAPACIDADE];
static int  numItensVetor = 0;
static bool vetorOrdenadoPorNome = false;

static No* inicioLista = NULL;

// --- VETOR ---
static void inserirItemVetor(void) {
    if (numItensVetor >= CAPACIDADE) {
        printf("\n[!] Mochila (vetor) cheia. Remova algo antes de adicionar.\n");
        return;
    }

    Item novo;
    printf("\n== Cadastro de Item (Vetor) ==\n");
    printf("Nome (max 29): ");
    lerLinha(novo.nome, sizeof(novo.nome));
    printf("Tipo (ex: arma, municao, cura) (max 19): ");
    lerLinha(novo.tipo, sizeof(novo.tipo));

    printf("Quantidade: ");
    while (scanf("%d", &novo.quantidade) != 1 || novo.quantidade < 0) {
        printf("Valor inválido. Informe inteiro >= 0: ");
        int ch; while ((ch = getchar()) != '\n' && ch != EOF) {}
    }
    int ch; while ((ch = getchar()) != '\n' && ch != EOF) {}

    for (int i = 0; i < numItensVetor; i++) {
        if (strcmp(mochilaVetor[i].nome, novo.nome) == 0) {
            mochilaVetor[i].quantidade += novo.quantidade;
            printf("\n[OK] Item já existia. Quantidade somada!\n");
            vetorOrdenadoPorNome = false;
            return;
        }
    }

    mochilaVetor[numItensVetor++] = novo;
    vetorOrdenadoPorNome = false;
    printf("\n[OK] Item adicionado ao vetor!\n");
}

static void removerItemVetor(void) {
    if (numItensVetor == 0) {
        printf("\n[!] Mochila (vetor) vazia. Nada a remover.\n");
        return;
    }

    char nome[30];
    printf("\n== Remover Item (Vetor) ==\n");
    printf("Informe o nome exato do item: ");
    lerLinha(nome, sizeof(nome));

    int idx = -1;
    for (int i = 0; i < numItensVetor; i++) {
        if (strcmp(mochilaVetor[i].nome, nome) == 0) { idx = i; break; }
    }
    if (idx == -1) {
        printf("\n[!] Item '%s' não encontrado no vetor.\n", nome);
        return;
    }
    for (int i = idx; i < numItensVetor - 1; i++) mochilaVetor[i] = mochilaVetor[i+1];
    numItensVetor--;
    vetorOrdenadoPorNome = false;
    printf("\n[OK] Item removido do vetor.\n");
}

static void listarItensVetor(void) {
    printf("\n===== Itens na mochila (VETOR) (%d/%d) =====\n", numItensVetor, CAPACIDADE);
    if (numItensVetor == 0) {
        printf("Mochila vazia.\n");
        return;
    }
    printf("%-3s | %-30s | %-12s | %-10s\n", "#", "Nome", "Tipo", "Quantidade");
    printf("----+--------------------------------+--------------+------------\n");
    for (int i = 0; i < numItensVetor; i++) {
        printf("%-3d | %-30s | %-12s | %-10d\n",
               i + 1, mochilaVetor[i].nome, mochilaVetor[i].tipo, mochilaVetor[i].quantidade);
    }
    printf("\nStatus de ordenação por nome: %s\n",
           vetorOrdenadoPorNome ? "ORDENADO (nome A→Z)" : "NÃO ORDENADO");
}

static int buscarSequencialVetor(const char* nome) {
    comparacoesSequencial = 0;
    for (int i = 0; i < numItensVetor; i++) {
        comparacoesSequencial++;
        if (strcmp(mochilaVetor[i].nome, nome) == 0) return i;
    }
    return -1;
}

static void ordenarVetorPorNome(void) {
    if (numItensVetor <= 1) { vetorOrdenadoPorNome = true; return; }
    for (int i = 0; i < numItensVetor - 1; i++) {
        int minIdx = i;
        for (int j = i + 1; j < numItensVetor; j++) {
            if (strcmp(mochilaVetor[j].nome, mochilaVetor[minIdx].nome) < 0) {
                minIdx = j;
            }
        }
        if (minIdx != i) {
            Item tmp = mochilaVetor[i];
            mochilaVetor[i] = mochilaVetor[minIdx];
            mochilaVetor[minIdx] = tmp;
        }
    }
    vetorOrdenadoPorNome = true;
    printf("\n[OK] Vetor ordenado por NOME (Selection Sort – mochila).\n");
}

static int buscarBinariaVetor(const char* nome) {
    if (!vetorOrdenadoPorNome) {
        printf("\n[!] A busca binária exige o vetor ORDENADO por nome.\n");
        return -1;
    }
    comparacoesBinaria = 0;
    int ini = 0, fim = numItensVetor - 1;
    while (ini <= fim) {
        int meio = (ini + fim) / 2;
        comparacoesBinaria++;
        int cmp = strcmp(mochilaVetor[meio].nome, nome);
        if (cmp == 0) return meio;
        else if (cmp < 0) ini = meio + 1;
        else fim = meio - 1;
    }
    return -1;
}

// --- LISTA ---
static void inserirItemLista(void) {
    Item novo;
    printf("\n== Cadastro de Item (Lista) ==\n");
    printf("Nome (max 29): ");
    lerLinha(novo.nome, sizeof(novo.nome));
    printf("Tipo (ex: arma, municao, cura) (max 19): ");
    lerLinha(novo.tipo, sizeof(novo.tipo));

    printf("Quantidade: ");
    while (scanf("%d", &novo.quantidade) != 1 || novo.quantidade < 0) {
        printf("Valor inválido. Informe inteiro >= 0: ");
        int ch; while ((ch = getchar()) != '\n' && ch != EOF) {}
    }
    int ch; while ((ch = getchar()) != '\n' && ch != EOF) {}

    for (No* p = inicioLista; p != NULL; p = p->proximo) {
        if (strcmp(p->dados.nome, novo.nome) == 0) {
            p->dados.quantidade += novo.quantidade;
            printf("\n[OK] Item já existia na lista. Quantidade somada!\n");
            return;
        }
    }

    No* n = (No*)malloc(sizeof(No));
    if (!n) { printf("\n[ERRO] Falha de alocação.\n"); return; }
    n->dados = novo;
    n->proximo = inicioLista;
    inicioLista = n;
    printf("\n[OK] Item inserido na lista (início).\n");
}

static void removerItemLista(void) {
    if (!inicioLista) {
        printf("\n[!] Lista vazia. Nada a remover.\n");
        return;
    }
    char nome[30];
    printf("\n== Remover Item (Lista) ==\n");
    printf("Informe o nome exato do item: ");
    lerLinha(nome, sizeof(nome));

    No *ant = NULL, *p = inicioLista;
    while (p && strcmp(p->dados.nome, nome) != 0) {
        ant = p; p = p->proximo;
    }
    if (!p) {
        printf("\n[!] Item '%s' não encontrado na lista.\n", nome);
        return;
    }
    if (!ant) inicioLista = p->proximo;
    else ant->proximo = p->proximo;
    free(p);
    printf("\n[OK] Item removido da lista.\n");
}

static void listarItensLista(void) {
    printf("\n===== Itens na mochila (LISTA) =====\n");
    if (!inicioLista) { printf("Lista vazia.\n"); return; }

    printf("%-3s | %-30s | %-12s | %-10s\n", "#", "Nome", "Tipo", "Quantidade");
    printf("----+--------------------------------+--------------+------------\n");
    int i = 1;
    for (No* p = inicioLista; p != NULL; p = p->proximo, i++) {
        printf("%-3d | %-30s | %-12s | %-10d\n",
               i, p->dados.nome, p->dados.tipo, p->dados.quantidade);
    }
}

static No* buscarSequencialLista(const char* nome) {
    comparacoesSequencial = 0;
    for (No* p = inicioLista; p != NULL; p = p->proximo) {
        comparacoesSequencial++;
        if (strcmp(p->dados.nome, nome) == 0) return p;
    }
    return NULL;
}

// ===================== MENU (mochila) =====================

typedef enum {
    ESTRUTURA_VETOR = 1,
    ESTRUTURA_LISTA = 2
} TipoEstrutura;

static TipoEstrutura estruturaAtiva = ESTRUTURA_VETOR;

static void exibirMenuPrincipal(void) {
    printf("\n===== Mochila do Jogador (Loot Avançado) =====\n");
    printf("Estrutura ativa: %s\n", estruturaAtiva == ESTRUTURA_VETOR ? "VETOR" : "LISTA ENCADEADA");
    printf("1. Alternar estrutura (Vetor <-> Lista)\n");
    printf("2. Adicionar um item\n");
    printf("3. Remover um item\n");
    printf("4. Listar todos os itens\n");
    printf("5. Buscar item por nome (Sequencial)\n");
    printf("6. [Vetor] Ordenar por NOME (Selection Sort)\n");
    printf("7. [Vetor] Buscar por nome (BINÁRIA)\n");
    printf("8. >>> Modo: Montagem da Torre (Componentes) <<<\n");
    printf("0. Sair\n");
    printf("Escolha: ");
}

static void opcaoAdicionar(void) {
    double t0 = agora_ms();
    if (estruturaAtiva == ESTRUTURA_VETOR) inserirItemVetor();
    else inserirItemLista();
    double t1 = agora_ms();
    printf("[Tempo] %.3f ms\n", (t1 - t0));
}

static void opcaoRemover(void) {
    double t0 = agora_ms();
    if (estruturaAtiva == ESTRUTURA_VETOR) removerItemVetor();
    else removerItemLista();
    double t1 = agora_ms();
    printf("[Tempo] %.3f ms\n", (t1 - t0));
}

static void opcaoListar(void) {
    double t0 = agora_ms();
    if (estruturaAtiva == ESTRUTURA_VETOR) listarItensVetor();
    else listarItensLista();
    double t1 = agora_ms();
    printf("[Tempo] %.3f ms\n", (t1 - t0));
}

static void opcaoBuscarSequencial(void) {
    char nome[30];
    printf("\n== Buscar Item (Sequencial) ==\n");
    printf("Nome: ");
    lerLinha(nome, sizeof(nome));

    double t0 = agora_ms();
    if (estruturaAtiva == ESTRUTURA_VETOR) {
        int idx = buscarSequencialVetor(nome);
        double t1 = agora_ms();
        if (idx == -1) printf("\n[!] Item '%s' não encontrado no vetor.\n", nome);
        else {
            printf("\n[ENCONTRADO no VETOR]\n");
            printf("Nome: %s\nTipo: %s\nQuantidade: %d\n",
                   mochilaVetor[idx].nome, mochilaVetor[idx].tipo, mochilaVetor[idx].quantidade);
        }
        printf("[Comparações (sequencial)]: %lld\n", comparacoesSequencial);
        printf("[Tempo] %.3f ms\n", (t1 - t0));
    } else {
        No* p = buscarSequencialLista(nome);
        double t1 = agora_ms();
        if (!p) printf("\n[!] Item '%s' não encontrado na lista.\n", nome);
        else {
            printf("\n[ENCONTRADO na LISTA]\n");
            printf("Nome: %s\nTipo: %s\nQuantidade: %d\n",
                   p->dados.nome, p->dados.tipo, p->dados.quantidade);
        }
        printf("[Comparações (sequencial)]: %lld\n", comparacoesSequencial);
        printf("[Tempo] %.3f ms\n", (t1 - t0));
    }
}

static void opcaoOrdenarVetor(void) {
    if (estruturaAtiva != ESTRUTURA_VETOR) {
        printf("\n[!] Ordenação por nome é exclusiva do VETOR.\n");
        return;
    }
    double t0 = agora_ms();
    ordenarVetorPorNome();
    double t1 = agora_ms();
    printf("[Tempo] %.3f ms\n", (t1 - t0));
}

static void opcaoBuscarBinaria(void) {
    if (estruturaAtiva != ESTRUTURA_VETOR) {
        printf("\n[!] Busca binária é exclusiva do VETOR.\n");
        return;
    }
    char nome[30];
    printf("\n== Buscar Item (BINÁRIA no Vetor) ==\n");
    printf("Nome: ");
    lerLinha(nome, sizeof(nome));

    double t0 = agora_ms();
    int idx = buscarBinariaVetor(nome);
    double t1 = agora_ms();

    if (idx == -1) {
        if (!vetorOrdenadoPorNome) { /* aviso já foi exibido */ }
        else printf("\n[!] Item '%s' não encontrado pela busca binária.\n", nome);
    } else {
        printf("\n[ENCONTRADO no VETOR via BINÁRIA]\n");
        printf("Nome: %s\nTipo: %s\nQuantidade: %d\n",
               mochilaVetor[idx].nome, mochilaVetor[idx].tipo, mochilaVetor[idx].quantidade);
    }
    if (vetorOrdenadoPorNome) {
        printf("[Comparações (binária)]: %lld\n", comparacoesBinaria);
        printf("[Tempo] %.3f ms\n", (t1 - t0));
    }
}

/* ============================================================================
   MÓDULO: MONTAGEM DA TORRE (Componentes + Algoritmos didáticos)
   ============================================================================ */

typedef struct {
    char nome[30];
    char tipo[20];
    int  prioridade; // 1..10
} Componente;

#define MAX_COMPONENTES 20
static Componente componentes[MAX_COMPONENTES];
static int numComponentes = 0;
static bool compOrdenadoPorNome = false; // necessário para a busca binária

// Exibe vetor de componentes
static void mostrarComponentes(Componente v[], int n) {
    printf("\n==== Componentes (%d/%d) ====\n", n, MAX_COMPONENTES);
    if (n == 0) { printf("Nenhum componente cadastrado.\n"); return; }
    printf("%-3s | %-30s | %-12s | %-10s\n", "#", "Nome", "Tipo", "Prioridade");
    printf("----+--------------------------------+--------------+------------\n");
    for (int i = 0; i < n; i++) {
        printf("%-3d | %-30s | %-12s | %-10d\n",
               i+1, v[i].nome, v[i].tipo, v[i].prioridade);
    }
}

// Cadastro (até 20)
static void cadastrarComponentes(void) {
    if (numComponentes >= MAX_COMPONENTES) {
        printf("\n[!] Capacidade máxima atingida (%d).\n", MAX_COMPONENTES);
        return;
    }
    int livres = MAX_COMPONENTES - numComponentes;
    int q;
    printf("\nQuantos componentes deseja cadastrar agora? (1..%d): ", livres);
    while (scanf("%d", &q) != 1 || q < 1 || q > livres) {
        printf("Valor inválido. Digite entre 1 e %d: ", livres);
        int ch; while ((ch = getchar()) != '\n' && ch != EOF) {}
    }
    int ch; while ((ch = getchar()) != '\n' && ch != EOF) {}

    for (int i = 0; i < q; i++) {
        Componente c;
        printf("\n== Cadastro %d/%d ==\n", i+1, q);
        printf("Nome (max 29): ");
        lerLinha(c.nome, sizeof(c.nome));
        printf("Tipo (ex: controle, suporte, propulsao) (max 19): ");
        lerLinha(c.tipo, sizeof(c.tipo));
        printf("Prioridade (1..10): ");
        while (scanf("%d", &c.prioridade) != 1 || c.prioridade < 1 || c.prioridade > 10) {
            printf("Inválido. Informe inteiro entre 1 e 10: ");
            int ch2; while ((ch2 = getchar()) != '\n' && ch2 != EOF) {}
        }
        int ch2; while ((ch2 = getchar()) != '\n' && ch2 != EOF) {}

        componentes[numComponentes++] = c;
    }
    compOrdenadoPorNome = false; // novo cadastro pode quebrar ordenação por nome
    printf("\n[OK] Componentes cadastrados!\n");
    mostrarComponentes(componentes, numComponentes);
}

// ---------- ALGORITMOS DE ORDENAÇÃO (com contagem de comparações) ----------

// Bubble Sort por NOME (string)
static void bubbleSortNome(Componente v[], int n, long long *comparacoes) {
    *comparacoes = 0;
    if (n <= 1) { compOrdenadoPorNome = true; return; }
    bool trocou;
    for (int i = 0; i < n - 1; i++) {
        trocou = false;
        for (int j = 0; j < n - 1 - i; j++) {
            (*comparacoes)++; // comparação de chave (nome)
            if (strcmp(v[j].nome, v[j+1].nome) > 0) {
                Componente tmp = v[j]; v[j] = v[j+1]; v[j+1] = tmp;
                trocou = true;
            }
        }
        if (!trocou) break; // pequena otimização
    }
    compOrdenadoPorNome = true;
}

// Insertion Sort por TIPO (string)
static void insertionSortTipo(Componente v[], int n, long long *comparacoes) {
    *comparacoes = 0;
    for (int i = 1; i < n; i++) {
        Componente chave = v[i];
        int j = i - 1;
        // contamos apenas as comparações de strcmp realizadas
        while (j >= 0) {
            (*comparacoes)++;
            if (strcmp(v[j].tipo, chave.tipo) > 0) {
                v[j+1] = v[j];
                j--;
            } else {
                break;
            }
        }
        v[j+1] = chave;
    }
    compOrdenadoPorNome = false; // ordenado por TIPO, não por NOME
}

// Selection Sort por PRIORIDADE (int)
static void selectionSortPrioridade(Componente v[], int n, long long *comparacoes) {
    *comparacoes = 0;
    for (int i = 0; i < n - 1; i++) {
        int minIdx = i;
        for (int j = i + 1; j < n; j++) {
            (*comparacoes)++; // comparação de prioridade
            if (v[j].prioridade < v[minIdx].prioridade) {
                minIdx = j;
            }
        }
        if (minIdx != i) {
            Componente tmp = v[i]; v[i] = v[minIdx]; v[minIdx] = tmp;
        }
    }
    compOrdenadoPorNome = false; // ordenado por PRIORIDADE, não por NOME
}

// Função genérica para medir tempo de um algoritmo de ordenação
// Assinatura do algoritmo: void alg(Componente*, int, long long*)
typedef void (*AlgOrdenacao)(Componente*, int, long long*);
static double medirTempoOrdenacao(AlgOrdenacao alg, Componente v[], int n, long long *compOut) {
    clock_t t0 = clock();
    alg(v, n, compOut);
    clock_t t1 = clock();
    return ((double)(t1 - t0) * 1000.0 / CLOCKS_PER_SEC); // ms
}

// ---------- BUSCA BINÁRIA POR NOME (somente após ordenar por NOME) ----------
static int buscaBinariaPorNome(Componente v[], int n, const char alvo[], long long *comparacoes) {
    *comparacoes = 0;
    if (!compOrdenadoPorNome) {
        printf("\n[!] A busca binária exige o vetor ORDENADO por NOME (use Bubble Sort).\n");
        return -1;
    }
    int ini = 0, fim = n - 1;
    while (ini <= fim) {
        int meio = (ini + fim) / 2;
        (*comparacoes)++;
        int cmp = strcmp(v[meio].nome, alvo);
        if (cmp == 0) return meio;
        else if (cmp < 0) ini = meio + 1;
        else fim = meio - 1;
    }
    return -1;
}

// ---------- MENU do Módulo “Montagem da Torre” ----------
static void exibirMenuTorre(void) {
    printf("\n===== Montagem da Torre (Componentes) =====\n");
    printf("1. Cadastrar componentes (até 20)\n");
    printf("2. Listar componentes\n");
    printf("3. Ordenar por NOME [Bubble Sort]\n");
    printf("4. Ordenar por TIPO [Insertion Sort]\n");
    printf("5. Ordenar por PRIORIDADE [Selection Sort]\n");
    printf("6. Busca BINÁRIA por NOME (após ordenar por NOME)\n");
    printf("0. Voltar ao menu principal\n");
    printf("Escolha: ");
}

static void loopTorre(void) {
    int op;
    do {
        exibirMenuTorre();
        if (scanf("%d", &op) != 1) {
            int ch; while ((ch = getchar()) != '\n' && ch != EOF) {}
            printf("\nEntrada inválida.\n");
            continue;
        }
        int ch; while ((ch = getchar()) != '\n' && ch != EOF) {}

        if (op == 1) {
            cadastrarComponentes();
            pausa();
        } else if (op == 2) {
            mostrarComponentes(componentes, numComponentes);
            printf("Ordenado por NOME? %s\n", compOrdenadoPorNome ? "SIM" : "NÃO");
            pausa();
        } else if (op == 3) {
            long long comps = 0;
            double ms = medirTempoOrdenacao(bubbleSortNome, componentes, numComponentes, &comps);
            printf("\n[OK] Bubble Sort (NOME) concluído.\n");
            printf("Comparações: %lld | Tempo: %.3f ms\n", comps, ms);
            mostrarComponentes(componentes, numComponentes);
            pausa();
        } else if (op == 4) {
            long long comps = 0;
            double ms = medirTempoOrdenacao(insertionSortTipo, componentes, numComponentes, &comps);
            printf("\n[OK] Insertion Sort (TIPO) concluído.\n");
            printf("Comparações: %lld | Tempo: %.3f ms\n", comps, ms);
            mostrarComponentes(componentes, numComponentes);
            pausa();
        } else if (op == 5) {
            long long comps = 0;
            double ms = medirTempoOrdenacao(selectionSortPrioridade, componentes, numComponentes, &comps);
            printf("\n[OK] Selection Sort (PRIORIDADE) concluído.\n");
            printf("Comparações: %lld | Tempo: %.3f ms\n", comps, ms);
            mostrarComponentes(componentes, numComponentes);
            pausa();
        } else if (op == 6) {
            if (numComponentes == 0) { printf("\n[!] Nenhum componente cadastrado.\n"); pausa(); continue; }
            char chave[30];
            printf("\nComponente-chave (nome): ");
            lerLinha(chave, sizeof(chave));
            long long comps = 0;
            clock_t t0 = clock();
            int idx = buscaBinariaPorNome(componentes, numComponentes, chave, &comps);
            clock_t t1 = clock();
            double ms = ((double)(t1 - t0) * 1000.0 / CLOCKS_PER_SEC);
            if (idx >= 0) {
                printf("\n[ENCONTRADO] Componente-chave presente!\n");
                printf("Nome: %s | Tipo: %s | Prioridade: %d\n",
                       componentes[idx].nome, componentes[idx].tipo, componentes[idx].prioridade);
            } else {
                printf("\n[NÃO ENCONTRADO] Componente-chave '%s' ausente.\n", chave);
            }
            printf("Comparações (busca binária): %lld | Tempo: %.3f ms\n", comps, ms);
            // Visual confirm: mostrar tabela completa após a busca
            mostrarComponentes(componentes, numComponentes);
            pausa();
        } else if (op == 0) {
            break;
        } else {
            printf("\nOpção inválida.\n");
        }
        limparTela();
    } while (op != 0);
}

/* ===================== MAIN ===================== */

int main(void) {
    int opcao;
    do {
        exibirMenuPrincipal();
        if (scanf("%d", &opcao) != 1) {
            int ch; while ((ch = getchar()) != '\n' && ch != EOF) {}
            printf("\nEntrada inválida.\n");
            continue;
        }
        int ch; while ((ch = getchar()) != '\n' && ch != EOF) {}

        switch (opcao) {
            case 1: {
                estruturaAtiva = (estruturaAtiva == ESTRUTURA_VETOR)
                                 ? ESTRUTURA_LISTA : ESTRUTURA_VETOR;
                printf("\n[OK] Estrutura ativa agora é: %s\n",
                       estruturaAtiva == ESTRUTURA_VETOR ? "VETOR" : "LISTA ENCADEADA");
                pausa();
            } break;

            case 2:
                opcaoAdicionar();
                listarItensVetor();
                listarItensLista();
                pausa();
                break;

            case 3:
                opcaoRemover();
                listarItensVetor();
                listarItensLista();
                pausa();
                break;

            case 4:
                opcaoListar();
                if (estruturaAtiva == ESTRUTURA_VETOR) listarItensLista();
                else listarItensVetor();
                pausa();
                break;

            case 5:
                opcaoBuscarSequencial();
                pausa();
                break;

            case 6:
                opcaoOrdenarVetor();
                listarItensVetor();
                pausa();
                break;

            case 7:
                opcaoBuscarBinaria();
                pausa();
                break;

            case 8:
                loopTorre();
                break;

            case 0:
                printf("\nSaindo...\n");
                break;

            default:
                printf("\nOpção inválida.\n");
        }
        limparTela();
    } while (opcao != 0);

    // Libera memória da lista antes de sair
    while (inicioLista) {
        No* tmp = inicioLista;
        inicioLista = inicioLista->proximo;
        free(tmp);
    }

    return 0;
}
