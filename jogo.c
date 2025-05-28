#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "jogo.h"
#include "funcoes.h"
#include "alternativa.h"
#include "dificuldade.h"
#include "pergunta.h"
#include "questoes.h"

// Função interna para selecionar perguntas de um nível específico
void selecionarPerguntasNivel(BancoDeQuestoes* banco, Pergunta* destino, int* indice, int nivel, int qtd) {
    Pergunta* temp[qtd];
    int count = 0;

    // Coleta todas as perguntas do nível desejado
    for (int i = 0; i < banco->quantidade; i++) {
        if (banco->vetor[i].nivel == nivel) {
            if (count < qtd) {
                temp[count++] = &banco->vetor[i];
            }
        }
    }

    // Embaralha
    for (int i = 0; i < count; i++) {
        int j = rand() % count;
        Pergunta* aux = temp[i];
        temp[i] = temp[j];
        temp[j] = aux;
    }

    // Copia as primeiras 'qtd' perguntas para o destino
    for (int i = 0; i < qtd && i < count; i++) {
        destino[(*indice)++] = *temp[i];
    }
}

void jogar(BancoDeQuestoes* banco) {
    Pergunta perguntasSelecionadas[15];
    int index = 0;

    srand(time(NULL)); // Aleatoriedade

    selecionarPerguntasNivel(banco, perguntasSelecionadas, &index, 1, 2); // Muito fácil
    selecionarPerguntasNivel(banco, perguntasSelecionadas, &index, 2, 2); // Fácil
    selecionarPerguntasNivel(banco, perguntasSelecionadas, &index, 3, 4); // Médio
    selecionarPerguntasNivel(banco, perguntasSelecionadas, &index, 4, 4); // Difícil
    selecionarPerguntasNivel(banco, perguntasSelecionadas, &index, 5, 3); // Muito difícil

    int acertos = 0;
    int marco1 = 5;
    int marco2 = 10;
    int premio = 0;
    int premios[15] = {
        1000, 2000, 5000, 10000, 20000,
        30000, 40000, 50000, 75000, 100000,
        150000, 200000, 300000, 500000, 1000000
    };

    for (int i = 0; i < 15; i++) {
        limparTela();
        printf("🏆 Pergunta %d - Valendo R$ %d\n", i + 1, premios[i]);
        printf("%s\n", perguntasSelecionadas[i].enunciado);
        printf("A) %s\n", perguntasSelecionadas[i].alternativas[0]);
        printf("B) %s\n", perguntasSelecionadas[i].alternativas[1]);
        printf("C) %s\n", perguntasSelecionadas[i].alternativas[2]);
        printf("D) %s\n", perguntasSelecionadas[i].alternativas[3]);

        printf("\nSua resposta (A/B/C/D ou S para sair): ");
        char resp;
        scanf(" %c", &resp);
        resp = toupper(resp);

        if (resp == 'S') {
            printf("Você saiu com R$ %d!\n", premio);
            pausarTela();
            return;
        }

        if (resp == perguntasSelecionadas[i].correta) {
            printf("✅ Resposta correta!\n");
            acertos++;
            premio = premios[i];
        } else {
            printf("❌ Resposta errada! A correta era: %c\n", perguntasSelecionadas[i].correta);
            if (i >= marco2)
                premio = premios[marco2 - 1];
            else if (i >= marco1)
                premio = premios[marco1 - 1];
            else
                premio = 0;
            printf("Você sai com R$ %d\n", premio);
            pausarTela();
            return;
        }
        pausarTela();
    }

    limparTela();
    printf("🎉 PARABÉNS! Você ganhou R$ 1.000.000,00! 🎉\n");
    pausarTela();
}
