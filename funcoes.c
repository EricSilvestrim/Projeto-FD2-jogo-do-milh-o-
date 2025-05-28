#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#ifdef _WIN32
  #include <windows.h>
#else
  #include <unistd.h>
#endif

#include "funcoespadroes.h"

void limparTela() {
  #ifdef _WIN32
    system("cls");
  #else
    system("clear");
  #endif
}

void pausarTela() {
  printf("\nPressione ENTER para continuar...");
  limparBuffer();
}

void printColorido(const char* texto, const char* cor) {
  printf("%s%s%s", cor, texto, COR_RESET);
}

void tocarSom(int tipo) {
  switch (tipo) {
    case 1: printf("\a"); break;
    case 2: printf("\a\a"); break;
    case 3: printf("\a\a\a"); break;
    default: break;
  }
  fflush(stdout);
  #ifdef _WIN32
    Sleep(300);
  #else
    usleep(300000);
  #endif
}

void limparBuffer() {
  int c;
  // consome até encontrar '\n' ou EOF
  while ((c = getchar()) != '\n' && c != EOF) { }
}
