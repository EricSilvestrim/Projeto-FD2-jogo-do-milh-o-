#ifndef QUESTAO
#define QUESTAO

#include "alternativa.h"
#include "dificuldade.h"

typedef struct {
    char *enunciado;
    Alternativa alternativas[4];
    char correta;       // Letra da alternativa correta: 'A', 'B', 'C' ou 'D'
    Dificuldade nivel;  // Nível de dificuldade (1 a 5)
} Pergunta;

// Protótipos das funções para manipulação das perguntas

// Recebe uma nova pergunta, adiciona ao vetor de perguntas e atualiza total
Pergunta** recebePergunta(Pergunta **perguntas, int *total);

// Lista todas as perguntas, organizadas por nível
void listaPerguntas(Pergunta **perguntas, int total);

// Pesquisa perguntas que contenham um texto no enunciado e exibe resultados
void pesquisaPergunta(Pergunta **perguntas, int total);

// Altera pergunta em um índice especificado
void alterarPergunta(Pergunta **perguntas, int total);

// Exclui pergunta em índice especificado e realoca vetor
void excluirPergunta(Pergunta **perguntas, int *total);

#endif
