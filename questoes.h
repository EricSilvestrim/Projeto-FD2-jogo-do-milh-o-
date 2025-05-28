#ifndef QUESTOES_H
#define QUESTOES_H

#include "pergunta.h"

// Estrutura que gerencia o vetor dinâmico de perguntas
typedef struct {
    Pergunta* vetor;
    int quantidade;
    int capacidade;
} BancoDeQuestoes;

// Funções principais
void inicializarBanco(BancoDeQuestoes* banco);
void liberarBanco(BancoDeQuestoes* banco);

int carregarCSV(BancoDeQuestoes* banco, const char* nomeArquivo);
int salvarBinario(BancoDeQuestoes* banco, const char* nomeArquivo);
int carregarBinario(BancoDeQuestoes* banco, const char* nomeArquivo);
int exportarParaCSV(BancoDeQuestoes* banco, const char* nomeArquivo);

void inserirPergunta(BancoDeQuestoes* banco, Pergunta p);
void listarPerguntas(BancoDeQuestoes* banco);
void listarPorNivel(BancoDeQuestoes* banco, int nivel);

#endif
