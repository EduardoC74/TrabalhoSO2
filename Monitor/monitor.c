#include <tchar.h>
#include <math.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <windows.h>
#include <time.h>
#include "..\Estruturas\estruturas.h"

void criaFileMap(HANDLE* hFileMap, BOOL* primeiroProcesso, TCHAR* caminho) {

    *hFileMap = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, caminho);
    if (*hFileMap == NULL) {
        *primeiroProcesso = TRUE;
        //criamos o bloco de memoria partilhada
        *hFileMap = CreateFileMapping(
            INVALID_HANDLE_VALUE,
            NULL,
            PAGE_READWRITE,
            0,
            sizeof(BufferCircular), //tamanho da memoria partilhada
            caminho);//nome do filemapping. nome que vai ser usado para partilha entre processos

        if (*hFileMap == NULL) {
            _tprintf(TEXT("Erro no CreateFileMapping\n"));
            return -1;
        }
    }
}

DWORD WINAPI ThreadAcaba(LPVOID param) {
    DadosThreads* dados = (DadosThreads*)param;

    WaitForSingleObject(dados->acabou, INFINITE);
    _tprintf(_T("\nJogo terminado!\n"));
    dados->terminar = 1;

    return;
}

DWORD WINAPI ThreadmostraMapa(LPVOID param) {
    DadosThreads* dados = (DadosThreads*)param;
    BOOL primeira = TRUE;

    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO Info;
    WORD defaultAttributes = GetForegroundWindow;
    WORD swapAttribute = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
    GetConsoleScreenBufferInfo(handle, &Info);
    defaultAttributes = Info.wAttributes;

    while (!dados->terminar) {

        if (dados->memPar->mapaJogo.tamMap != 0) {

            /*if (primeira) {
                DWORD tempo;
                tempo = dados->memPar->mapaJogo.tempAgua / 1000;
                while (tempo > 0) {
                    _tprintf(_T("A agua vai comecar a correr em  %d\n"), tempo);
                    tempo--;
                    Sleep(1000);
                }
                primeira = FALSE;
            }*/

            WaitForSingleObject(dados->hMutex, INFINITE);
            _tprintf(_T("\t"));
            for (int i = 0; i < dados->memPar->mapaJogo.tamMap + 2; i++) //impresao dos x antes do tabuleiro
                _tprintf(_T("-"));
            _tprintf(_T("\n"));

            for (int i = 0; i < dados->memPar->mapaJogo.tamMap; i++) {
                _tprintf(_T("\t|"));

                for (int j = 0; j < dados->memPar->mapaJogo.tamMap; j++) {

                    if (dados->memPar->mapaJogo.mapa[i][j].agua) {
                        SetConsoleTextAttribute(handle, swapAttribute);
                        _tprintf(_T("%c"), dados->memPar->mapaJogo.mapa[i][j].tubos);
                        SetConsoleTextAttribute(handle, defaultAttributes);
                    }
                    else
                        _tprintf(_T("%c"), dados->memPar->mapaJogo.mapa[i][j].tubos);

                }
                _tprintf(_T("|"));
                _tprintf(_T("\n"));
            }

            _tprintf(_T("\t"));
            for (int i = 0; i < dados->memPar->mapaJogo.tamMap + 2; i++) //impresao dos x antes do tabuleiro
                _tprintf(_T("-"));
            _tprintf(_T("\n"));

            ReleaseMutex(dados->hMutex);

            Sleep(1000);
        }
    }
    return 0;
}

DWORD WINAPI ThreadMonitor(LPVOID param) {
    DadosThreads* dados = (DadosThreads*)param;
    Comandos cel;
    TCHAR comando[256];

    do{
        cel.inserirParedes[0] = 0;
        cel.modoAleatorio = FALSE;
        cel.pararAgua[0] = 0;

        _fgetts(comando, 256, stdin);
        comando[_tcslen(comando) - 1] = '\0';
        _tprintf(_T("\n"));

        WaitForSingleObject(dados->hMutex, INFINITE);

        if (dados->memPar->mapaJogo.tamMap != 0) {

            if (_tcscmp(comando, _T("inserir parede")) == 0) {
                cel.inserirParedes[0] = 1;
                do {
                    _tprintf(_T("Linha: "));
                    wscanf_s(_T("%d"), &cel.inserirParedes[1]);
                    _tprintf(_T("Coluna: "));
                    wscanf_s(_T("%d"), &cel.inserirParedes[2]);
                } while (cel.inserirParedes[1] < 0 || cel.inserirParedes[1] >= dados->memPar->mapaJogo.tamMap ||
                    cel.inserirParedes[2] < 0 || cel.inserirParedes[2] >= dados->memPar->mapaJogo.tamMap);
            }
            else if (_tcscmp(comando, _T("modo aleatorio")) == 0) {
                cel.modoAleatorio = TRUE;
            }
            else if (_tcscmp(comando, _T("parar agua")) == 0) {
                cel.pararAgua[0] = 1;
                _tprintf(_T("Tempo: "));
                wscanf_s(_T("%d"), &cel.pararAgua[1]);
            }
        }
        ReleaseMutex(dados->hMutex);

        //aqui entramos na logica da aula teorica

        //esperamos por uma posicao para escrevermos
        WaitForSingleObject(dados->hSemEscrita, INFINITE);

        //esperamos que o mutex esteja livre
        WaitForSingleObject(dados->hMutex, INFINITE);

        //vamos copiar a variavel cel para a memoria partilhada (para a posição de escrita)
        CopyMemory(&dados->memPar->comandos[dados->memPar->posE], &cel, sizeof(Comandos));
        dados->memPar->posE++; //incrementamos a posicao de escrita para o proximo produtor escrever na posicao seguinte

        //se apos o incremento a posicao de escrita chegar ao fim, tenho de voltar ao inicio
        if (dados->memPar->posE == TAM_BUFFER)
            dados->memPar->posE = 0;

        //libertamos o mutex
        ReleaseMutex(dados->hMutex);

        //libertamos o semaforo. temos de libertar uma posicao de leitura
        ReleaseSemaphore(dados->hSemLeitura, 1, NULL);
    
    } while (_tcscmp(comando, _T("sair")) != 0 && !dados->terminar);

    return 0;
}

