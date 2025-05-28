#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "questoes.h"

void inicializarBanco(BancoDeQuestoes* banco) {
    banco->quantidade = 0;
    banco->capacidade = 10;
    banco->vetor = (Pergunta*)malloc(sizeof(Pergunta) * banco->capacidade);
}

void liberarBanco(BancoDeQuestoes* banco) {
    free(banco->vetor);
}

void inserirPergunta(BancoDeQuestoes* banco, Pergunta p) {
    if (banco->quantidade >= banco->capacidade) {
        banco->capacidade *= 2;
        banco->vetor = realloc(banco->vetor, sizeof(Pergunta) * banco->capacidade);
    }
    banco->vetor[banco->quantidade++] = p;
}

int carregarCSV(BancoDeQuestoes* banco, const char* nomeArquivo) {
    FILE* arquivo = fopen(nomeArquivo, "r");
    if (!arquivo) return 0;

    char linha[1024];
    while (fgets(linha, sizeof(linha), arquivo)) {
        Pergunta p;
        char nivelStr[5];

        // Quebra pelo delimitador ;
        sscanf(linha, "%[^;];%[^;];%[^;];%[^;];%[^;];%c;%s",
            p.enunciado,
            p.alternativas[0],
            p.alternativas[1],
            p.alternativas[2],
            p.alternativas[3],
            &p.correta,
            nivelStr
        );
        p.nivel = atoi(nivelStr);
        inserirPergunta(banco, p);
    }

    fclose(arquivo);
    return 1;
}

int salvarBinario(BancoDeQuestoes* banco, const char* nomeArquivo) {
    FILE* arquivo = fopen(nomeArquivo, "wb");
    if (!arquivo) return 0;

    fwrite(&banco->quantidade, sizeof(int), 1, arquivo);
    fwrite(banco->vetor, sizeof(Pergunta), banco->quantidade, arquivo);
    fclose(arquivo);
    return 1;
}

int carregarBinario(BancoDeQuestoes* banco, const char* nomeArquivo) {
    FILE* arquivo = fopen(nomeArquivo, "rb");
    if (!arquivo) return 0;

    int qtd;
    fread(&qtd, sizeof(int), 1, arquivo);
    banco->quantidade = qtd;
    banco->capacidade = qtd + 10;
    banco->vetor = (Pergunta*)malloc(sizeof(Pergunta) * banco->capacidade);
    fread(banco->vetor, sizeof(Pergunta), qtd, arquivo);
    fclose(arquivo);
    return 1;
}

int exportarParaCSV(BancoDeQuestoes* banco, const char* nomeArquivo) {
    FILE* arquivo = fopen(nomeArquivo, "w");
    if (!arquivo) return 0;

    for (int i = 0; i < banco->quantidade; i++) {
        Pergunta p = banco->vetor[i];
        fprintf(arquivo, "%s;%s;%s;%s;%s;%c;%d\n",
            p.enunciado,
            p.alternativas[0],
            p.alternativas[1],
            p.alternativas[2],
            p.alternativas[3],
            p.correta,
            p.nivel
        );
    }

    fclose(arquivo);
    return 1;
}

void listarPerguntas(BancoDeQuestoes* banco) {
    for (int i = 0; i < banco->quantidade; i++) {
        Pergunta p = banco->vetor[i];
        printf("[%d] %s\nA) %s\nB) %s\nC) %s\nD) %s\nResposta: %c | Nível: %d\n\n",
            i + 1,
            p.enunciado,
            p.alternativas[0],
            p.alternativas[1],
            p.alternativas[2],
            p.alternativas[3],
            p.correta,
            p.nivel
        );
    }
}

void listarPorNivel(BancoDeQuestoes* banco, int nivel) {
    for (int i = 0; i < banco->quantidade; i++) {
        Pergunta p = banco->vetor[i];
        if (p.nivel == nivel) {
            printf("[%d] %s\nA) %s\nB) %s\nC) %s\nD) %s\nResposta: %c\n\n",
                i + 1,
                p.enunciado,
                p.alternativas[0],
                p.alternativas[1],
                p.alternativas[2],
                p.alternativas[3],
                p.correta
            );
        }
    }
}
