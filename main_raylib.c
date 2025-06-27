#include "perguntas.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <errno.h>
#include <raylib.h>

// Definições para a interface gráfica
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define TITLE "Jogo de Perguntas e Respostas"

// Cores personalizadas
#define COLOR_BACKGROUND (Color){ 25, 25, 75, 255 }
#define COLOR_TITLE (Color){ 230, 230, 255, 255 }
#define COLOR_TEXT (Color){ 220, 220, 220, 255 }
#define COLOR_BUTTON (Color){ 80, 80, 180, 255 }
#define COLOR_BUTTON_HOVER (Color){ 100, 100, 220, 255 }
#define COLOR_BUTTON_ACTIVE (Color){ 120, 120, 255, 255 }
#define COLOR_CORRECT (Color){ 50, 180, 50, 255 }
#define COLOR_WRONG (Color){ 180, 50, 50, 255 }
#define COLOR_HIGHLIGHT (Color){ 255, 215, 0, 255 }

// Estados do jogo
typedef enum {
    STATE_MENU,
    STATE_GAME,
    STATE_RESULT,
    STATE_EXIT
} GameState;

// Estrutura para botões
typedef struct {
    Rectangle rect;
    const char *text;
    bool hover;
    bool pressed;
} Button;

// Estrutura para o estado do jogo
typedef struct {
    GameState currentState;
    BancoDePerguntas *banco;
    Pergunta perguntasJogo[15];
    int perguntaAtualIdx;
    long long premioAcumulado;
    long long premioGarantido;
    long long tabelaPremios[15];
    char respostaUsuario;
    bool respostaAvaliada;
    bool respostaCorreta;
    bool jogoTerminado;
    bool usuarioParou;
    char mensagemResultado[256];
} GameData;

// Protótipos de funções
void inicializarJogo(GameData *gameData);
void atualizarMenu(GameData *gameData);
void desenharMenu(GameData *gameData);
void atualizarJogo(GameData *gameData);
void desenharJogo(GameData *gameData);
void atualizarResultado(GameData *gameData);
void desenharResultado(GameData *gameData);
void selecionarPerguntasJogo(GameData *gameData);
void embaralharIndices(int *array, int n);
Button criarBotao(float x, float y, float largura, float altura, const char *texto);
bool atualizarBotao(Button *botao);
void desenharBotao(Button botao);
void desenharTextoComQuebraLinha(const char *texto, int posX, int posY, int fontSize, Color color, int maxWidth);

// Função principal
int main() {
    // Inicializa o gerador de números aleatórios
    srand(time(NULL));
    
    // Inicializa a janela
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, TITLE);
    SetTargetFPS(60);
    
    // Inicializa os dados do jogo
    GameData gameData;
    inicializarJogo(&gameData);
    
    // Loop principal
    while (!WindowShouldClose() && gameData.currentState != STATE_EXIT) {
        // Atualização
        switch (gameData.currentState) {
            case STATE_MENU:
                atualizarMenu(&gameData);
                break;
            case STATE_GAME:
                atualizarJogo(&gameData);
                break;
            case STATE_RESULT:
                atualizarResultado(&gameData);
                break;
            default:
                break;
        }
        
        // Desenho
        BeginDrawing();
        ClearBackground(COLOR_BACKGROUND);
        
        switch (gameData.currentState) {
            case STATE_MENU:
                desenharMenu(&gameData);
                break;
            case STATE_GAME:
                desenharJogo(&gameData);
                break;
            case STATE_RESULT:
                desenharResultado(&gameData);
                break;
            default:
                break;
        }
        
        EndDrawing();
    }
    
    // Libera a memória alocada
    liberarBancoDePerguntas(gameData.banco);
    
    // Fecha a janela
    CloseWindow();
    
    return 0;
}

