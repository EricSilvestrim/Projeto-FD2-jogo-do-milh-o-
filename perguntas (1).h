// /home/ubuntu/include/perguntas.h
#ifndef PERGUNTAS_H
#define PERGUNTAS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Constantes para tamanhos maximos
#define MAX_ENUNCIADO 512
#define MAX_ALTERNATIVAS 256
#define MAX_RESPOSTA 1
#define MAX_PERGUNTAS_INICIAL 100 // Tamanho inicial do vetor dinamico

// Estrutura para armazenar uma pergunta
typedef struct {
    char enunciado[MAX_ENUNCIADO];
    char alternativaA[MAX_ALTERNATIVAS];
    char alternativaB[MAX_ALTERNATIVAS];
    char alternativaC[MAX_ALTERNATIVAS];
    char alternativaD[MAX_ALTERNATIVAS];
    char respostaCorreta; // 'A', 'B', 'C' ou 'D'
    int dificuldade;      // Nivel de 1 a 5
    int id;               // Identificador unico (pode ser util)
} Pergunta;

// Estrutura para gerenciar o vetor dinamico de perguntas
typedef struct {
    Pergunta *perguntas;
    int quantidadeAtual;
    int capacidade;
} BancoDePerguntas;

// Protótipos das Funções (a serem implementadas em .c)

// Inicialização e Liberação
BancoDePerguntas* criarBancoDePerguntas();
void liberarBancoDePerguntas(BancoDePerguntas *banco);

// Carregamento e Salvamento
int carregarPerguntasCSV(BancoDePerguntas *banco, const char *nomeArquivo);
int salvarPerguntasBinario(const BancoDePerguntas *banco, const char *nomeArquivo);
int carregarPerguntasBinario(BancoDePerguntas *banco, const char *nomeArquivo);
int exportarPerguntasCSV(const BancoDePerguntas *banco, const char *nomeArquivo);

// Gerenciamento (CRUD)
int adicionarPergunta(BancoDePerguntas *banco, Pergunta novaPergunta);
void listarPerguntasPorNivel(const BancoDePerguntas *banco, int nivel);
void listarTodasPerguntas(const BancoDePerguntas *banco);
Pergunta* buscarPerguntaPorId(const BancoDePerguntas *banco, int id);
Pergunta* buscarPerguntaPorEnunciado(const BancoDePerguntas *banco, const char *enunciadoParcial);
int alterarPergunta(BancoDePerguntas *banco, int id);
int excluirPergunta(BancoDePerguntas *banco, int id);

// Lógica do Jogo
void iniciarJogo(BancoDePerguntas *banco);

// Funções Auxiliares
void limparBufferEntrada();
int obterOpcaoMenu();
void exibirMenuPrincipal();
int validarPergunta(const Pergunta *p);
int proximoIdDisponivel(const BancoDePerguntas *banco);

#endif // PERGUNTAS_H

