#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../headers/leitor_csv.h"
#include "../headers/funcoes_padrao.h"  // para converteMaiscula
#include "../headers/pergunta.h"        // para struct Pergunta

// Função para remover espaços no início e fim de uma string
char* trim(char *str) {
    char *end;

    // Remove espaços à esquerda
    while (isspace((unsigned char)*str)) str++;
    if (*str == 0) return str; // string vazia

    // Remove espaços à direita
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    *(end + 1) = '\0';
    return str;
}

// Função principal para carregar perguntas de um CSV
Pergunta* carregaPerguntasDeCSV(const char *nome_arquivo, int *total) {
    FILE *file = fopen(nome_arquivo, "r");
    if (!file) {
        printf("Erro ao abrir o arquivo %s\n", nome_arquivo);
        *total = 0;
        return NULL;
    }

    Pergunta *perguntas = NULL;
    char linha[1024];
    int capacidade = 10;
    int count = 0;

    perguntas = malloc(capacidade * sizeof(Pergunta));
    if (!perguntas) {
        printf("Erro ao alocar memória\n");
        fclose(file);
        *total = 0;
        return NULL;
    }

    // Ignora a primeira linha (cabeçalho)
    fgets(linha, sizeof(linha), file);

    while (fgets(linha, sizeof(linha), file)) {
        if (count >= capacidade) {
            capacidade *= 2;
            Pergunta *tmp = realloc(perguntas, capacidade * sizeof(Pergunta));
            if (!tmp) {
                printf("Erro ao realocar memória\n");
                fclose(file);
                free(perguntas);
                *total = count;
                return NULL;
            }
            perguntas = tmp;
        }

        char *token = strtok(linha, ";");
        if (!token) continue;

        // Enunciado
        char *enunciado = strdup(trim(token));

        // Alternativas A, B, C, D
        char *alternativas[4];
        for (int i = 0; i < 4; i++) {
            token = strtok(NULL, ";");
            alternativas[i] = token ? strdup(trim(token)) : strdup("");
        }

        // Letra da alternativa correta
        token = strtok(NULL, ";");
        char correta = (token && strlen(token) > 0) ? toupper(token[0]) : 'A';

        // Nível de dificuldade
        token = strtok(NULL, ";");
        int nivel = token ? atoi(token) : 1;

        // Preenche a struct
        Pergunta *p = &perguntas[count];
        p->enunciado = enunciado;

        char letras[] = {'A', 'B', 'C', 'D'};
        for (int i = 0; i < 4; i++) {
            p->alternativas[i].letra = letras[i];
            p->alternativas[i].texto = alternativas[i];
            converteMaiscula(p->alternativas[i].texto);
        }

        p->correta = correta;
        p->nivel = (Dificuldade) nivel;

        count++;
    }

    fclose(file);
    *total = count;
    return perguntas;
}
