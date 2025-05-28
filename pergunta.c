#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pergunta.h"

Pergunta* carregarCSV(const char *arquivo, int *total) {
    FILE *fp = fopen(arquivo, "r");
    if (!fp) {
        printf("Erro ao abrir %s\n", arquivo);
        return NULL;
    }

    Pergunta *lista = NULL;
    char linha[512];
    *total = 0;

    while (fgets(linha, sizeof(linha), fp)) {
        lista = realloc(lista, (*total + 1) * sizeof(Pergunta));
        Pergunta *p = &lista[*total];

        // Quebra a linha do CSV
        char *token = strtok(linha, ",");
        strncpy(p->enunciado, token, TAM_ENUNCIADO);

        for (int i = 0; i < 4; i++) {
            token = strtok(NULL, ",");
            strncpy(p->alternativas[i], token, TAM_ALTERNATIVA);
        }

        token = strtok(NULL, ",");
        p->correta = token[0];

        token = strtok(NULL, ",");
        p->nivel = atoi(token);

        (*total)++;
    }

    fclose(fp);
    return lista;
}

Pergunta* carregarBinario(const char *arquivo, int *total) {
    FILE *fp = fopen(arquivo, "rb");
    if (!fp) return NULL;

    fread(total, sizeof(int), 1, fp);
    Pergunta *lista = malloc(*total * sizeof(Pergunta));
    fread(lista, sizeof(Pergunta), *total, fp);
    fclose(fp);
    return lista;
}

void salvarBinario(const char *arquivo, Pergunta *lista, int total) {
    FILE *fp = fopen(arquivo, "wb");
    if (!fp) {
        printf("Erro ao salvar em %s\n", arquivo);
        return;
    }
    fwrite(&total, sizeof(int), 1, fp);
    fwrite(lista, sizeof(Pergunta), total, fp);
    fclose(fp);
}


Pergunta* inserirPergunta(Pergunta *lista, int *total) {
    lista = realloc(lista, (*total + 1) * sizeof(Pergunta));
    Pergunta *p = &lista[*total];

    printf("Enunciado: ");
    getchar(); // limpa o buffer
    fgets(p->enunciado, TAM_ENUNCIADO, stdin);
    p->enunciado[strcspn(p->enunciado, "\n")] = 0;

    for (int i = 0; i < 4; i++) {
        printf("Alternativa %c: ", 'A'+i);
        fgets(p->alternativas[i], TAM_ALTERNATIVA, stdin);
        p->alternativas[i][strcspn(p->alternativas[i], "\n")] = 0;
    }

    printf("Alternativa correta (A/B/C/D): ");
    scanf(" %c", &p->correta);

    printf("Dificuldade (1 a 5): ");
    scanf("%d", &p->nivel);

    (*total)++;
    return lista;
}

void listarPerguntas(Pergunta *lista, int total) {
    for (int i = 0; i < total; i++) {
        printf("\n[%d] %s\n", i+1, lista[i].enunciado);
        for (int j = 0; j < 4; j++) {
            printf("  %c) %s\n", 'A'+j, lista[i].alternativas[j]);
        }
        printf("Correta: %c | Dificuldade: %d\n", lista[i].correta, lista[i].nivel);
    }
}

void pesquisarPorNivel(Pergunta *lista, int total, int nivel) {
    for (int i = 0; i < total; i++) {
        if (lista[i].nivel == nivel) {
            printf("\n[%d] %s\n", i+1, lista[i].enunciado);
            for (int j = 0; j < 4; j++) {
                printf("  %c) %s\n", 'A'+j, lista[i].alternativas[j]);
            }
            printf("Correta: %c\n", lista[i].correta);
        }
    }
}

void alterarPergunta(Pergunta *lista, int total) {
    int index;
    listarPerguntas(lista, total);
    printf("Digite o número da pergunta a alterar: ");
    scanf("%d", &index);
    index--;

    if (index < 0 || index >= total) {
        printf("Índice inválido.\n");
        return;
    }

    printf("Novo enunciado: ");
    getchar();
    fgets(lista[index].enunciado, TAM_ENUNCIADO, stdin);
    lista[index].enunciado[strcspn(lista[index].enunciado, "\n")] = 0;

    for (int i = 0; i < 4; i++) {
        printf("Nova alternativa %c: ", 'A'+i);
        fgets(lista[index].alternativas[i], TAM_ALTERNATIVA, stdin);
        lista[index].alternativas[i][strcspn(lista[index].alternativas[i], "\n")] = 0;
    }

    printf("Nova correta (A/B/C/D): ");
    scanf(" %c", &lista[index].correta);
    printf("Nova dificuldade: ");
    scanf("%d", &lista[index].nivel);
}

Pergunta* excluirPergunta(Pergunta *lista, int *total) {
    int index;
    listarPerguntas(lista, *total);
    printf("Digite o número da pergunta a excluir: ");
    scanf("%d", &index);
    index--;

    if (index < 0 || index >= *total) {
        printf("Índice inválido.\n");
        return lista;
    }

    for (int i = index; i < *total - 1; i++) {
        lista[i] = lista[i+1];
    }

    (*total)--;
    lista = realloc(lista, (*total) * sizeof(Pergunta));
    return lista;
}

void exportarCSV(const char *arquivo, Pergunta *lista, int total) {
    FILE *fp = fopen(arquivo, "w");
    if (!fp) {
        printf("Erro ao exportar para %s\n", arquivo);
        return;
    }

    for (int i = 0; i < total; i++) {
        fprintf(fp, "%s,%s,%s,%s,%s,%c,%d\n",
            lista[i].enunciado,
            lista[i].alternativas[0],
            lista[i].alternativas[1],
            lista[i].alternativas[2],
            lista[i].alternativas[3],
            lista[i].correta,
            lista[i].nivel
        );
    }

    fclose(fp);
    printf("Exportado com sucesso!\n");
}