// Inicializa os dados do jogo
void inicializarJogo(GameData *gameData) {
    // Inicializa o estado do jogo
    gameData->currentState = STATE_MENU;
    gameData->perguntaAtualIdx = 0;
    gameData->premioAcumulado = 0;
    gameData->premioGarantido = 0;
    gameData->respostaUsuario = '\0';
    gameData->respostaAvaliada = false;
    gameData->respostaCorreta = false;
    gameData->jogoTerminado = false;
    gameData->usuarioParou = false;
    strcpy(gameData->mensagemResultado, "");
    
    // Tabela de prêmios
    gameData->tabelaPremios[0] = 500;
    gameData->tabelaPremios[1] = 1000;
    gameData->tabelaPremios[2] = 2000;
    gameData->tabelaPremios[3] = 3000;
    gameData->tabelaPremios[4] = 5000;    // Marco 1: 5k
    gameData->tabelaPremios[5] = 10000;
    gameData->tabelaPremios[6] = 20000;
    gameData->tabelaPremios[7] = 30000;
    gameData->tabelaPremios[8] = 40000;
    gameData->tabelaPremios[9] = 50000;   // Marco 2: 50k
    gameData->tabelaPremios[10] = 100000;
    gameData->tabelaPremios[11] = 200000;
    gameData->tabelaPremios[12] = 300000;
    gameData->tabelaPremios[13] = 500000;
    gameData->tabelaPremios[14] = 1000000; // Final: 1M
    
    // Cria o banco de perguntas
    gameData->banco = criarBancoDePerguntas();
    if (gameData->banco == NULL) {
        fprintf(stderr, "Falha crítica: Não foi possível inicializar o banco de perguntas.\n");
        gameData->currentState = STATE_EXIT;
        return;
    }
    
    // Carrega as perguntas
    const char *arquivoBinario = "/home/ubuntu/dados/perguntas.bin";
    const char *arquivoCSVInicial = "/home/ubuntu/perguntas_iniciais.csv";
    
    // Tenta carregar do arquivo binário primeiro
    if (carregarPerguntasBinario(gameData->banco, arquivoBinario) != 0) {
        // Se falhar, tenta carregar do CSV inicial
        if (carregarPerguntasCSV(gameData->banco, arquivoCSVInicial) != 0) {
            // Se ambos falharem, exibe mensagem de erro
            fprintf(stderr, "Erro: Não foi possível carregar perguntas. Verifique os arquivos.\n");
            gameData->currentState = STATE_EXIT;
            return;
        }
    }
}

// Atualiza o estado do menu
void atualizarMenu(GameData *gameData) {
    // Botão Jogar
    Button botaoJogar = criarBotao(SCREEN_WIDTH/2 - 150, SCREEN_HEIGHT/2, 300, 60, "JOGAR");
    
    // Verifica se o botão foi clicado
    if (atualizarBotao(&botaoJogar)) {
        // Verifica se há perguntas suficientes para o jogo
        int contagemPorNivel[5] = {0}; // Índices 0 a 4 para níveis 1 a 5
        for (int i = 0; i < gameData->banco->quantidadeAtual; i++) {
            if (gameData->banco->perguntas[i].dificuldade >= 1 && gameData->banco->perguntas[i].dificuldade <= 5) {
                contagemPorNivel[gameData->banco->perguntas[i].dificuldade - 1]++;
            }
        }
        
        bool perguntasSuficientes = true;
        for (int i = 0; i < 5; i++) {
            if (contagemPorNivel[i] < 3) { // Precisa de 3 perguntas por nível
                perguntasSuficientes = false;
                break;
            }
        }
        
        if (perguntasSuficientes) {
            // Reinicia os dados do jogo
            gameData->perguntaAtualIdx = 0;
            gameData->premioAcumulado = 0;
            gameData->premioGarantido = 0;
            gameData->respostaUsuario = '\0';
            gameData->respostaAvaliada = false;
            gameData->respostaCorreta = false;
            gameData->jogoTerminado = false;
            gameData->usuarioParou = false;
            strcpy(gameData->mensagemResultado, "");
            
            // Seleciona as perguntas para o jogo
            selecionarPerguntasJogo(gameData);
            
            // Muda para o estado de jogo
            gameData->currentState = STATE_GAME;
        }
    }
    
    // Botão Sair
    Button botaoSair = criarBotao(SCREEN_WIDTH/2 - 150, SCREEN_HEIGHT/2 + 100, 300, 60, "SAIR");
    
    // Verifica se o botão foi clicado
    if (atualizarBotao(&botaoSair)) {
        // Salva os dados antes de sair
        const char *arquivoBinario = "/home/ubuntu/dados/perguntas.bin";
        salvarPerguntasBinario(gameData->banco, arquivoBinario);
        
        // Muda para o estado de saída
        gameData->currentState = STATE_EXIT;
    }
}

