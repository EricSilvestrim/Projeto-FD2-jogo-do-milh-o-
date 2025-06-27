// /home/ubuntu/src/main.c
#include "perguntas.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h> // Para srand
#include <errno.h> // Incluído para usar errno e ENOENT

// --- Implementação das Funções --- 

// Função para limpar o buffer de entrada (evita problemas com scanf)
void limparBufferEntrada() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Função para criar e inicializar o banco de perguntas
BancoDePerguntas* criarBancoDePerguntas() {
    BancoDePerguntas *banco = (BancoDePerguntas*) malloc(sizeof(BancoDePerguntas));
    if (banco == NULL) {
        perror("Erro ao alocar memória para o banco de perguntas");
        return NULL;
    }
    
    // Aloca memória inicial para as perguntas
    banco->perguntas = (Pergunta*) malloc(MAX_PERGUNTAS_INICIAL * sizeof(Pergunta));
    if (banco->perguntas == NULL) {
        perror("Erro ao alocar memória para o vetor de perguntas");
        free(banco);
        return NULL;
    }
    
    banco->quantidadeAtual = 0;
    banco->capacidade = MAX_PERGUNTAS_INICIAL;
    // printf("Banco de perguntas inicializado com capacidade para %d perguntas.\n", banco->capacidade); // Comentado para testes
    return banco;
}

// Função para liberar a memória alocada para o banco de perguntas
void liberarBancoDePerguntas(BancoDePerguntas *banco) {
    if (banco != NULL) {
        if (banco->perguntas != NULL) {
            free(banco->perguntas);
            banco->perguntas = NULL; // Boa prática: anular ponteiro após free
        }
        free(banco);
        banco = NULL; // Boa prática
        // printf("Memória do banco de perguntas liberada.\n"); // Comentado para testes
    }
}

// Função para exibir o menu principal
void exibirMenuPrincipal() {
    printf("\n=====================================\n");
    printf("   Jogo de Perguntas e Respostas   ");
    printf("\n=====================================\n");
    printf("1. Jogar\n");
    printf("2. Cadastrar Pergunta\n");
    printf("3. Listar Perguntas (por Nível)\n");
    printf("4. Listar Todas as Perguntas\n");
    printf("5. Pesquisar Pergunta (por Enunciado)\n");
    printf("6. Alterar Pergunta (por ID)\n");
    printf("7. Excluir Pergunta (por ID)\n");
    printf("8. Exportar Perguntas para CSV\n");
    printf("9. Salvar e Sair\n");
    printf("0. Sair sem Salvar\n");
    printf("-------------------------------------\n");
    printf("Escolha uma opção: ");
}

// Função para obter a opção do menu do usuário com validação
int obterOpcaoMenu() {
    int opcao;
    // Loop até que uma entrada numérica válida seja fornecida
    while (scanf("%d", &opcao) != 1) {
        printf("Entrada inválida. Por favor, digite um número entre 0 e 9: ");
        limparBufferEntrada(); // Limpa o buffer se a entrada não for um número
    }
    limparBufferEntrada(); // Limpa o buffer após ler o número com sucesso
    return opcao;
}

#ifndef TESTE
// Função principal (só é compilada se não estiver em modo TESTE)
int main() {
    BancoDePerguntas *banco = NULL;
    int opcao;
    // int dadosCarregados = 0; // Removido - não utilizado
    const char *arquivoCSVInicial = "/home/ubuntu/perguntas_iniciais.csv"; //Para rodar no Linux
    const char *arquivoCSVInicial = "C:\\Users\\danie\\Downloads\\perguntas_iniciais.csv"; //Para rodar no Windows. Mexam só para onde está o arquivo do csv

    // Inicializa o gerador de números aleatórios para o jogo
    srand(time(NULL));

    // Cria o banco de perguntas
    banco = criarBancoDePerguntas();
    if (banco == NULL) {
        fprintf(stderr, "Falha crítica: Não foi possível inicializar o banco de perguntas.\n");
        return 1; // Encerra se não conseguiu alocar memória
    }

    // --- Lógica de Carregamento Inicial ---
    printf("\n--- Carregamento Inicial ---\n");
    // Tenta carregar do arquivo binário primeiro
    printf("Tentando carregar dados do arquivo binário (%s)...\n", arquivoBinario);
    if (carregarPerguntasBinario(banco, arquivoBinario) == 0) { // Código 0 indica sucesso
        printf("Dados carregados com sucesso do arquivo binário.\n");
        // dadosCarregados = 1;
    } else {
        // Se falhar ou não existir, tenta carregar do CSV inicial
        printf("Arquivo binário não encontrado ou inválido. Tentando carregar do CSV inicial (%s)...\n", arquivoCSVInicial);
        if (carregarPerguntasCSV(banco, arquivoCSVInicial) == 0) { // Código 0 indica sucesso
            printf("Dados carregados com sucesso do arquivo CSV inicial.\n");
            printf("Bem-vindo! Parece ser a primeira execução ou o arquivo de dados estava ausente.\n");
            // dadosCarregados = 1;
        } else {
            // Se ambos falharem
            printf("Falha ao carregar dados do CSV inicial. Iniciando com banco de perguntas vazio.\n");
            // O banco já está vazio por padrão, não precisa fazer nada extra
        }
    }
    printf("----------------------------\n");

    // --- Loop Principal do Menu ---
    do {
        exibirMenuPrincipal();
        opcao = obterOpcaoMenu();

        switch (opcao) {
            case 1: // Jogar
                iniciarJogo(banco);
                break;
            case 2: { // Cadastrar Pergunta
                Pergunta novaP;
                // Chama a função auxiliar para obter os dados do usuário
                if (obterDadosPergunta(&novaP, 0)) { // 0 indica que é cadastro, não alteração
                    adicionarPergunta(banco, novaP);
                } else {
                    printf("Cadastro cancelado ou dados inválidos.\n");
                }
                break;
            }
            case 3: { // Listar Perguntas (por Nível)
                int nivel;
                printf("Digite o nível para listar (1-5): ");
                // Valida a entrada do nível
                while (scanf("%d", &nivel) != 1 || nivel < 1 || nivel > 5) {
                     printf("Nível inválido. Digite um número entre 1 e 5: ");
                     limparBufferEntrada();
                }
                limparBufferEntrada();
                listarPerguntasPorNivel(banco, nivel);
                break;
            }
            case 4: // Listar Todas as Perguntas
                 listarTodasPerguntas(banco);
                break;
            case 5: { // Pesquisar Pergunta
                 char termoBusca[MAX_ENUNCIADO];
                 printf("Digite parte do enunciado para buscar: ");
                 // Usa fgets para ler a linha toda, prevenindo buffer overflow
                 if (fgets(termoBusca, MAX_ENUNCIADO, stdin) != NULL) {
                    termoBusca[strcspn(termoBusca, "\n")] = 0; // Remove a quebra de linha
                    buscarPerguntaPorEnunciado(banco, termoBusca);
                 } else {
                    printf("Erro ao ler termo de busca.\n");
                 }
                break;
            }
            case 6: { // Alterar Pergunta
                 int idAlterar;
                 printf("Digite o ID da pergunta a alterar: ");
                 if (scanf("%d", &idAlterar) == 1) {
                    limparBufferEntrada();
                    alterarPergunta(banco, idAlterar); // Chamada corrigida
                 } else {
                    limparBufferEntrada();
                    printf("ID inválido.\n");
                 }
                break;
            }
            case 7: { // Excluir Pergunta
                 int idExcluir;
                 printf("Digite o ID da pergunta a excluir: ");
                 if (scanf("%d", &idExcluir) == 1) {
                    limparBufferEntrada();
                    excluirPergunta(banco, idExcluir);
                 } else {
                    limparBufferEntrada();
                    printf("ID inválido.\n");
                 }
                break;
            }
            case 8: { // Exportar para CSV
                 const char *arquivoExport = "/home/ubuntu/export/perguntas_exportadas.csv";
                 if (exportarPerguntasCSV(banco, arquivoExport) == 0) {
                     printf("Perguntas exportadas com sucesso para %s\n", arquivoExport);
                 } else {
                     printf("Erro ao exportar perguntas para CSV.\n");
                 }
                break;
            }
            case 9: // Salvar e Sair
                printf("Salvando dados antes de sair...\n");
                if (salvarPerguntasBinario(banco, arquivoBinario) == 0) {
                    printf("Dados salvos com sucesso em %s\n", arquivoBinario);
                } else {
                    // Pergunta ao usuário se deseja sair mesmo assim?
                    fprintf(stderr, "Erro ao salvar dados no arquivo binário!\n");
                    printf("Deseja sair mesmo assim sem salvar? (s/n): ");
                    char confirma;
                    scanf(" %c", &confirma);
                    limparBufferEntrada();
                    if (tolower(confirma) != 's') {
                        opcao = -1; // Força a continuação do loop
                        printf("Retornando ao menu.\n");
                    }
                }
                // Se salvou com sucesso ou confirmou sair sem salvar, o loop termina
                if (opcao == 9) {
                     printf("Saindo do programa...\n");
                }
                break;
            case 0: // Sair sem Salvar
                printf("Tem certeza que deseja sair SEM SALVAR as alterações? (s/n): ");
                char confirmaSair;
                scanf(" %c", &confirmaSair);
                limparBufferEntrada();
                if (tolower(confirmaSair) == 's') {
                    printf("Saindo sem salvar...\n");
                } else {
                    opcao = -1; // Força a continuação do loop
                    printf("Retornando ao menu.\n");
                }
                break;
            default:
                printf("Opção inválida. Tente novamente.\n");
        }

        // Pausa para o usuário ler a saída, exceto ao sair
        if (opcao != 9 && opcao != 0) {
             printf("\nPressione Enter para continuar...");
             getchar(); // Espera o Enter
        }

    } while (opcao != 9 && opcao != 0);

    // Libera a memória alocada antes de finalizar
    liberarBancoDePerguntas(banco);
    printf("Programa finalizado.\n");

    return 0;
}
#endif // TESTE


