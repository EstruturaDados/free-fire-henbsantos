#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

// Código da Ilha – Edição Free Fire
// Nível: Mestre
// Este programa simula o gerenciamento avançado de uma mochila com componentes coletados durante a fuga de uma ilha.
// Ele introduz ordenação com critérios e busca binária para otimizar a gestão dos recursos.

// ===================== DADOS DO INVENTÁRIO (encaixados conforme as seções) =====================

// Struct Item:
// Representa um componente com nome, tipo, quantidade e prioridade (1 a 5).
// A prioridade indica a importância do item na montagem do plano de fuga.
typedef struct {
    char nome[30];
    char tipo[20];     // ex.: "arma", "municao", "cura"
    int  quantidade;
    // prioridade será usada na próxima etapa (comentário preservado)
} Item;

// Vetor mochila:
// Armazena até 10 itens coletados.
// Variáveis de controle: numItens (quantidade atual), comparacoes (análise de desempenho), ordenadaPorNome (para controle da busca binária).
#define CAPACIDADE 10
static Item mochila[CAPACIDADE];
static int numItens = 0;
// variáveis abaixo são reservadas para a próxima etapa (comentário preservado)
static long long comparacoes = 0;
static bool ordenadaPorNome = false;

// Enum CriterioOrdenacao:
// Define os critérios possíveis para a ordenação dos itens (nome, tipo ou prioridade).
// (Será usado na próxima etapa, mantendo o comentário)
typedef enum {
    ORDENAR_POR_NOME = 1,
    ORDENAR_POR_TIPO = 2,
    ORDENAR_POR_PRIORIDADE = 3
} CriterioOrdenacao;


// limparTela():
// Simula a limpeza da tela imprimindo várias linhas em branco.
static void limparTela(void) {
    for (int i = 0; i < 12; i++) puts("");
}

// =============== INTERFACE / MENU (encaixado) ==================

// exibirMenu():
// Apresenta o menu principal ao jogador, com destaque para status da ordenação.
static void exibirMenu(void) {
    printf("\n===== Mochila do Jogador (Loot Inicial) =====\n");
    printf("1. Adicionar um item\n");
    printf("2. Remover um item\n");
    printf("3. Listar todos os itens\n");
    printf("4. Ordenar os itens por critério (nome, tipo, prioridade)\n");
    printf("5. Realizar busca binária por nome\n");
    printf("0. Sair\n");
    printf("Escolha: ");
}

// ===================== I/O UTIL =====================
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

// ===================== OPERAÇÕES NESTA ETAPA =====================

// inserirItem():
// Adiciona um novo componente à mochila se houver espaço.
// Solicita nome, tipo, quantidade e prioridade.
// Após inserir, marca a mochila como "não ordenada por nome".
static int indicePorNomeSequencial(const char *nome) {
    for (int i = 0; i < numItens; i++) {
        if (strcmp(mochila[i].nome, nome) == 0) return i;
    }
    return -1;
}

static void inserirItem(void) {
    if (numItens >= CAPACIDADE) {
        printf("\n[!] Mochila cheia. Remova algo antes de adicionar.\n");
        return;
    }

    Item novo;
    printf("\n== Cadastro de Item ==\n");
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

    int idx = indicePorNomeSequencial(novo.nome);
    if (idx != -1) {
        printf("\n[!] Já existe item com esse nome. Somando quantidades.\n");
        mochila[idx].quantidade += novo.quantidade;
    } else {
        mochila[numItens++] = novo;
        printf("\n[OK] Item adicionado!\n");
    }
    ordenadaPorNome = false; // ainda será usado na próxima etapa
}

// removerItem():
// Permite remover um componente da mochila pelo nome.
// Se encontrado, reorganiza o vetor para preencher a lacuna.
static void removerItem(void) {
    if (numItens == 0) {
        printf("\n[!] Mochila vazia. Nada a remover.\n");
        return;
    }

    char nome[30];
    printf("\n== Remover Item ==\n");
    printf("Informe o nome exato do item: ");
    lerLinha(nome, sizeof(nome));

    int idx = indicePorNomeSequencial(nome);
    if (idx == -1) {
        printf("\n[!] Item '%s' não encontrado.\n", nome);
        return;
    }
    for (int i = idx; i < numItens - 1; i++) mochila[i] = mochila[i+1];
    numItens--;
    printf("\n[OK] Item removido.\n");
    // estado de ordenação fica indefinido; manteremos como estava
}