// Desenha o menu
void desenharMenu(GameData *gameData) {
    // Título
    DrawText(TITLE, SCREEN_WIDTH/2 - MeasureText(TITLE, 60)/2, 100, 60, COLOR_TITLE);
    
    // Subtítulo
    const char *subtitulo = "Responda 15 perguntas para ganhar R$ 1 Milhão!";
    DrawText(subtitulo, SCREEN_WIDTH/2 - MeasureText(subtitulo, 30)/2, 200, 30, COLOR_TEXT);
    
    // Botão Jogar
    Button botaoJogar = criarBotao(SCREEN_WIDTH/2 - 150, SCREEN_HEIGHT/2, 300, 60, "JOGAR");
    desenharBotao(botaoJogar);
    
    // Botão Sair
    Button botaoSair = criarBotao(SCREEN_WIDTH/2 - 150, SCREEN_HEIGHT/2 + 100, 300, 60, "SAIR");
    desenharBotao(botaoSair);
    
    // Informações adicionais
    const char *info = "Marcos de segurança: Acertando a 5ª (R$ 5.000) e 10ª (R$ 50.000) perguntas.";
    DrawText(info, SCREEN_WIDTH/2 - MeasureText(info, 20)/2, SCREEN_HEIGHT - 100, 20, COLOR_TEXT);
    
    // Verificação de perguntas suficientes
    int contagemPorNivel[5] = {0};
    for (int i = 0; i < gameData->banco->quantidadeAtual; i++) {
        if (gameData->banco->perguntas[i].dificuldade >= 1 && gameData->banco->perguntas[i].dificuldade <= 5) {
            contagemPorNivel[gameData->banco->perguntas[i].dificuldade - 1]++;
        }
    }
    
    bool perguntasSuficientes = true;
    for (int i = 0; i < 5; i++) {
        if (contagemPorNivel[i] < 3) {
            perguntasSuficientes = false;
            break;
        }
    }
    
    if (!perguntasSuficientes) {
        const char *aviso = "AVISO: Não há perguntas suficientes para iniciar o jogo!";
        DrawText(aviso, SCREEN_WIDTH/2 - MeasureText(aviso, 24)/2, SCREEN_HEIGHT/2 - 100, 24, RED);
        
        const char *detalhes = "São necessárias pelo menos 3 perguntas de cada nível (1-5).";
        DrawText(detalhes, SCREEN_WIDTH/2 - MeasureText(detalhes, 20)/2, SCREEN_HEIGHT/2 - 70, 20, RED);
    }
}