// --- Implementação das Funções (Continuação) ---

// Função auxiliar para encontrar o próximo ID disponível (sequencial simples)
int proximoIdDisponivel(const BancoDePerguntas *banco) {
    if (banco->quantidadeAtual == 0) {
        return 1; // Começa do ID 1 se o banco estiver vazio
    }
    // Retorna o ID da última pergunta + 1. Assume que IDs não são reutilizados.
    return banco->perguntas[banco->quantidadeAtual - 1].id + 1;
}

// Função para validar os dados de uma pergunta (campos básicos)
int validarPergunta(const Pergunta *p) {
    // Verifica se os campos de texto não estão vazios e dentro dos limites
    if (p == NULL) return 0;
    if (strlen(p->enunciado) == 0 || strlen(p->enunciado) >= MAX_ENUNCIADO) return 0;
    if (strlen(p->alternativaA) == 0 || strlen(p->alternativaA) >= MAX_ALTERNATIVAS) return 0;
    if (strlen(p->alternativaB) == 0 || strlen(p->alternativaB) >= MAX_ALTERNATIVAS) return 0;
    if (strlen(p->alternativaC) == 0 || strlen(p->alternativaC) >= MAX_ALTERNATIVAS) return 0;
    if (strlen(p->alternativaD) == 0 || strlen(p->alternativaD) >= MAX_ALTERNATIVAS) return 0;
    // Verifica se a resposta correta é válida
    if (p->respostaCorreta != 'A' && p->respostaCorreta != 'B' && p->respostaCorreta != 'C' && p->respostaCorreta != 'D') return 0;
    // Verifica se a dificuldade está no intervalo correto
    if (p->dificuldade < 1 || p->dificuldade > 5) return 0;
    return 1; // Todos os critérios passaram
}

// Função para adicionar uma pergunta ao banco (com redimensionamento dinâmico)
int adicionarPergunta(BancoDePerguntas *banco, Pergunta novaPergunta) {
    // 1. Validar dados da nova pergunta (exceto ID, que será gerado)
    if (!validarPergunta(&novaPergunta)) {
        // fprintf(stderr, "Erro: Dados da pergunta inválidos (campos vazios ou fora do padrão?). Cadastro cancelado.\n"); // Comentado para testes
        return -1; // Código de erro para dados inválidos
    }

    // 2. Verificar duplicata por enunciado (comparação simples)
    for (int i = 0; i < banco->quantidadeAtual; i++) {
        if (strcmp(banco->perguntas[i].enunciado, novaPergunta.enunciado) == 0) {
            // fprintf(stderr, "Erro: Pergunta com este enunciado já existe (ID: %d). Cadastro cancelado.\n", banco->perguntas[i].id); // Comentado para testes
            return -2; // Código de erro para duplicata
        }
    }

    // 3. Verificar se precisa redimensionar o vetor (realocação)
    if (banco->quantidadeAtual >= banco->capacidade) {
        int novaCapacidade = banco->capacidade * 2; // Estratégia: dobrar a capacidade
        // printf("Atingiu a capacidade máxima (%d). Tentando redimensionar para %d...\n", banco->capacidade, novaCapacidade); // Comentado para testes
        Pergunta *temp = (Pergunta*) realloc(banco->perguntas, novaCapacidade * sizeof(Pergunta));
        if (temp == NULL) {
            perror("Erro crítico ao redimensionar o vetor de perguntas");
            // Não retorna erro fatal, mas avisa que não pôde adicionar
            fprintf(stderr, "Não foi possível adicionar a nova pergunta por falta de memória.\n");
            return -3; // Código de erro para falha de memória na realocação
        }
        banco->perguntas = temp; // Atualiza o ponteiro para o novo bloco de memória
        banco->capacidade = novaCapacidade; // Atualiza a capacidade
        // printf("Capacidade do banco aumentada para %d perguntas.\n", novaCapacidade); // Comentado para testes
    }

    // 4. Atribuir ID único à nova pergunta
    novaPergunta.id = proximoIdDisponivel(banco);

    // 5. Adicionar a nova pergunta no final do vetor
    banco->perguntas[banco->quantidadeAtual] = novaPergunta;
    banco->quantidadeAtual++; // Incrementa o contador de perguntas

    // printf("Pergunta ID %d adicionada com sucesso.\n", novaPergunta.id); // Comentado para testes
    return 0; // Sucesso
}

