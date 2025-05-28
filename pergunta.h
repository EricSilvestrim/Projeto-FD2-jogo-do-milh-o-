#ifndef PERGUNTA_H
#define PERGUNTA_H

#define TAM_ENUNCIADO 256
#define TAM_ALTERNATIVA 100

#include "questoes.h"

typedef struct {
    char enunciado[TAM_ENUNCIADO];
    char alternativas[4][TAM_ALTERNATIVA];
    char correta; // 'A', 'B', 'C' ou 'D'
    int nivel; // 1 a 5
} Pergunta;

// Carregamento de dados
Pergunta* carregarCSV(const char *arquivo, int *total);
Pergunta* carregarBinario(const char *arquivo, int *total);
void salvarBinario(const char *arquivo, Pergunta *lista, int total);

// CRUD
void inserirPergunta(BancoDeQuestoes *banco);
void listarPerguntas(BancoDeQuestoes *banco);
void pesquisarPorNivel(Pergunta *lista, int total, int nivel);
void alterarPergunta(BancoDeQuestoes *banco);
Pergunta* excluirPergunta(Pergunta *lista, int *total);

// Exportar CSV
void exportarCSV(BancoDeQuestoes *banco, const char *nomeArquivo);

#endif