// Atualiza o estado do jogo
void atualizarJogo(GameData *gameData) {
    // Se o jogo terminou, muda para o estado de resultado
    if (gameData->jogoTerminado) {
        gameData->currentState = STATE_RESULT;
        return;
    }
    
    // Se a resposta ainda não foi avaliada
    if (!gameData->respostaAvaliada) {
        // Verifica se alguma tecla de resposta foi pressionada
        if (IsKeyPressed(KEY_A)) gameData->respostaUsuario = 'A';
        else if (IsKeyPressed(KEY_B)) gameData->respostaUsuario = 'B';
        else if (IsKeyPressed(KEY_C)) gameData->respostaUsuario = 'C';
        else if (IsKeyPressed(KEY_D)) gameData->respostaUsuario = 'D';
        else if (IsKeyPressed(KEY_P)) gameData->respostaUsuario = 'P';
        
        // Verifica se o usuário clicou em alguma alternativa
        Vector2 mousePos = GetMousePosition();
        
        // Posições e dimensões das alternativas
        float altX = SCREEN_WIDTH/2 - 400;
        float altY = SCREEN_HEIGHT/2 + 50;
        float altWidth = 800;
        float altHeight = 60;
        float altSpacing = 70;
        
        // Alternativa A
        Rectangle rectA = { altX, altY, altWidth, altHeight };
        if (CheckCollisionPointRec(mousePos, rectA) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            gameData->respostaUsuario = 'A';
        }
        
        // Alternativa B
        Rectangle rectB = { altX, altY + altSpacing, altWidth, altHeight };
        if (CheckCollisionPointRec(mousePos, rectB) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            gameData->respostaUsuario = 'B';
        }
        
        // Alternativa C
        Rectangle rectC = { altX, altY + 2*altSpacing, altWidth, altHeight };
        if (CheckCollisionPointRec(mousePos, rectC) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            gameData->respostaUsuario = 'C';
        }
        
        // Alternativa D
        Rectangle rectD = { altX, altY + 3*altSpacing, altWidth, altHeight };
        if (CheckCollisionPointRec(mousePos, rectD) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            gameData->respostaUsuario = 'D';
        }
        
        // Botão Parar
        Button botaoParar = criarBotao(SCREEN_WIDTH - 150, SCREEN_HEIGHT - 80, 120, 50, "PARAR");
        if (atualizarBotao(&botaoParar)) {
            gameData->respostaUsuario = 'P';
        }
        
        // Se o usuário escolheu uma resposta
        if (gameData->respostaUsuario != '\0') {
            // Marca a resposta como avaliada
            gameData->respostaAvaliada = true;
            
            // Opção de Parar
            if (gameData->respostaUsuario == 'P') {
                gameData->usuarioParou = true;
                gameData->jogoTerminado = true;
                
                // Define o prêmio acumulado
                if (gameData->perguntaAtualIdx > 0) {
                    gameData->premioAcumulado = gameData->tabelaPremios[gameData->perguntaAtualIdx - 1];
                }
                
                // Define a mensagem de resultado
                sprintf(gameData->mensagemResultado, "Você decidiu parar e leva para casa R$ %lld!", gameData->premioAcumulado);
                return;
            }
            
            // Verifica se a resposta está correta
            Pergunta p = gameData->perguntasJogo[gameData->perguntaAtualIdx];
            gameData->respostaCorreta = (gameData->respostaUsuario == p.respostaCorreta);
            
            if (gameData->respostaCorreta) {
                // Atualiza o prêmio acumulado
                gameData->premioAcumulado = gameData->tabelaPremios[gameData->perguntaAtualIdx];
                
                // Atualiza o prêmio garantido nos marcos de segurança
                if (gameData->perguntaAtualIdx + 1 == 5) { // Acertou a 5ª pergunta
                    gameData->premioGarantido = gameData->tabelaPremios[4]; // Garante 5.000
                }
                if (gameData->perguntaAtualIdx + 1 == 10) { // Acertou a 10ª pergunta
                    gameData->premioGarantido = gameData->tabelaPremios[9]; // Garante 50.000
                }
            } else {
                // Se errou, fica com o prêmio garantido
                gameData->premioAcumulado = gameData->premioGarantido;
                gameData->jogoTerminado = true;
                
                // Define a mensagem de resultado
                sprintf(gameData->mensagemResultado, "Que pena! Você errou, mas leva para casa o prêmio seguro de R$ %lld.", gameData->premioAcumulado);
                return;
            }
        }
    } else {
        // Se a resposta já foi avaliada e está correta
        if (gameData->respostaCorreta) {
            // Verifica se o usuário pressionou Enter para continuar
            if (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                // Avança para a próxima pergunta
                gameData->perguntaAtualIdx++;
                
                // Verifica se ganhou o jogo
                if (gameData->perguntaAtualIdx == 15) {
                    gameData->jogoTerminado = true;
                    sprintf(gameData->mensagemResultado, "PARABÉNS! Você é o mais novo MILIONÁRIO do pedaço! Ganhou R$ %lld!", gameData->premioAcumulado);
                    return;
                }
                
                // Reinicia para a próxima pergunta
                gameData->respostaUsuario = '\0';
                gameData->respostaAvaliada = false;
            }
        }
    }
}