// Função para carregar perguntas de um arquivo CSV
// Formato esperado: Enunciado,AltA,AltB,AltC,AltD,Resposta,Dificuldade
int carregarPerguntasCSV(BancoDePerguntas *banco, const char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "r");
    if (arquivo == NULL) {
        // Não imprime erro se o arquivo não existe, pois pode ser a primeira execução
        if (errno != ENOENT) { // ENOENT = No such file or directory
             perror("Erro ao tentar abrir o arquivo CSV");
        }
        return -1; // Indica que não foi possível abrir (pode não existir)
    }

    char linha[1024]; // Buffer para ler cada linha (tamanho generoso)
    int perguntasCarregadas = 0;
    int linhaNum = 0;

    // Opcional: Ler e ignorar cabeçalho (se existir)
    // fgets(linha, sizeof(linha), arquivo);
    // linhaNum++;

    // printf("Lendo perguntas do arquivo CSV: %s\n", nomeArquivo); // Comentado para testes
    while (fgets(linha, sizeof(linha), arquivo)) {
        linhaNum++;
        // Remove a quebra de linha do final, se houver
        linha[strcspn(linha, "\r\n")] = 0;

        // Ignora linhas vazias ou comentadas (ex: começando com #)
        if (strlen(linha) == 0 || linha[0] == '#') {
            continue;
        }

        Pergunta p;
        char *token;
        int campo = 0;
        char *ptr = linha; // Ponteiro para percorrer a linha

        // Inicializa a pergunta para evitar dados lixo
        memset(&p, 0, sizeof(Pergunta));

        // Usa strtok para separar os campos por vírgula
        // Cuidado: strtok modifica a string original!
        token = strtok(ptr, ";");
        while (token != NULL && campo < 7) {
            // Remove espaços em branco no início/fim do token (trim)
            while (isspace((unsigned char)*token)) token++;
            char *end = token + strlen(token) - 1;
            while (end > token && isspace((unsigned char)*end)) end--;
            *(end + 1) = 0;

            // Atribui o token ao campo correspondente da struct
            switch (campo) {
                case 0: strncpy(p.enunciado, token, MAX_ENUNCIADO - 1); p.enunciado[MAX_ENUNCIADO - 1] = '\0'; break;
                case 1: strncpy(p.alternativaA, token, MAX_ALTERNATIVAS - 1); p.alternativaA[MAX_ALTERNATIVAS - 1] = '\0'; break;
                case 2: strncpy(p.alternativaB, token, MAX_ALTERNATIVAS - 1); p.alternativaB[MAX_ALTERNATIVAS - 1] = '\0'; break;
                case 3: strncpy(p.alternativaC, token, MAX_ALTERNATIVAS - 1); p.alternativaC[MAX_ALTERNATIVAS - 1] = '\0'; break;
                case 4: strncpy(p.alternativaD, token, MAX_ALTERNATIVAS - 1); p.alternativaD[MAX_ALTERNATIVAS - 1] = '\0'; break;
                case 5: p.respostaCorreta = toupper(token[0]); break; // Pega só o primeiro caractere
                case 6: p.dificuldade = atoi(token); break;
            }
            token = strtok(NULL, ";"); // Pega o próximo token
            campo++;
        }

        // Verifica se a linha tinha o número correto de campos
        if (campo != 7) {
            fprintf(stderr, "Aviso: Linha %d do CSV está mal formatada (esperados 7 campos, encontrados %d). Pulando linha: [%s]\n", linhaNum, campo, linha);
            continue; // Pula para a próxima linha
        }

        // Adiciona a pergunta lida ao banco (a função adicionarPergunta já valida e atribui ID)
        int resultadoAdd = adicionarPergunta(banco, p);
        if (resultadoAdd == 0) {
            perguntasCarregadas++;
        } else {
            fprintf(stderr, "Erro ao adicionar pergunta da linha %d do CSV (código %d).\n", linhaNum, resultadoAdd);
            // Pode decidir parar ou continuar dependendo da política de erro
        }
    }

    fclose(arquivo);
    // printf("%d perguntas carregadas com sucesso do arquivo %s.\n", perguntasCarregadas, nomeArquivo); // Comentado para testes
    return 0; // Retorna sucesso geral (mesmo que algumas linhas tenham falhado)
}


// Função para salvar as perguntas em um arquivo binário
int salvarPerguntasBinario(const BancoDePerguntas *banco, const char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "wb"); // Abre para escrita binária (write binary)
    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo binário para escrita");
        return -1;
    }

    // 1. Escreve a quantidade atual de perguntas no início do arquivo
    if (fwrite(&(banco->quantidadeAtual), sizeof(int), 1, arquivo) != 1) {
        perror("Erro ao escrever a quantidade de perguntas no arquivo binário");
        fclose(arquivo);
        return -2;
    }

    // 2. Escreve o array de perguntas (se houver alguma)
    if (banco->quantidadeAtual > 0) {
        size_t escritos = fwrite(banco->perguntas, sizeof(Pergunta), banco->quantidadeAtual, arquivo);
        if (escritos != banco->quantidadeAtual) {
            perror("Erro ao escrever as perguntas no arquivo binário");
            fprintf(stderr, "Erro: Esperado escrever %d perguntas, mas fwrite retornou %zu.\n", banco->quantidadeAtual, escritos);
            fclose(arquivo);
            return -3;
        }
    }

    fclose(arquivo);
    // printf("%d perguntas salvas com sucesso no arquivo binário %s.\n", banco->quantidadeAtual, nomeArquivo); // Comentado para testes
    return 0; // Sucesso
}

