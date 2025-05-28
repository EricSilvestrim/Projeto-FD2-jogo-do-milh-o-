#ifndef FUNCOES_PADROES_H
#define FUNCOES_PADROES_H

// Códigos de cor ANSI
#define COR_RESET    "\x1b[0m"
#define COR_VERMELHO "\x1b[31m"
#define COR_VERDE    "\x1b[32m"
#define COR_AMARELO  "\x1b[33m"
#define COR_AZUL     "\x1b[34m"
#define COR_ROXO     "\x1b[35m"
#define COR_CIANO    "\x1b[36m"

// Limpa a tela do terminal
void limparTela();

// Pausa a execução até o usuário pressionar ENTER
void pausarTela();

// Imprime um texto colorido
void printColorido(const char* texto, const char* cor);

// Emite beeps:
// 1: acerto, 2: erro, 3: aviso
void tocarSom(int tipo);

// Limpa o buffer de entrada (stdin)
void limparBuffer();

#endif