// Desenha o estado do jogo
void desenharJogo(GameData *gameData) {
    Pergunta p = gameData->perguntasJogo[gameData->perguntaAtualIdx];
    long long valorPergunta = gameData->tabelaPremios[gameData->perguntaAtualIdx];
    long long valorSeErrar = gameData->premioGarantido;
    long long valorSeParar = (gameData->perguntaAtualIdx == 0) ? 0 : gameData->tabelaPremios[gameData->perguntaAtualIdx - 1];
    
    // Cabeçalho
    char cabecalho[100];
    sprintf(cabecalho, "Pergunta %d / 15 (Nível %d)", gameData->perguntaAtualIdx + 1, p.dificuldade);
    DrawText(cabecalho, SCREEN_WIDTH/2 - MeasureText(cabecalho, 30)/2, 50, 30, COLOR_TITLE);
    
    // Valores
    char valores[100];
    sprintf(valores, "Valendo: R$ %lld | Se parar: R$ %lld | Se errar: R$ %lld", valorPergunta, valorSeParar, valorSeErrar);
    DrawText(valores, SCREEN_WIDTH/2 - MeasureText(valores, 20)/2, 100, 20, COLOR_TEXT);
    
    // Enunciado
    DrawRectangle(SCREEN_WIDTH/2 - 450, SCREEN_HEIGHT/2 - 150, 900, 150, (Color){ 40, 40, 100, 255 });
    desenharTextoComQuebraLinha(p.enunciado, SCREEN_WIDTH/2 - 430, SCREEN_HEIGHT/2 - 130, 24, COLOR_TEXT, 860);
    
    // Posições e dimensões das alternativas
    float altX = SCREEN_WIDTH/2 - 400;
    float altY = SCREEN_HEIGHT/2 + 50;
    float altWidth = 800;
    float altHeight = 60;
    float altSpacing = 70;
    
    // Alternativas
    Color colorA = COLOR_BUTTON;
    Color colorB = COLOR_BUTTON;
    Color colorC = COLOR_BUTTON;
    Color colorD = COLOR_BUTTON;
    
    // Se a resposta foi avaliada, destaca a resposta correta e a do usuário
    if (gameData->respostaAvaliada && gameData->respostaUsuario != 'P') {
        // Destaca a resposta correta
        if (p.respostaCorreta == 'A') colorA = COLOR_CORRECT;
        else if (p.respostaCorreta == 'B') colorB = COLOR_CORRECT;
        else if (p.respostaCorreta == 'C') colorC = COLOR_CORRECT;
        else if (p.respostaCorreta == 'D') colorD = COLOR_CORRECT;
        
        // Se a resposta do usuário estiver errada, destaca em vermelho
        if (!gameData->respostaCorreta) {
            if (gameData->respostaUsuario == 'A') colorA = COLOR_WRONG;
            else if (gameData->respostaUsuario == 'B') colorB = COLOR_WRONG;
            else if (gameData->respostaUsuario == 'C') colorC = COLOR_WRONG;
            else if (gameData->respostaUsuario == 'D') colorD = COLOR_WRONG;
        }
    } else {
        // Destaca a alternativa sob o mouse
        Vector2 mousePos = GetMousePosition();
        
        Rectangle rectA = { altX, altY, altWidth, altHeight };
        if (CheckCollisionPointRec(mousePos, rectA)) colorA = COLOR_BUTTON_HOVER;
        
        Rectangle rectB = { altX, altY + altSpacing, altWidth, altHeight };
        if (CheckCollisionPointRec(mousePos, rectB)) colorB = COLOR_BUTTON_HOVER;
        
        Rectangle rectC = { altX, altY + 2*altSpacing, altWidth, altHeight };
        if (CheckCollisionPointRec(mousePos, rectC)) colorC = COLOR_BUTTON_HOVER;
        
        Rectangle rectD = { altX, altY + 3*altSpacing, altWidth, altHeight };
        if (CheckCollisionPointRec(mousePos, rectD)) colorD = COLOR_BUTTON_HOVER;
    }
    
    // Desenha as alternativas
    DrawRectangle(altX, altY, altWidth, altHeight, colorA);
    DrawText(TextFormat("A) %s", p.alternativaA), altX + 20, altY + 15, 24, COLOR_TEXT);
    
    DrawRectangle(altX, altY + altSpacing, altWidth, altHeight, colorB);
    DrawText(TextFormat("B) %s", p.alternativaB), altX + 20, altY + altSpacing + 15, 24, COLOR_TEXT);
    
    DrawRectangle(altX, altY + 2*altSpacing, altWidth, altHeight, colorC);
    DrawText(TextFormat("C) %s", p.alternativaC), altX + 20, altY + 2*altSpacing + 15, 24, COLOR_TEXT);
    
    DrawRectangle(altX, altY + 3*altSpacing, altWidth, altHeight, colorD);
    DrawText(TextFormat("D) %s", p.alternativaD), altX + 20, altY + 3*altSpacing + 15, 24, COLOR_TEXT);
    
    // Botão Parar
    Button botaoParar = criarBotao(SCREEN_WIDTH - 150, SCREEN_HEIGHT - 80, 120, 50, "PARAR");
    desenharBotao(botaoParar);
    
    // Instruções
    if (!gameData->respostaAvaliada) {
        DrawText("Clique na alternativa ou pressione A, B, C ou D para responder", 
                SCREEN_WIDTH/2 - MeasureText("Clique na alternativa ou pressione A, B, C ou D para responder", 20)/2, 
                SCREEN_HEIGHT - 50, 20, COLOR_TEXT);
    } else if (gameData->respostaCorreta) {
        DrawText("RESPOSTA CORRETA! Clique ou pressione ENTER para continuar", 
                SCREEN_WIDTH/2 - MeasureText("RESPOSTA CORRETA! Clique ou pressione ENTER para continuar", 24)/2, 
                SCREEN_HEIGHT - 50, 24, COLOR_CORRECT);
        
        // Exibe mensagem de marco de segurança se aplicável
        if (gameData->perguntaAtualIdx + 1 == 5 || gameData->perguntaAtualIdx + 1 == 10) {
            DrawText(TextFormat("Marco de segurança alcançado! Você garantiu R$ %lld!", gameData->premioGarantido),
                    SCREEN_WIDTH/2 - MeasureText(TextFormat("Marco de segurança alcançado! Você garantiu R$ %lld!", gameData->premioGarantido), 24)/2,
                    SCREEN_HEIGHT - 80, 24, COLOR_HIGHLIGHT);
        }
    }
}