// Função para carregar as perguntas de um arquivo binário
int carregarPerguntasBinario(BancoDePerguntas *banco, const char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "rb"); // Abre para leitura binária (read binary)
    if (arquivo == NULL) {
        // Se o arquivo não existe, não é um erro fatal, apenas informa.
        if (errno != ENOENT) { // Se for outro erro além de 'não encontrado'
            perror("Erro ao tentar abrir o arquivo binário para leitura");
        }
        return -1; // Indica que o arquivo não foi aberto (não encontrado ou erro)
    }

    // 1. Lê a quantidade de perguntas que foram salvas no arquivo
    int quantidadeNoArquivo;
    if (fread(&quantidadeNoArquivo, sizeof(int), 1, arquivo) != 1) {
        // Verifica se chegou ao fim do arquivo (EOF) - arquivo vazio ou corrompido?
        if (feof(arquivo)) {
            // fprintf(stderr, "Erro: Arquivo binário parece estar vazio ou corrompido (não contém a quantidade).\n"); // Comentado para testes
        } else {
            perror("Erro ao ler a quantidade de perguntas do arquivo binário");
        }
        fclose(arquivo);
        return -2;
    }

    // Verifica se a quantidade lida é válida (não negativa)
    if (quantidadeNoArquivo < 0) {
        fprintf(stderr, "Erro: Quantidade de perguntas inválida (%d) encontrada no arquivo binário.\n", quantidadeNoArquivo);
        fclose(arquivo);
        return -3;
    }
    
    // Se o arquivo indica 0 perguntas, apenas atualiza o banco e retorna sucesso.
    if (quantidadeNoArquivo == 0) {
        // printf("Arquivo binário lido com sucesso, mas não contém perguntas.\n"); // Comentado para testes
        banco->quantidadeAtual = 0; // Garante que o banco está vazio
        // Não precisa redimensionar ou ler mais nada
        fclose(arquivo);
        return 0; // Sucesso, 0 perguntas carregadas
    }

    // 2. Garante que há espaço suficiente no vetor dinâmico
    if (quantidadeNoArquivo > banco->capacidade) {
        int novaCapacidade = quantidadeNoArquivo + (MAX_PERGUNTAS_INICIAL / 2); // Aloca espaço extra
        // printf("Redimensionando banco para carregar %d perguntas (nova capacidade: %d)...\n", quantidadeNoArquivo, novaCapacidade); // Comentado para testes
        Pergunta *temp = (Pergunta*) realloc(banco->perguntas, novaCapacidade * sizeof(Pergunta));
        if (temp == NULL) {
            perror("Erro crítico ao redimensionar o vetor para carregar do binário");
            fprintf(stderr, "Não foi possível carregar as perguntas por falta de memória.\n");
            fclose(arquivo);
            return -4; // Erro de memória
        }
        banco->perguntas = temp;
        banco->capacidade = novaCapacidade;
    }

    // 3. Lê o bloco de perguntas do arquivo para o vetor
    size_t lidos = fread(banco->perguntas, sizeof(Pergunta), quantidadeNoArquivo, arquivo);
    if (lidos != quantidadeNoArquivo) {
        perror("Erro ao ler as perguntas do arquivo binário");
        fprintf(stderr, "Erro: Esperado ler %d perguntas, mas fread retornou %zu.\n", quantidadeNoArquivo, lidos);
        // Decide o que fazer: carregar parcialmente ou falhar? Por segurança, falha.
        banco->quantidadeAtual = 0; // Reseta a quantidade para evitar dados inconsistentes
        fclose(arquivo);
        return -5; // Erro na leitura dos dados
    }

    // Se chegou aqui, a leitura foi bem-sucedida
    banco->quantidadeAtual = quantidadeNoArquivo; // Atualiza a quantidade no banco
    fclose(arquivo);
    // printf("%d perguntas carregadas com sucesso do arquivo binário %s.\n", banco->quantidadeAtual, nomeArquivo); // Comentado para testes
    return 0; // Sucesso
}


// Função para exportar as perguntas para um arquivo CSV
int exportarPerguntasCSV(const BancoDePerguntas *banco, const char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "w"); // Abre para escrita (write), sobrescreve se existir
    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo CSV para exportação");
        return -1;
    }

    // Escreve o cabeçalho (bom para compatibilidade)
    fprintf(arquivo, "Enunciado,AlternativaA,AlternativaB,AlternativaC,AlternativaD,RespostaCorreta,Dificuldade\n");

    // Escreve cada pergunta no formato CSV
    // printf("Exportando %d perguntas para %s...\n", banco->quantidadeAtual, nomeArquivo); // Comentado para testes
    for (int i = 0; i < banco->quantidadeAtual; i++) {
        Pergunta p = banco->perguntas[i];
        // Nota: Esta implementação simples não trata vírgulas ou aspas dentro dos campos.
        // Uma exportação CSV robusta precisaria verificar e colocar aspas "" ao redor
        // dos campos que contêm vírgulas, aspas ou quebras de linha, e duplicar aspas internas.
        fprintf(arquivo, "%s,%s,%s,%s,%s,%c,%d\n",
                p.enunciado,       // Assume que não tem vírgulas
                p.alternativaA,    // Assume que não tem vírgulas
                p.alternativaB,    // Assume que não tem vírgulas
                p.alternativaC,    // Assume que não tem vírgulas
                p.alternativaD,    // Assume que não tem vírgulas
                p.respostaCorreta, // Caractere único, sem problemas
                p.dificuldade);    // Inteiro, sem problemas
    }

    fclose(arquivo);
    // printf("Exportação concluída com sucesso.\n"); // Comentado para testes
    return 0; // Sucesso
}


// Função para listar todas as perguntas cadastradas
void listarTodasPerguntas(const BancoDePerguntas *banco) {
    printf("\n--- Listando Todas as Perguntas (%d) ---\n", banco->quantidadeAtual);
    if (banco->quantidadeAtual == 0) {
        printf("Nenhuma pergunta cadastrada no momento.\n");
        return;
    }
    // Itera sobre todas as perguntas e imprime seus detalhes
    for (int i = 0; i < banco->quantidadeAtual; i++) {
        Pergunta p = banco->perguntas[i];
        printf("ID: %-4d | Nível: %d | Resposta: %c\n", p.id, p.dificuldade, p.respostaCorreta);
        printf("  Enunciado: %s\n", p.enunciado);
        printf("    A) %s\n", p.alternativaA);
        printf("    B) %s\n", p.alternativaB);
        printf("    C) %s\n", p.alternativaC);
        printf("    D) %s\n", p.alternativaD);
        printf("----------------------------------------\n");
    }
}

