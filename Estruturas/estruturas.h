#pragma once

#include <tchar.h>
#include <math.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <windows.h>
#include <time.h>

#define TAM_BUFFER 20


#define FILE_MENU_BITMAP1 1
#define FILE_MENU_BITMAP2 2


//cada posicao pode ser composta por tubos ou agua
typedef struct {
    TCHAR tubos;
    BOOL agua;
}CelulaBuffer; 

//estrutura para o mapa de jogo
typedef struct {
    int tamMap;
    int tempAgua;
    int paraAgua;
    int paraAguaMon;
    DWORD inicioX;
    DWORD inicioY;
    DWORD destinoX;
    DWORD destinoY;
    BOOL aleatorio;
    int clis;
    CelulaBuffer mapa[TAM_BUFFER][TAM_BUFFER]; //buffer circular em si (array de estruturas)
}AreaDeJogo;

typedef struct {
    DWORD pararAgua[2];
    BOOL modoAleatorio;
    DWORD inserirParedes[3];
}Comandos;

//representa a nossa memoria partilhada
typedef struct {
    int nMonitores;
    int posE; //proxima posicao de escrita
    int posL; //proxima posicao de leitura
    AreaDeJogo mapaJogo;
    Comandos comandos[TAM_BUFFER];
}BufferCircular;

typedef struct {
    //DWORD totalClicks;
    TCHAR nome[100];
    HANDLE pipe;
    TCHAR tipoJogo[100];
    DWORD hover;
    DWORD clickTipo;
    DWORD linhaColuna[2];
    TCHAR ordem[6];
    DWORD nivel;
    DWORD clis;
    TCHAR pecas[13];
    AreaDeJogo mapaJogo;
    HANDLE iniciaJogo;
    BOOL aleatorio;
}MensagesCliente;

// estrutura do named pipe
typedef struct {
    HANDLE hPipe; // handle do pipe
    OVERLAPPED overlap;
    BOOL activo; //representa se a instancia do named pipe esta ou nao ativa, se ja tem um cliente ou nao
} PipeDados;

//estrutura de apoio
typedef struct {
    BufferCircular* memPar; //ponteiro para a memoria partilhada
    MensagesCliente cli[2];
    HANDLE hSemEscrita; //handle para o semaforo que controla as escritas (controla quantas posicoes estao vazias)
    HANDLE hSemLeitura; //handle para o semaforo que controla as leituras (controla quantas posicoes estao preenchidas)
    HANDLE hMutex;
    PipeDados hPipes[2];
    HANDLE hEvents[2];
    HANDLE hPipeCli;
    HANDLE acabou;
    HANDLE comecaCliente;
    HANDLE inicia;
    HANDLE clienteEscreveu;
    HANDLE clienteIniciou2;
    HANDLE clientePipe;
    HANDLE atualizaJogo;
    HANDLE leDoCliente;
    HWND hWnd;
    int nClientes;
    int terminar; // 1 para sair, 0 em caso contrário
}DadosThreads;