// Atualiza o estado do resultado
void atualizarResultado(GameData *gameData) {
    // Botão Voltar ao Menu
    Button botaoMenu = criarBotao(SCREEN_WIDTH/2 - 150, SCREEN_HEIGHT/2 + 100, 300, 60, "VOLTAR AO MENU");
    
    // Verifica se o botão foi clicado
    if (atualizarBotao(&botaoMenu)) {
        // Muda para o estado de menu
        gameData->currentState = STATE_MENU;
    }
}

// Desenha o resultado
void desenharResultado(GameData *gameData) {
    // Título
    const char *titulo = "Fim do Jogo";
    DrawText(titulo, SCREEN_WIDTH/2 - MeasureText(titulo, 60)/2, 100, 60, COLOR_TITLE);
    
    // Mensagem de resultado
    DrawText(gameData->mensagemResultado, 
            SCREEN_WIDTH/2 - MeasureText(gameData->mensagemResultado, 30)/2, 
            SCREEN_HEIGHT/2 - 50, 30, gameData->usuarioParou ? COLOR_TEXT : (gameData->perguntaAtualIdx == 15 ? COLOR_HIGHLIGHT : COLOR_WRONG));
    
    // Botão Voltar ao Menu
    Button botaoMenu = criarBotao(SCREEN_WIDTH/2 - 150, SCREEN_HEIGHT/2 + 100, 300, 60, "VOLTAR AO MENU");
    desenharBotao(botaoMenu);
}