// Função para listar perguntas filtradas por nível de dificuldade
void listarPerguntasPorNivel(const BancoDePerguntas *banco, int nivel) {
    printf("\n--- Listando Perguntas Nível %d ---\n", nivel);
    int encontradas = 0;
    // Validação do nível já feita na chamada do menu
    // Itera sobre as perguntas, imprimindo apenas as do nível especificado
    for (int i = 0; i < banco->quantidadeAtual; i++) {
        if (banco->perguntas[i].dificuldade == nivel) {
            Pergunta p = banco->perguntas[i];
            printf("ID: %-4d | Resposta: %c\n", p.id, p.respostaCorreta);
            printf("  Enunciado: %s\n", p.enunciado);
            printf("    A) %s\n", p.alternativaA);
            printf("    B) %s\n", p.alternativaB);
            printf("    C) %s\n", p.alternativaC);
            printf("    D) %s\n", p.alternativaD);
            printf("----------------------------------------\n");
            encontradas++;
        }
    }
    // Informa se nenhuma pergunta foi encontrada para o nível
    if (encontradas == 0) {
        printf("Nenhuma pergunta encontrada para o nível %d.\n", nivel);
    }
}

// Função para buscar uma pergunta pelo seu ID único
// Retorna um ponteiro para a pergunta encontrada, ou NULL se não encontrar.
Pergunta* buscarPerguntaPorId(const BancoDePerguntas *banco, int id) {
    for (int i = 0; i < banco->quantidadeAtual; i++) {
        if (banco->perguntas[i].id == id) {
            return &(banco->perguntas[i]); // Retorna o endereço da struct no vetor
        }
    }
    return NULL; // ID não encontrado
}

// Função para buscar perguntas que contenham um termo no enunciado (case-insensitive)
// Imprime a primeira encontrada e retorna um ponteiro para ela, ou NULL.
Pergunta* buscarPerguntaPorEnunciado(const BancoDePerguntas *banco, const char *enunciadoParcial) {
    // Verifica se o termo de busca é válido
    if (enunciadoParcial == NULL || strlen(enunciadoParcial) == 0) {
        printf("Termo de busca inválido.\n");
        return NULL;
    }

    printf("\n--- Buscando por \"%s\" no enunciado ---\n", enunciadoParcial);
    char enunciadoLower[MAX_ENUNCIADO];
    char parcialLower[MAX_ENUNCIADO];
    int encontradas = 0;
    
    // Converte o termo de busca para minúsculas para comparação case-insensitive
    int lenParcial = strlen(enunciadoParcial);
    for(int i = 0; i < lenParcial; i++){
        parcialLower[i] = tolower((unsigned char)enunciadoParcial[i]);
    }
    parcialLower[lenParcial] = '\0'; // Termina a string

    // Itera sobre as perguntas
    for (int i = 0; i < banco->quantidadeAtual; i++) {
        // Converte o enunciado da pergunta atual para minúsculas
        int lenEnunciado = strlen(banco->perguntas[i].enunciado);
        for(int j = 0; j < lenEnunciado; j++){
            enunciadoLower[j] = tolower((unsigned char)banco->perguntas[i].enunciado[j]);
        }
        enunciadoLower[lenEnunciado] = '\0';
        
        // Usa strstr para verificar se o enunciado (minúsculo) contém o termo (minúsculo)
        if (strstr(enunciadoLower, parcialLower) != NULL) {
             encontradas++;
             // Imprime os detalhes da pergunta encontrada
             printf("--- Pergunta Encontrada (#%d) ---\n", encontradas);
             Pergunta p = banco->perguntas[i];
             printf("ID: %-4d | Nível: %d | Resposta: %c\n", p.id, p.dificuldade, p.respostaCorreta);
             printf("  Enunciado: %s\n", p.enunciado);
             printf("    A) %s\n", p.alternativaA);
             printf("    B) %s\n", p.alternativaB);
             printf("    C) %s\n", p.alternativaC);
             printf("    D) %s\n", p.alternativaD);
             printf("----------------------------------\n");
             // Retorna ponteiro para a primeira encontrada (simplificação)
             // Uma versão melhor poderia listar todas ou pedir para escolher
             if (encontradas == 1) {
                 // return &(banco->perguntas[i]); // Descomentar se quiser retornar só a primeira
             }
        }
    }

    if (encontradas == 0) {
        printf("Nenhuma pergunta encontrada contendo \"%s\".\n", enunciadoParcial);
        return NULL; // Nenhuma encontrada
    } else {
        printf("%d pergunta(s) encontrada(s).\n", encontradas);
        // Retorna NULL mesmo se encontrou mais de uma, pois a função só imprime
        // Poderia retornar o ponteiro da última encontrada se desejado.
        return NULL; 
    }
}


