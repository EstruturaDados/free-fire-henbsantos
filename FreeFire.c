#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

//==================================================================
// – Edição Free Fire 
//

// ===================== DADOS DO INVENTÁRIO (comuns) =====================

// Struct Item:
// Representa um componente com nome, tipo e quantidade.
// (prioridade foi retirada nesta etapa para focar nos requisitos solicitados)
typedef struct {
    char nome[30];
    char tipo[20];     // ex.: "arma", "municao", "cura"
    int  quantidade;
} Item;

// Struct No (lista encadeada):
// Nó contendo um Item e ponteiro para o próximo elemento.
typedef struct No {
    Item dados;
    struct No* proximo;
} No;

// ===================== AJUDA / I/O / UTIL =====================

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

// Cronômetro simples para tempo de operação
static double agora_ms(void) {
    return (double)clock() * 1000.0 / CLOCKS_PER_SEC;
}

// ===================== CONTADORES GLOBAIS DE BUSCA =====================
// São atualizados SOMENTE durante as buscas, como solicitado.
static long long comparacoesSequencial = 0;
static long long comparacoesBinaria    = 0;

// ===================== MOCHILA COM VETOR (LISTA SEQUENCIAL) =====================

#define CAPACIDADE 10
static Item mochilaVetor[CAPACIDADE];
static int  numItensVetor = 0;
static bool vetorOrdenadoPorNome = false; // verdadeiro após ordenar por nome

// Inserir (vetor):
// - Se nome já existe, soma quantidades.
// - Caso contrário, insere no fim se houver espaço.
// - Marca vetor como "não ordenado".
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

    // Verifica duplicado por nome (sequencial, sem contar comparações aqui)
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

// Remover (vetor) por nome:
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
    vetorOrdenadoPorNome = false; // estrutura mudou
    printf("\n[OK] Item removido do vetor.\n");
}

// Listar (vetor):
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

// Busca sequencial (vetor):
// Retorna índice encontrado ou -1. Atualiza comparacoesSequencial.
static int buscarSequencialVetor(const char* nome) {
    comparacoesSequencial = 0;
    for (int i = 0; i < numItensVetor; i++) {
        comparacoesSequencial++; // comparando nome
        if (strcmp(mochilaVetor[i].nome, nome) == 0) return i;
    }
    return -1;
}

// Ordenação por NOME (vetor) usando Selection Sort:
// Ordem alfabética crescente pelo campo nome.
static void ordenarVetorPorNome(void) {
    if (numItensVetor <= 1) { vetorOrdenadoPorNome = true; return; }

    // Selection Sort clássico
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
    printf("\n[OK] Vetor ordenado por NOME (Selection Sort).\n");
}

// Busca BINÁRIA (vetor) por nome:
// Requer vetorOrdenadoPorNome == true.
// Retorna índice ou -1. Atualiza comparacoesBinaria.
static int buscarBinariaVetor(const char* nome) {
    if (!vetorOrdenadoPorNome) {
        printf("\n[!] A busca binária exige o vetor ORDENADO por nome.\n");
        return -1;
    }
    comparacoesBinaria = 0;
    int ini = 0, fim = numItensVetor - 1;
    while (ini <= fim) {
        int meio = (ini + fim) / 2;
        comparacoesBinaria++; // comparação principal
        int cmp = strcmp(mochilaVetor[meio].nome, nome);
        if (cmp == 0) return meio;
        else if (cmp < 0) ini = meio + 1;
        else fim = meio - 1;
    }
    return -1;
}

// ===================== MOCHILA COM LISTA ENCADEADA =====================

static No* inicioLista = NULL; // cabeça da lista

// Inserir (lista):
// - Se nome já existe, soma quantidades.
// - Caso contrário, insere no início (O(1)). (Pode-se optar por fim; aqui início)
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

    // Procura duplicado por nome
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

// Remover (lista) por nome:
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

// Listar (lista):
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

// Busca sequencial (lista):
// Retorna ponteiro para nó encontrado ou NULL. Atualiza comparacoesSequencial.
static No* buscarSequencialLista(const char* nome) {
    comparacoesSequencial = 0;
    for (No* p = inicioLista; p != NULL; p = p->proximo) {
        comparacoesSequencial++; // comparação de nome
        if (strcmp(p->dados.nome, nome) == 0) return p;
    }
    return NULL;
}

// ===================== MENUS E ORQUESTRAÇÃO =====================

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
    printf("0. Sair\n");
    printf("Escolha: ");
}

// Fluxos das operações (roteiam para a estrutura ativa)
// — medem o tempo da operação para reforçar o requisito de desempenho (< 2s)

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
        if (!vetorOrdenadoPorNome) {
            // mensagem já exibida em buscarBinariaVetor(), aqui só complementamos
        } else {
            printf("\n[!] Item '%s' não encontrado pela busca binária.\n", nome);
        }
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

// ===================== PROGRAMA PRINCIPAL =====================

int main(void) {
    // Menu principal com opções para alternar entre VETOR e LISTA encadeada.
    // Após cada operação, listamos o inventário e medimos tempo de execução
    // para evidenciar o requisito de eficiência (< 2s).

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
            case 1:
                estruturaAtiva = (estruturaAtiva == ESTRUTURA_VETOR)
                                 ? ESTRUTURA_LISTA : ESTRUTURA_VETOR;
                printf("\n[OK] Estrutura ativa agora é: %s\n",
                       estruturaAtiva == ESTRUTURA_VETOR ? "VETOR" : "LISTA ENCADEADA");
                pausa();
                break;

            case 2:
                opcaoAdicionar();
                listarItensVetor();   // sempre mostramos os dois para comparação
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
                // também mostramos a outra estrutura para o jogador comparar
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