// Seleciona as perguntas para o jogo
void selecionarPerguntasJogo(GameData *gameData) {
    int k = 0; // Índice para preencher perguntasJogo
    
    for (int nivel = 1; nivel <= 5; nivel++) {
        // Conta quantas perguntas existem deste nível
        int contagemNivel = 0;
        for (int i = 0; i < gameData->banco->quantidadeAtual; i++) {
            if (gameData->banco->perguntas[i].dificuldade == nivel) {
                contagemNivel++;
            }
        }
        
        // Cria um array temporário para guardar os índices das perguntas do nível atual
        int *indicesNivel = (int*)malloc(contagemNivel * sizeof(int));
        if (indicesNivel == NULL) {
            fprintf(stderr, "Erro de alocação de memória para índices de nível.\n");
            return;
        }
        
        // Coleta os índices
        int countNivel = 0;
        for (int i = 0; i < gameData->banco->quantidadeAtual; i++) {
            if (gameData->banco->perguntas[i].dificuldade == nivel) {
                indicesNivel[countNivel++] = i;
            }
        }
        
        // Embaralha os índices coletados
        embaralharIndices(indicesNivel, countNivel);
        
        // Seleciona as 3 primeiras perguntas do array de índices embaralhado
        for (int i = 0; i < 3 && i < countNivel; i++) {
            gameData->perguntasJogo[k++] = gameData->banco->perguntas[indicesNivel[i]];
        }
        
        // Libera a memória do array temporário
        free(indicesNivel);
    }
}

// Embaralha um array de inteiros (Algoritmo Fisher-Yates)
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

// Cria um botão
Button criarBotao(float x, float y, float largura, float altura, const char *texto) {
    Button botao;
    botao.rect = (Rectangle){ x, y, largura, altura };
    botao.text = texto;
    botao.hover = false;
    botao.pressed = false;
    return botao;
}

// Atualiza o estado de um botão
bool atualizarBotao(Button *botao) {
    Vector2 mousePos = GetMousePosition();
    bool clicked = false;
    
    // Verifica se o mouse está sobre o botão
    botao->hover = CheckCollisionPointRec(mousePos, botao->rect);
    
    // Verifica se o botão foi clicado
    if (botao->hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        botao->pressed = true;
        clicked = true;
    } else {
        botao->pressed = false;
    }
    
    return clicked;
}

// Desenha um botão
void desenharBotao(Button botao) {
    Color color = COLOR_BUTTON;
    
    // Muda a cor se o mouse estiver sobre o botão
    if (botao.hover) {
        color = COLOR_BUTTON_HOVER;
    }
    
    // Muda a cor se o botão estiver pressionado
    if (botao.pressed) {
        color = COLOR_BUTTON_ACTIVE;
    }
    
    // Desenha o botão
    DrawRectangle(botao.rect.x, botao.rect.y, botao.rect.width, botao.rect.height, color);
    DrawRectangleLines(botao.rect.x, botao.rect.y, botao.rect.width, botao.rect.height, COLOR_TEXT);
    
    // Desenha o texto do botão
    DrawText(botao.text, 
            botao.rect.x + botao.rect.width/2 - MeasureText(botao.text, 24)/2, 
            botao.rect.y + botao.rect.height/2 - 12, 
            24, COLOR_TEXT);
}

// Desenha texto com quebra de linha
void desenharTextoComQuebraLinha(const char *texto, int posX, int posY, int fontSize, Color color, int maxWidth) {
    int textWidth = MeasureText(texto, fontSize);
    
    // Se o texto couber na largura máxima, desenha normalmente
    if (textWidth <= maxWidth) {
        DrawText(texto, posX, posY, fontSize, color);
        return;
    }
    
    // Caso contrário, quebra o texto em linhas
    int startPos = 0;
    int endPos = 0;
    int currentY = posY;
    int textLength = strlen(texto);
    
    while (startPos < textLength) {
        // Encontra o ponto de quebra
        int breakPos = startPos;
        int lastSpace = -1;
        
        while (breakPos < textLength) {
            if (texto[breakPos] == ' ') {
                lastSpace = breakPos;
            }
            
            int width = MeasureText(TextSubtext(texto, startPos, breakPos - startPos + 1), fontSize);
            if (width > maxWidth) {
                break;
            }
            
            breakPos++;
        }
        
        // Se encontrou um espaço para quebrar
        if (lastSpace != -1 && lastSpace > startPos && breakPos < textLength) {
            endPos = lastSpace;
        } else {
            endPos = breakPos;
        }
        
        // Desenha a linha atual
        DrawText(TextSubtext(texto, startPos, endPos - startPos), posX, currentY, fontSize, color);
        
        // Avança para a próxima linha
        currentY += fontSize + 5;
        startPos = (endPos < textLength && texto[endPos] == ' ') ? endPos + 1 : endPos;
    }
}