// Função auxiliar para obter os dados de uma pergunta do usuário (para cadastro/alteração)
// Retorna 1 se sucesso, 0 se o usuário cancelou ou erro.
int obterDadosPergunta(Pergunta *p, int paraAlteracao) {
    printf("\n--- %s Pergunta ---\n", paraAlteracao ? "Alterar Dados da" : "Cadastrar Nova");
    char buffer[MAX_ENUNCIADO]; // Buffer para leitura segura com fgets

    // Enunciado
    printf("Enunciado (max %d caracteres): ", MAX_ENUNCIADO - 1);
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) return 0; // Erro ou EOF
    buffer[strcspn(buffer, "\n")] = 0; // Remove newline
    if (strlen(buffer) == 0) { printf("Erro: Enunciado não pode ser vazio.\n"); return 0; }
    strncpy(p->enunciado, buffer, MAX_ENUNCIADO - 1);
    p->enunciado[MAX_ENUNCIADO - 1] = '\0'; // Garante terminação nula

    // Alternativa A
    printf("Alternativa A (max %d caracteres): ", MAX_ALTERNATIVAS - 1);
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) return 0;
    buffer[strcspn(buffer, "\n")] = 0;
    if (strlen(buffer) == 0) { printf("Erro: Alternativa A não pode ser vazia.\n"); return 0; }
    strncpy(p->alternativaA, buffer, MAX_ALTERNATIVAS - 1);
    p->alternativaA[MAX_ALTERNATIVAS - 1] = '\0';

    // Alternativa B
    printf("Alternativa B (max %d caracteres): ", MAX_ALTERNATIVAS - 1);
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) return 0;
    buffer[strcspn(buffer, "\n")] = 0;
    if (strlen(buffer) == 0) { printf("Erro: Alternativa B não pode ser vazia.\n"); return 0; }
    strncpy(p->alternativaB, buffer, MAX_ALTERNATIVAS - 1);
    p->alternativaB[MAX_ALTERNATIVAS - 1] = '\0';

    // Alternativa C
    printf("Alternativa C (max %d caracteres): ", MAX_ALTERNATIVAS - 1);
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) return 0;
    buffer[strcspn(buffer, "\n")] = 0;
    if (strlen(buffer) == 0) { printf("Erro: Alternativa C não pode ser vazia.\n"); return 0; }
    strncpy(p->alternativaC, buffer, MAX_ALTERNATIVAS - 1);
    p->alternativaC[MAX_ALTERNATIVAS - 1] = '\0';

    // Alternativa D
    printf("Alternativa D (max %d caracteres): ", MAX_ALTERNATIVAS - 1);
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) return 0;
    buffer[strcspn(buffer, "\n")] = 0;
    if (strlen(buffer) == 0) { printf("Erro: Alternativa D não pode ser vazia.\n"); return 0; }
    strncpy(p->alternativaD, buffer, MAX_ALTERNATIVAS - 1);
    p->alternativaD[MAX_ALTERNATIVAS - 1] = '\0';

    // Resposta Correta (valida entrada)
    char resp;
    do {
        printf("Resposta Correta (A, B, C ou D): ");
        if (scanf(" %c", &resp) != 1) { 
            limparBufferEntrada(); 
            printf("Erro de leitura. Tente novamente.\n");
            continue; 
        }
        resp = toupper(resp);
        limparBufferEntrada(); // Limpa o buffer após scanf
        if (resp != 'A' && resp != 'B' && resp != 'C' && resp != 'D') {
            printf("Erro: Resposta inválida. Use A, B, C ou D.\n");
        }
    } while (resp != 'A' && resp != 'B' && resp != 'C' && resp != 'D');
    p->respostaCorreta = resp;

    // Dificuldade (valida entrada)
    int dif;
    do {
        printf("Nível de Dificuldade (1 a 5): ");
        if (scanf("%d", &dif) != 1) { 
            limparBufferEntrada(); 
            printf("Erro: Entrada inválida para dificuldade. Digite um número.\n"); 
            continue; 
        }
        limparBufferEntrada(); // Limpa o buffer após scanf
        if (dif < 1 || dif > 5) {
            printf("Erro: Dificuldade inválida. Use um valor entre 1 e 5.\n");
        }
    } while (dif < 1 || dif > 5);
    p->dificuldade = dif;

    // ID não é pedido aqui, será atribuído pela função adicionarPergunta
    // ou mantido pela função alterarPergunta.
    // p->id = paraAlteracao ? p->id : 0; // Lógica movida para as funções chamadoras

    return 1; // Sucesso na obtenção dos dados
}


// Função para alterar uma pergunta existente no banco (Assinatura corrigida)
int alterarPergunta(BancoDePerguntas *banco, int id) {
    // 1. Encontra a pergunta original pelo ID
    Pergunta *perguntaOriginal = buscarPerguntaPorId(banco, id);
    if (perguntaOriginal == NULL) {
        printf("Erro: Pergunta com ID %d não encontrada para alteração.\n", id);
        return -1; // Código de erro: não encontrado
    }

    printf("\n--- Alterando Pergunta ID %d ---\n", id);
    printf("Dados atuais:\n");
    // Mostra os dados atuais para referência
    printf("  Nível: %d | Resposta: %c\n", perguntaOriginal->dificuldade, perguntaOriginal->respostaCorreta);
    printf("  Enunciado: %s\n", perguntaOriginal->enunciado);
    printf("    A) %s\n", perguntaOriginal->alternativaA);
    printf("    B) %s\n", perguntaOriginal->alternativaB);
    printf("    C) %s\n", perguntaOriginal->alternativaC);
    printf("    D) %s\n", perguntaOriginal->alternativaD);
    printf("------------------------------------\n");
    printf("Digite os novos dados. Pressione Enter para manter o valor atual (exceto para Resposta e Nível).\n");

    Pergunta dadosAtualizados; // Cria uma struct temporária para os novos dados
    dadosAtualizados.id = id; // Mantém o ID original
    char buffer[MAX_ENUNCIADO]; // Buffer para leitura

    // Pede cada campo, permitindo manter o original se o usuário teclar Enter
    // Enunciado
    printf("Novo Enunciado (atual: %.50s...): ", perguntaOriginal->enunciado);
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = 0;
    if (strlen(buffer) > 0) strncpy(dadosAtualizados.enunciado, buffer, MAX_ENUNCIADO - 1);
    else strncpy(dadosAtualizados.enunciado, perguntaOriginal->enunciado, MAX_ENUNCIADO - 1);
    dadosAtualizados.enunciado[MAX_ENUNCIADO - 1] = '\0';

    // Alternativa A
    printf("Nova Alternativa A (atual: %.50s...): ", perguntaOriginal->alternativaA);
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = 0;
    if (strlen(buffer) > 0) strncpy(dadosAtualizados.alternativaA, buffer, MAX_ALTERNATIVAS - 1);
    else strncpy(dadosAtualizados.alternativaA, perguntaOriginal->alternativaA, MAX_ALTERNATIVAS - 1);
    dadosAtualizados.alternativaA[MAX_ALTERNATIVAS - 1] = '\0';
    
    // Alternativa B
    printf("Nova Alternativa B (atual: %.50s...): ", perguntaOriginal->alternativaB);
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = 0;
    if (strlen(buffer) > 0) strncpy(dadosAtualizados.alternativaB, buffer, MAX_ALTERNATIVAS - 1);
    else strncpy(dadosAtualizados.alternativaB, perguntaOriginal->alternativaB, MAX_ALTERNATIVAS - 1);
    dadosAtualizados.alternativaB[MAX_ALTERNATIVAS - 1] = '\0';

    // Alternativa C
    printf("Nova Alternativa C (atual: %.50s...): ", perguntaOriginal->alternativaC);
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = 0;
    if (strlen(buffer) > 0) strncpy(dadosAtualizados.alternativaC, buffer, MAX_ALTERNATIVAS - 1);
    else strncpy(dadosAtualizados.alternativaC, perguntaOriginal->alternativaC, MAX_ALTERNATIVAS - 1);
    dadosAtualizados.alternativaC[MAX_ALTERNATIVAS - 1] = '\0';

    // Alternativa D
    printf("Nova Alternativa D (atual: %.50s...): ", perguntaOriginal->alternativaD);
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = 0;
    if (strlen(buffer) > 0) strncpy(dadosAtualizados.alternativaD, buffer, MAX_ALTERNATIVAS - 1);
    else strncpy(dadosAtualizados.alternativaD, perguntaOriginal->alternativaD, MAX_ALTERNATIVAS - 1);
    dadosAtualizados.alternativaD[MAX_ALTERNATIVAS - 1] = '\0';

    // Resposta Correta (obrigatório informar novo valor)
    char resp;
    do {
        printf("Nova Resposta Correta (A, B, C ou D) (atual: %c): ", perguntaOriginal->respostaCorreta);
        if (scanf(" %c", &resp) != 1) { 
            limparBufferEntrada(); 
            printf("Erro de leitura. Tente novamente.\n");
            continue; 
        }
        resp = toupper(resp);
        limparBufferEntrada();
        if (resp != 'A' && resp != 'B' && resp != 'C' && resp != 'D') {
            printf("Erro: Resposta inválida. Use A, B, C ou D.\n");
        }
    } while (resp != 'A' && resp != 'B' && resp != 'C' && resp != 'D');
    dadosAtualizados.respostaCorreta = resp;

    // Dificuldade (obrigatório informar novo valor)
    int dif;
    do {
        printf("Novo Nível de Dificuldade (1 a 5) (atual: %d): ", perguntaOriginal->dificuldade);
        if (scanf("%d", &dif) != 1) { 
            limparBufferEntrada(); 
            printf("Erro: Entrada inválida para dificuldade. Digite um número.\n"); 
            continue; 
        }
        limparBufferEntrada();
        if (dif < 1 || dif > 5) {
            printf("Erro: Dificuldade inválida. Use um valor entre 1 e 5.\n");
        }
    } while (dif < 1 || dif > 5);
    dadosAtualizados.dificuldade = dif;

    // 2. Validar os dados atualizados (campos não podem ficar vazios, etc.)
    if (!validarPergunta(&dadosAtualizados)) {
        printf("Erro: Os dados atualizados são inválidos (campos vazios ou fora do padrão?). Alteração cancelada.\n");
        return -2; // Código de erro: dados inválidos
    }

    // 3. Verificar duplicata de enunciado (ignorando a própria pergunta sendo alterada)
    for (int i = 0; i < banco->quantidadeAtual; i++) {
        // Compara com todas as outras perguntas
        if (banco->perguntas[i].id != id && strcmp(banco->perguntas[i].enunciado, dadosAtualizados.enunciado) == 0) {
            fprintf(stderr, "Erro: Já existe outra pergunta (ID: %d) com este enunciado. Alteração cancelada.\n", banco->perguntas[i].id);
            return -3; // Código de erro para duplicata
        }
    }

    // 4. Atualizar a pergunta no banco (sobrescreve a original)
    *perguntaOriginal = dadosAtualizados; // Copia a struct inteira

    printf("Pergunta ID %d alterada com sucesso.\n", id);
    return 0; // Sucesso
}

