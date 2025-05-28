#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include "pergunta.h"
#include "funcoes.h"
#include "jogo.h"
#include "questoes.h"
#include "alternativa.h"
#include "dificuldade.h"

void exibirMenu() {
    limparTela();
    printf("====================================\n");
    printf("       💰 JOGO DO MILHÃO 💰\n");
    printf("====================================\n");
    printf("1 - Jogar\n");
    printf("2 - Gerenciar Perguntas (CRUD)\n");
    printf("3 - Exportar perguntas para CSV\n");
    printf("4 - Sair\n");
    printf("Escolha uma opção: ");
}

void menuCrud(BancoDeQuestoes* banco) {
    int opcao;
    do {
        limparTela();
        printf("===== MENU DE PERGUNTAS =====\n");
        printf("1 - Inserir nova pergunta\n");
        printf("2 - Listar perguntas\n");
        printf("3 - Pesquisar pergunta\n");
        printf("4 - Alterar pergunta\n");
        printf("5 - Remover pergunta\n");
        printf("6 - Voltar ao menu principal\n");
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);
        limparBuffer();

        switch (opcao) {
            case 1:
                inserirPergunta(banco);
                break;
            case 2:
                listarPerguntas(banco);
                break;
            case 3:
                pesquisarPergunta(banco);
                break;
            case 4:
                alterarPergunta(banco);
                break;
            case 5:
                removerPergunta(banco);
                break;
            case 6:
                printf("Voltando ao menu principal...\n");
                break;
            default:
                printf("Opção inválida!\n");
        }
        if (opcao != 6) pausarTela();
    } while (opcao != 6);
}

int main() {
    setlocale(LC_ALL, ""); // Suporte a acentos (Linux)
    BancoDeQuestoes banco;
    inicializarBanco(&banco);

    if (!carregarBinario(&banco, "dados/perguntas.bin")) {
        printf("📝 Nenhum arquivo binário encontrado. Lendo CSV inicial...\n");
        if (carregarCSV(&banco, "dados/perguntas.csv")) {
            printf("✅ Perguntas carregadas do CSV com sucesso!\n");
        } else {
            printf("⚠️ Erro ao carregar o arquivo CSV!\n");
        }
        pausarTela();
    }

    int opcao;
    do {
        exibirMenu();
        scanf("%d", &opcao);
        limparBuffer();

        switch (opcao) {
            case 1:
                jogar(&banco);
                break;
            case 2:
                menuCrud(&banco);
                break;
            case 3:
                exportarCSV(&banco, "dados/exportadas.csv");
                printf("📁 Perguntas exportadas com sucesso!\n");
                pausarTela();
                break;
            case 4:
                printf("Salvando e saindo...\n");
                salvarBinario(&banco, "dados/perguntas.bin");
                liberarBanco(&banco);
                break;
            default:
                printf("Opção inválida!\n");
                pausarTela();
        }
    } while (opcao != 4);

    return 0;
}