int _tmain(int argc, TCHAR* argv[])
{
    HANDLE hFileMap; //handle para o file map
    HANDLE hThreads[3];
    HANDLE hServidor;
    DadosThreads dados;
    BOOL  primeiroProcesso = FALSE;
    TCHAR comando[100];

#ifdef UNICODE
    _setmode(_fileno(stdin), _O_WTEXT);
    _setmode(_fileno(stdout), _O_WTEXT);
    _setmode(_fileno(stderr), _O_WTEXT);
#endif

    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
    
    hServidor = CreateSemaphore(NULL, 1, 1, TEXT("SEMAPHORE_UNIQUE_SERVIDOR_NAME"));

    // Se ERROR_ALREADY_EXISTS , entao nao deixa iniciar
    if (GetLastError() != ERROR_ALREADY_EXISTS) {
        _ftprintf(stderr, TEXT("[ERRO] Nao existe um servidor aberto!\n"));
        return -1;
    }

    //criar semaforo que conta as escritas
    dados.hSemEscrita = CreateSemaphore(NULL, TAM_BUFFER, TAM_BUFFER, TEXT("SO2_SEMAFORO_ESCRITA"));

    //criar semaforo que conta as leituras
    //0 porque nao ha nada para ser lido e depois podemos ir até um maximo de 10 posicoes para serem lidas
    dados.hSemLeitura = CreateSemaphore(NULL, 0, TAM_BUFFER, TEXT("SO2_SEMAFORO_LEITURA"));

    //criar mutex para os produtores
    dados.hMutex = CreateMutex(NULL, FALSE, TEXT("SO2_MUTEX_PRODUTOR"));

    if (dados.hSemEscrita == NULL || dados.hSemLeitura == NULL || dados.hMutex == NULL) {
        _tprintf(TEXT("Erro no CreateSemaphore ou no CreateMutex\n"));
        return -1;
    }

    //o openfilemapping vai abrir um filemapping com o nome que passamos no lpName
    //se devolver um HANDLE ja existe e nao fazemos a inicializacao
    //se devolver NULL nao existe e vamos fazer a inicializacao

    criaFileMap(&hFileMap, &primeiroProcesso, _T("SO2_MEM_PARTILHADA"));

    //mapeamos o bloco de memoria para o espaco de enderaçamento do nosso processo
    dados.memPar = (BufferCircular*)MapViewOfFile(hFileMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);

    if (dados.memPar == NULL) {
        _tprintf(TEXT("Erro no MapViewOfFile\n"));
        return -1;
    }

    if (primeiroProcesso == TRUE) {
        //dados.memPar->nMonitores = 0;
        dados.memPar->posE = 0;
        dados.memPar->posL = 0;
    }

    dados.terminar = 0;

    dados.acabou = OpenEvent(EVENT_ALL_ACCESS, TRUE, _T("SO2_SERVIDOR_ACABOU"));
        /*CreateEvent(NULL, //A pointer to a SECURITY_ATTRIBUTES structure. If this parameter is NULL, the handle cannot be inherited by child processes
        FALSE,//If this parameter is TRUE, the function creates a manual-reset event object, which requires the use of the ResetEvent function to set the event state to nonsignaled
        //If this parameter is FALSE, the function creates an auto-reset event object, and system automatically resets the event state to nonsignaled after a single waiting thread has been released. 
        FALSE, //If this parameter is TRUE, the initial state of the event object is signaled; otherwise, it is nonsignaled.
        "SO2_SERVIDOR_ACABOU");*/ //optional the name of the event object

    //temos de usar o mutex para aumentar o nProdutores para termos os ids corretos
    WaitForSingleObject(dados.hMutex, INFINITE);
    dados.memPar->nMonitores++;
    ReleaseMutex(dados.hMutex);

    //lancamos a thread
    hThreads[0] = CreateThread(NULL, 0, ThreadMonitor, &dados, 0, NULL);
    if (hThreads[0] == NULL) {
        _tprintf(_T("Erro ao criar a thread Monitor!"));
        return -1;
    }

    hThreads[1] = CreateThread(NULL, 0, ThreadAcaba, &dados, 0, NULL);
    if (hThreads[1] == NULL) {
        _tprintf(_T("Erro ao criar a thread Monitor!"));
        return -1;
    }

    hThreads[2] = CreateThread(NULL, 0, ThreadmostraMapa, &dados, 0, NULL);
    if (hThreads[2] == NULL) {
        _tprintf(_T("Erro ao criar a thread Monitor!"));
        return -1;
    }

    _tprintf(_T("--> Jogo dos Tubos (Monitor) <--\n\n"));

    //esperar que a thread termine
    WaitForMultipleObjects(3, hThreads, FALSE, INFINITE);

    CloseHandle(dados.hSemEscrita);
    CloseHandle(dados.hSemLeitura);
    CloseHandle(hServidor);
    CloseHandle(dados.acabou);
    CloseHandle(dados.hMutex);
    CloseHandle(hFileMap);
    UnmapViewOfFile(dados.memPar);
    //CloseHandles ... mas é feito automaticamente quando o processo termina

    return 0;
}