// Função para excluir uma pergunta do banco pelo ID
int excluirPergunta(BancoDePerguntas *banco, int id) {
    int indiceParaExcluir = -1;
    // 1. Encontra o índice da pergunta com o ID fornecido
    for (int i = 0; i < banco->quantidadeAtual; i++) {
        if (banco->perguntas[i].id == id) {
            indiceParaExcluir = i;
            break;
        }
    }

    // Se não encontrou o ID
    if (indiceParaExcluir == -1) {
        // printf("Erro: Pergunta com ID %d não encontrada para exclusão.\n", id); // Comentado para testes
        return -1; // Não encontrado
    }

    // 2. Confirmação do usuário (desabilitada em modo TESTE)
    #ifndef TESTE // Only ask for confirmation if not in test mode
    printf("Tem certeza que deseja excluir a pergunta ID %d?\n", id);
    printf("  Enunciado: %.60s...\n", banco->perguntas[indiceParaExcluir].enunciado); // Mostra parte do enunciado
    printf("Digite 's' para confirmar a exclusão: ");
    char confirmacao;
    scanf(" %c", &confirmacao);
    limparBufferEntrada();

    if (tolower(confirmacao) != 's') {
        printf("Exclusão cancelada pelo usuário.\n");
        return 1; // Cancelado pelo usuário
    }
    #endif // TESTE

    // 3. Desloca os elementos seguintes para cobrir o espaço da pergunta excluída
    // Move cada elemento a partir do índice+1 uma posição para trás
    for (int i = indiceParaExcluir; i < banco->quantidadeAtual - 1; i++) {
        banco->perguntas[i] = banco->perguntas[i + 1];
    }

    // 4. Decrementa a quantidade de perguntas
    banco->quantidadeAtual--;

    // printf("Pergunta ID %d excluída com sucesso.\n", id); // Comentado para testes

    // Opcional: Considerar reduzir o tamanho do array (realloc) se ficar muito vazio
    // if (banco->quantidadeAtual < banco->capacidade / 4 && banco->capacidade > MAX_PERGUNTAS_INICIAL) { ... }

    return 0; // Sucesso
}


// --- Lógica do Jogo ---

// Função para embaralhar um array de inteiros (Algoritmo Fisher-Yates)
void embaralharIndices(int *array, int n) {
    if (n > 1) {
        for (int i = n - 1; i > 0; i--) {
            // Escolhe um índice aleatório entre 0 e i (inclusive)
            int j = rand() % (i + 1);
            // Troca o elemento atual (i) com o elemento aleatório (j)
            int temp = array[i];
            array[i] = array[j];
            array[j] = temp;
        }
    }
}