// listarItens():
// Exibe uma tabela formatada com todos os componentes presentes na mochila.
static void listarItens(void) {
    printf("\n===== Itens na mochila (%d/%d) =====\n", numItens, CAPACIDADE);
    if (numItens == 0) {
        printf("Mochila vazia.\n");
        return;
    }
    printf("%-3s | %-30s | %-12s | %-10s\n", "#", "Nome", "Tipo", "Quantidade");
    printf("----+--------------------------------+--------------+------------\n");
    for (int i = 0; i < numItens; i++) {
        printf("%-3d | %-30s | %-12s | %-10d\n",
               i + 1, mochila[i].nome, mochila[i].tipo, mochila[i].quantidade);
    }
}

// Busca SEQUENCIAL (exigida nesta etapa)
static void buscarItemSequencial(void) {
    if (numItens == 0) {
        printf("\n[!] Mochila vazia.\n");
        return;
    }
    char nome[30];
    printf("\n== Buscar Item (sequencial) ==\n");
    printf("Nome: ");
    lerLinha(nome, sizeof(nome));

    int idx = indicePorNomeSequencial(nome);
    if (idx == -1) {
        printf("\n[!] Item '%s' não encontrado.\n", nome);
        return;
    }
    printf("\n[ENCONTRADO]\n");
    printf("Nome: %s\nTipo: %s\nQuantidade: %d\n",
           mochila[idx].nome, mochila[idx].tipo, mochila[idx].quantidade);
}

// ===================== PLACEHOLDERS (próxima etapa, mantidos) =====================

// menuDeOrdenacao():
// Permite ao jogador escolher como deseja ordenar os itens.
// Utiliza a função insertionSort() com o critério selecionado.
// Exibe a quantidade de comparações feitas (análise de desempenho).
static void insertionSort(CriterioOrdenacao crit); // protótipo, implementação futura

static void menuDeOrdenacao(void) {
    printf("\n[INFO] Ordenação será implementada na próxima etapa.\n");
    printf("Critérios: nome, tipo, prioridade. (comentários preservados)\n");
    // Quando implementar: perguntar critério, chamar insertionSort(crit) e exibir comparacoes.
}

// insertionSort():
// Implementação do algoritmo de ordenação por inserção.
// Funciona com diferentes critérios de ordenação:
// - Por nome (ordem alfabética)
// - Por tipo (ordem alfabética)
// - Por prioridade (da mais alta para a mais baixa)
static void insertionSort(CriterioOrdenacao crit) {
    (void)crit;
    printf("\n[INFO] insertionSort ainda não implementado nesta etapa.\n");
}

// buscaBinariaPorNome():
// Realiza busca binária por nome, desde que a mochila esteja ordenada por nome.
// Se encontrar, exibe os dados do item buscado.
// Caso contrário, informa que não encontrou o item.
static void buscaBinariaPorNome(void) {
    printf("\n[INFO] Nesta etapa a busca é SEQUENCIAL. "
           "A busca binária será habilitada após a ordenação por nome.\n");
    buscarItemSequencial();
}


// ===================== PROGRAMA PRINCIPAL (encaixado no main existente) =====================

int main() {
    // Menu principal com opções:
    // 1. Adicionar um item
    // 2. Remover um item
    // 3. Listar todos os itens
    // 4. Ordenar os itens por critério (nome, tipo, prioridade)
    // 5. Realizar busca binária por nome
    // 0. Sair

    // A estrutura switch trata cada opção chamando a função correspondente.
    // A ordenação e busca binária exigem que os dados estejam bem organizados.

    int opcao;
    do {
        exibirMenu();
        if (scanf("%d", &opcao) != 1) {
            int ch; while ((ch = getchar()) != '\n' && ch != EOF) {}
            printf("\nEntrada inválida.\n");
            continue;
        }
        int ch; while ((ch = getchar()) != '\n' && ch != EOF) {}

        switch (opcao) {
            case 1:
                inserirItem();
                listarItens(); // Listagem após cada operação
                pausa();
                break;
            case 2:
                removerItem();
                listarItens(); // Listagem após cada operação
                pausa();
                break;
            case 3:
                listarItens();
                pausa();
                break;
            case 4:
                menuDeOrdenacao(); // placeholder nesta etapa
                listarItens();
                pausa();
                break;
            case 5:
                buscaBinariaPorNome(); // chama a mensagem e executa a busca sequencial
                listarItens();         // Listagem após a operação
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

    return 0;
}