// Função principal da lógica do jogo
void iniciarJogo(BancoDePerguntas *banco) {
    printf("\n=========================================\n");
    printf("   Bem-vindo ao Show das Perguntas!   ");
    printf("\n=========================================\n");
    printf("Responda 15 perguntas para ganhar R$ 1 Milhão!\n");
    printf("Marcos de segurança: Acertando a 5ª (R$ 5.000) e 10ª (R$ 50.000) perguntas.\n");
    printf("A qualquer momento, digite 'P' para parar e levar o prêmio acumulado.\n");
    printf("Pressione Enter para começar...");
    getchar(); // Pausa antes de iniciar

    // 1. Verificar se há perguntas suficientes para o jogo (3 por nível)
    int contagemPorNivel[5] = {0}; // Índices 0 a 4 para níveis 1 a 5
    for (int i = 0; i < banco->quantidadeAtual; i++) {
        if (banco->perguntas[i].dificuldade >= 1 && banco->perguntas[i].dificuldade <= 5) {
            contagemPorNivel[banco->perguntas[i].dificuldade - 1]++;
        }
    }

    int perguntasSuficientes = 1;
    for (int i = 0; i < 5; i++) {
        if (contagemPorNivel[i] < 3) { // Precisa de 3 perguntas por nível
            fprintf(stderr, "Erro: Não há perguntas suficientes de nível %d (necessário 3, encontrado %d).\n", i + 1, contagemPorNivel[i]);
            perguntasSuficientes = 0;
        }
    }

    if (!perguntasSuficientes) {
        printf("Não é possível iniciar o jogo devido à falta de perguntas em alguns níveis.\n");
        printf("Cadastre mais perguntas usando o menu principal.\n");
        return; // Retorna ao menu principal
    }

    // 2. Selecionar as 15 perguntas para o jogo (3 de cada nível, aleatoriamente)
    Pergunta perguntasJogo[15];
    int k = 0; // Índice para preencher perguntasJogo

    for (int nivel = 1; nivel <= 5; nivel++) {
        // Cria um array temporário para guardar os índices das perguntas do nível atual
        int indicesNivel[contagemPorNivel[nivel - 1]];
        int countNivel = 0;
        // Coleta os índices
        for (int i = 0; i < banco->quantidadeAtual; i++) {
            if (banco->perguntas[i].dificuldade == nivel) {
                indicesNivel[countNivel++] = i;
            }
        }
        // Embaralha os índices coletados
        embaralharIndices(indicesNivel, countNivel);
        // Seleciona as 3 primeiras perguntas do array de índices embaralhado
        for (int i = 0; i < 3; i++) {
            perguntasJogo[k++] = banco->perguntas[indicesNivel[i]];
        }
    }
    // Agora perguntasJogo contém 3 perguntas aleatórias de cada nível, em ordem de nível.

    // 3. Executar o loop do jogo
    int perguntaAtualIdx = 0; // Índice da pergunta atual (0 a 14)
    long long premioAcumulado = 0;
    long long premioGarantido = 0;
    // Tabela de prêmios por acerto
    long long tabelaPremios[15] = {    500,   1000,   2000,   3000,   5000,  // Nível 1 (Perguntas 1-5) -> Marco 1: 5k
                                    10000,  20000,  30000,  40000,  50000, // Nível 2 (Perguntas 6-10) -> Marco 2: 50k
                                   100000, 200000, 300000, 500000, 1000000 // Nível 3 (Perguntas 11-15) -> Final: 1M
                                };

    while (perguntaAtualIdx < 15) {
        Pergunta p = perguntasJogo[perguntaAtualIdx]; // 'p' agora está no escopo do while
        long long valorPergunta = tabelaPremios[perguntaAtualIdx];
        long long valorSeErrar = premioGarantido;
        long long valorSeParar = (perguntaAtualIdx == 0) ? 0 : tabelaPremios[perguntaAtualIdx - 1];

        printf("\n========================================");
        printf("\nPergunta %d / 15 (Nível %d)\n", perguntaAtualIdx + 1, p.dificuldade);
        printf("----------------------------------------\n");
        printf("Valendo: R$ %lld\n", valorPergunta);
        printf("Se parar: R$ %lld | Se errar: R$ %lld\n", valorSeParar, valorSeErrar);
        printf("----------------------------------------\n");
        printf("Enunciado: %s\n", p.enunciado);
        printf("  A) %s\n", p.alternativaA);
        printf("  B) %s\n", p.alternativaB);
        printf("  C) %s\n", p.alternativaC);
        printf("  D) %s\n", p.alternativaD);
        printf("----------------------------------------\n");
        printf("Sua resposta (A, B, C, D) ou 'P' para parar: ");

        char respostaUsuario;
        // Loop para garantir entrada válida (A, B, C, D ou P)
        while (scanf(" %c", &respostaUsuario) != 1) {
            limparBufferEntrada();
            printf("Entrada inválida. Digite A, B, C, D ou P: ");
        }
        respostaUsuario = toupper(respostaUsuario);
        limparBufferEntrada();

        // Opção de Parar
        if (respostaUsuario == 'P') {
            printf("\nVocê decidiu parar.\n");
            premioAcumulado = valorSeParar;
            printf("Você leva para casa R$ %lld! Parabéns!\n", premioAcumulado);
            break; // Sai do loop do jogo
        }

        // Validação da Alternativa
        if (respostaUsuario != 'A' && respostaUsuario != 'B' && respostaUsuario != 'C' && respostaUsuario != 'D') {
            printf("Opção inválida. Use A, B, C, D ou P.\n");
            // Não incrementa perguntaAtualIdx, repete a pergunta
            printf("Pressione Enter para tentar novamente...");
            getchar();
            continue; 
        }

        // Verificar se a resposta está correta
        if (respostaUsuario == p.respostaCorreta) {
            premioAcumulado = valorPergunta;
            printf("\n****************************************\n");
            printf("* RESPOSTA CORRETA! Você ganhou R$ %lld! *\n", premioAcumulado);
            printf("****************************************\n");            
            // Atualiza prêmio garantido nos marcos de segurança
            if (perguntaAtualIdx + 1 == 5) { // Acertou a 5ª pergunta
                premioGarantido = tabelaPremios[4]; // Garante 5.000
                printf("\n>>> Marco de segurança alcançado! Você garantiu R$ %lld! <<<\n", premioGarantido);
            }
            if (perguntaAtualIdx + 1 == 10) { // Acertou a 10ª pergunta
                premioGarantido = tabelaPremios[9]; // Garante 50.000
                printf("\n>>> Marco de segurança alcançado! Você garantiu R$ %lld! <<<\n", premioGarantido);
            }
            
            perguntaAtualIdx++; // Avança para a próxima pergunta

            // Verifica se ganhou o jogo
            if (perguntaAtualIdx == 15) {
                 printf("\n=============================================================\n");
                 printf("* PARABÉNS! VOCÊ É O MAIS NOVO MILIONÁRIO DO PEDAÇO! *\n");
                 printf("*         Você respondeu tudo e ganhou R$ %lld!         *\n", premioAcumulado);
                 printf("=============================================================\n");
                 // O loop while terminará naturalmente
            } else {
                // Pausa antes da próxima pergunta
                printf("\nPressione Enter para a próxima pergunta...");
                getchar(); 
            }
        } else { // Resposta Errada
            printf("\nXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n");
            printf("X        QUE PENA! RESPOSTA ERRADA!        X\n");
            printf("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n");
            printf("A resposta correta era a alternativa %c.\n", p.respostaCorreta); // 'p' está em escopo aqui
            premioAcumulado = premioGarantido; // Jogador fica com o prêmio garantido
            printf("Você perdeu, mas leva para casa o prêmio seguro de R$ %lld.\n", premioAcumulado);
            break; // Sai do loop do jogo
        } // Fim do if/else da resposta correta/errada
    } // Fim do while (loop do jogo)

    printf("\n--- Fim do Jogo ---\n");
    // A função retorna para o menu principal
} // Fim da função iniciarJogo

