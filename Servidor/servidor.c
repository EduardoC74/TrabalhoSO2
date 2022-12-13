#include <tchar.h>
#include <math.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <windows.h>
#include <time.h>
#include "..\Estruturas\estruturas.h"

#define PIPE_NAME TEXT("\\\\.\\pipe\\teste")

int num_aleatorio(int min, int max) {
    return rand() % (max - min + 1) + min;
}

void createKEY(HKEY* chave, TCHAR* caminho, DWORD* tamMap, DWORD* tempAgua) {
    if (RegCreateKeyEx(HKEY_CURRENT_USER, caminho, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &chave, NULL) != ERROR_SUCCESS) {
        DWORD erro = GetLastError();

        _tprintf(TEXT("ERRO %d a criar a chave\n"), erro);
        return -1;
    }

    RegSetValueEx(chave, _T("tamMap"), 0, REG_DWORD, (LPBYTE)tamMap, sizeof(DWORD));
    RegSetValueEx(chave, _T("tempAgua"), 0, REG_DWORD, (LPBYTE)tempAgua, sizeof(DWORD));

}

BOOL checkKey(HKEY chave, TCHAR* caminho) {
    if (RegOpenKey(HKEY_CURRENT_USER, caminho, &chave) != ERROR_SUCCESS)
        return FALSE;
    else
        return TRUE;
}

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

void copiaRegistry(HKEY* chave, TCHAR* caminho, DWORD* tamMap, DWORD* tempAgua) {

    if (RegOpenKeyEx(HKEY_CURRENT_USER, caminho, 0, KEY_ALL_ACCESS, &chave) != ERROR_SUCCESS) {
        _tprintf(_T("\nErro a abir a chave!\n"));
        return -1;
    }

    DWORD size = 257 * sizeof(DWORD);
    DWORD dtype = REG_DWORD;

    RegQueryValueEx(chave, _T("tamMap"), NULL, &dtype, (LPBYTE)tamMap, &size);
    RegQueryValueEx(chave, _T("tempAgua"), NULL, &dtype, (LPBYTE)tempAgua, &size);
}

void origemDestinoAgua(int* origemX, int* origemY, int* destinoX, int* destinoY, int tamMap) {
    srand((unsigned int)time(NULL));

    do {
        *origemX = num_aleatorio(0, tamMap - 1);
        *origemY = num_aleatorio(0, tamMap - 1);

    } while (*origemX != 0 && *origemX != tamMap - 1 && *origemY != 0 && *origemY != tamMap - 1);

    do {
        if (*origemX < tamMap / 2)
            *destinoX = num_aleatorio(tamMap / 2, tamMap - 1);
        else
            *destinoX = num_aleatorio(0, (tamMap / 2) - 1);

        if (*origemY < tamMap / 2)
            *destinoY = num_aleatorio(tamMap / 2, tamMap - 1);
        else
            *destinoY = num_aleatorio(0, (tamMap / 2) - 1);

    } while (*destinoX != 0 && *destinoX != tamMap - 1 && *destinoY != 0 && *destinoY != tamMap - 1);

}

void iniciaJogo(DadosThreads* dados) {

    int origemX, origemY, destinoX, destinoY;
    origemDestinoAgua(&origemX, &origemY, &destinoX, &destinoY, dados->memPar->mapaJogo.tamMap);
    dados->memPar->mapaJogo.inicioX = origemX;
    dados->memPar->mapaJogo.inicioY = origemY;
    dados->memPar->mapaJogo.destinoX = destinoX;
    dados->memPar->mapaJogo.destinoY = destinoY;

    for (int i = 0; i < dados->memPar->mapaJogo.tamMap; i++) {
        for (int j = 0; j < dados->memPar->mapaJogo.tamMap; j++) {
            dados->memPar->mapaJogo.mapa[i][j].tubos = _T(' ');
            dados->memPar->mapaJogo.mapa[i][j].agua = FALSE;
        }
    }

    dados->memPar->mapaJogo.mapa[dados->memPar->mapaJogo.inicioX][dados->memPar->mapaJogo.inicioY].tubos = _T('I');
    dados->memPar->mapaJogo.mapa[dados->memPar->mapaJogo.destinoX][dados->memPar->mapaJogo.destinoY].tubos = _T('D');

    for (int i = 0; i < dados->nClientes; i++) {

        dados->cli[i].pecas[0] = _T('━'); //img5
        dados->cli[i].pecas[1] = _T('┃'); //img6
        dados->cli[i].pecas[2] = _T('┏'); //img2
        dados->cli[i].pecas[3] = _T('┓'); //img1 
        dados->cli[i].pecas[4] = _T('┛'); //img3 
        dados->cli[i].pecas[5] = _T('┗'); //img4

        dados->cli[i].pecas[6] = _T('═'); //img11
        dados->cli[i].pecas[7] = _T('║'); //img12
        dados->cli[i].pecas[8] = _T('╔'); //img8
        dados->cli[i].pecas[9] = _T('╗'); //img7 
        dados->cli[i].pecas[10] = _T('╝'); //img9
        dados->cli[i].pecas[11] = _T('╚'); //img10

        dados->cli[i].pecas[12] = _T('■');

        dados->cli[i].ordem[0] = _T('━'); //img5
        dados->cli[i].ordem[1] = _T('┃'); //img6
        dados->cli[i].ordem[2] = _T('┏'); //img2
        dados->cli[i].ordem[3] = _T('┓'); //img1 
        dados->cli[i].ordem[4] = _T('┛'); //img3 
        dados->cli[i].ordem[5] = _T('┗'); //img4
    }

    dados->memPar->mapaJogo.mapa[dados->memPar->mapaJogo.inicioX][dados->memPar->mapaJogo.inicioY].tubos = _T('I');
    dados->memPar->mapaJogo.mapa[dados->memPar->mapaJogo.destinoX][dados->memPar->mapaJogo.destinoY].tubos = _T('D');

    _tprintf(_T("\nPosicao de origem da agua -> [%d] [%d]\n"), origemX, origemY);
    _tprintf(_T("Posicao de destino da agua -> [%d] [%d]\n\n"), destinoX, destinoY);

    SetEvent(dados->atualizaJogo);

    /*dados->memPar->mapaJogo.tamMap = 10;
    dados->memPar->mapaJogo.inicioX = 8;
    dados->memPar->mapaJogo.inicioY = 0;
    dados->memPar->mapaJogo.destinoX = 2;
    dados->memPar->mapaJogo.destinoY = 9;

    for (int i = 0; i < dados->memPar->mapaJogo.tamMap; i++) {
        for (int j = 0; j < dados->memPar->mapaJogo.tamMap; j++) {
            dados->memPar->mapaJogo.mapa[i][j].tubos = _T(' ');
            dados->memPar->mapaJogo.mapa[i][j].agua = FALSE;
        }
    }

    dados->memPar->mapaJogo.mapa[dados->memPar->mapaJogo.inicioX][dados->memPar->mapaJogo.inicioY].tubos = _T('I');
    dados->memPar->mapaJogo.mapa[dados->memPar->mapaJogo.destinoX][dados->memPar->mapaJogo.destinoY].tubos = _T('D');

    dados->memPar->mapaJogo.mapa[8][1].tubos = _T('━');
    dados->memPar->mapaJogo.mapa[8][2].tubos = _T('━');
    dados->memPar->mapaJogo.mapa[8][3].tubos = _T('━');
    dados->memPar->mapaJogo.mapa[8][4].tubos = _T('┛');
    dados->memPar->mapaJogo.mapa[7][4].tubos = _T('┏');
    dados->memPar->mapaJogo.mapa[7][5].tubos = _T('━');
    dados->memPar->mapaJogo.mapa[7][6].tubos = _T('┛');
    dados->memPar->mapaJogo.mapa[6][6].tubos = _T('┃');
    dados->memPar->mapaJogo.mapa[5][6].tubos = _T('┓');
    dados->memPar->mapaJogo.mapa[5][5].tubos = _T('━');
    dados->memPar->mapaJogo.mapa[5][4].tubos = _T('┗');
    dados->memPar->mapaJogo.mapa[4][4].tubos = _T('┃');
    dados->memPar->mapaJogo.mapa[3][4].tubos = _T('┃');
    dados->memPar->mapaJogo.mapa[2][4].tubos = _T('┏');
    dados->memPar->mapaJogo.mapa[2][5].tubos = _T('━');
    dados->memPar->mapaJogo.mapa[2][6].tubos = _T('━');
    dados->memPar->mapaJogo.mapa[2][7].tubos = _T('━');
    dados->memPar->mapaJogo.mapa[2][8].tubos = _T('━');

    /*dados->memPar->mapaJogo.mapa[0][1].tubos = _T('┓');
    dados->memPar->mapaJogo.mapa[1][1].tubos = _T('┃');
    dados->memPar->mapaJogo.mapa[2][1].tubos = _T('┗');
    dados->memPar->mapaJogo.mapa[2][2].tubos = _T('━');
    dados->memPar->mapaJogo.mapa[2][3].tubos = _T('┓');
    dados->memPar->mapaJogo.mapa[3][3].tubos = _T('┗');
    dados->memPar->mapaJogo.mapa[3][4].tubos = _T('┓');
    dados->memPar->mapaJogo.mapa[4][4].tubos = _T('┗');
    dados->memPar->mapaJogo.mapa[4][5].tubos = _T('━');
    dados->memPar->mapaJogo.mapa[4][6].tubos = _T('┓');
    dados->memPar->mapaJogo.mapa[5][6].tubos = _T('┃');
    dados->memPar->mapaJogo.mapa[6][6].tubos = _T('┃');
    dados->memPar->mapaJogo.mapa[7][6].tubos = _T('┗');
    dados->memPar->mapaJogo.mapa[7][7].tubos = _T('┓');
    dados->memPar->mapaJogo.mapa[8][7].tubos = _T('┗');
    dados->memPar->mapaJogo.mapa[8][8].tubos = _T('━');


    dados->msgCli.pecas[0] = _T('━'); //img5
    dados->msgCli.pecas[1] = _T('┃'); //img6
    dados->msgCli.pecas[2] = _T('┏'); //img2
    dados->msgCli.pecas[3] = _T('┓'); //img1
    dados->msgCli.pecas[4] = _T('┛'); //img3
    dados->msgCli.pecas[5] = _T('┗'); //img4

    dados->msgCli.pecas[6] = _T('═'); //img11
    dados->msgCli.pecas[7] = _T('║'); //img12
    dados->msgCli.pecas[8] = _T('╔'); //img8
    dados->msgCli.pecas[9] = _T('╗'); //img7
    dados->msgCli.pecas[10] = _T('╝'); //img9
    dados->msgCli.pecas[11] = _T('╚'); //img10

    _tprintf(_T("\nPosicao de origem da agua -> [%d] [%d]\n"), dados->memPar->mapaJogo.inicioX, dados->memPar->mapaJogo.inicioY);
    _tprintf(_T("Posicao de destino da agua -> [%d] [%d]\n"), dados->memPar->mapaJogo.destinoX, dados->memPar->mapaJogo.destinoY);

    //mapaJogo.mapa[0][1].tubos = _T('┛');
   ////mapaJogo.mapa[0][2].tubos= _T('┏');*/

}

void meteAgua(DadosThreads* dados, int x, int y) {
    if (dados->memPar->mapaJogo.mapa[x][y].tubos == _T('━'))
        dados->memPar->mapaJogo.mapa[x][y].tubos = _T('═');
    else  if (dados->memPar->mapaJogo.mapa[x][y].tubos == _T('┃'))
        dados->memPar->mapaJogo.mapa[x][y].tubos = _T('║');
    else  if (dados->memPar->mapaJogo.mapa[x][y].tubos == _T('┏'))
        dados->memPar->mapaJogo.mapa[x][y].tubos = _T('╔');
    else  if (dados->memPar->mapaJogo.mapa[x][y].tubos == _T('┓'))
        dados->memPar->mapaJogo.mapa[x][y].tubos = _T('╗');
    else  if (dados->memPar->mapaJogo.mapa[x][y].tubos == _T('┛'))
        dados->memPar->mapaJogo.mapa[x][y].tubos = _T('╝');
    else  if (dados->memPar->mapaJogo.mapa[x][y].tubos == _T('┗'))
        dados->memPar->mapaJogo.mapa[x][y].tubos = _T('╚');

    SetEvent(dados->atualizaJogo);
}

void mudaNivel(DadosThreads* dados) {
    dados->cli[0].nivel++;
    int origemX, origemY, destinoX, destinoY;
    origemDestinoAgua(&origemX, &origemY, &destinoX, &destinoY, dados->memPar->mapaJogo.tamMap);
    dados->memPar->mapaJogo.inicioX = origemX;
    dados->memPar->mapaJogo.inicioY = origemY;
    dados->memPar->mapaJogo.destinoX = destinoX;
    dados->memPar->mapaJogo.destinoY = destinoY;

    for (int i = 0; i < dados->memPar->mapaJogo.tamMap; i++) {
        for (int j = 0; j < dados->memPar->mapaJogo.tamMap; j++) {
            dados->memPar->mapaJogo.mapa[i][j].tubos = _T(' ');
            dados->memPar->mapaJogo.mapa[i][j].agua = FALSE;
        }
    }

    dados->memPar->mapaJogo.mapa[dados->memPar->mapaJogo.inicioX][dados->memPar->mapaJogo.inicioY].tubos = _T('I');
    dados->memPar->mapaJogo.mapa[dados->memPar->mapaJogo.destinoX][dados->memPar->mapaJogo.destinoY].tubos = _T('D');

    _tprintf(_T("\nPosicao de origem da agua -> [%d] [%d]\n"), origemX, origemY);
    _tprintf(_T("Posicao de destino da agua -> [%d] [%d]\n"), destinoX, destinoY);

    dados->cli[0].hover = 0;

    SetEvent(dados->atualizaJogo);
}

DWORD WINAPI ThreadAgua(LPVOID param) {
    DadosThreads* dados = (DadosThreads*)param;
    CelulaBuffer cel;
    DWORD posAntesX = -1, posAntesY = -1;
    BOOL primeira = TRUE;
    BOOL existe = FALSE, muda = FALSE;

    Sleep(dados->memPar->mapaJogo.tempAgua);
    //fazer sleep com mais uma variavel

    while (!dados->terminar) {
        existe = FALSE;


        if (dados->memPar->mapaJogo.paraAguaMon != 0) {
            Sleep(dados->memPar->mapaJogo.paraAguaMon);
            dados->memPar->mapaJogo.paraAguaMon = 0;
        }

        if (dados->memPar->mapaJogo.paraAgua == 2 || dados->memPar->mapaJogo.paraAgua == 3) {
            while (dados->memPar->mapaJogo.paraAgua == 2 || dados->memPar->mapaJogo.paraAgua == 3);
        }

        if (!primeira) {

            if (posAntesX >= 1 && existe != TRUE) {

                if (dados->memPar->mapaJogo.mapa[posAntesX - 1][posAntesY].tubos != _T(' ')) {

                    if ((dados->memPar->mapaJogo.mapa[posAntesX][posAntesY].tubos == _T('║') ||
                        dados->memPar->mapaJogo.mapa[posAntesX][posAntesY].tubos == _T('╝') ||
                        dados->memPar->mapaJogo.mapa[posAntesX][posAntesY].tubos == _T('╚') ||
                        dados->memPar->mapaJogo.mapa[posAntesX][posAntesY].tubos == _T('I')) &&
                        (dados->memPar->mapaJogo.mapa[posAntesX - 1][posAntesY].tubos == _T('┃') ||
                            dados->memPar->mapaJogo.mapa[posAntesX - 1][posAntesY].tubos == _T('┏') ||
                            dados->memPar->mapaJogo.mapa[posAntesX - 1][posAntesY].tubos == _T('┓') ||
                            dados->memPar->mapaJogo.mapa[posAntesX - 1][posAntesY].tubos == _T('D'))) {

                        dados->memPar->mapaJogo.mapa[posAntesX - 1][posAntesY].agua = TRUE;
                        meteAgua(dados, posAntesX - 1, posAntesY);
                        existe = TRUE;
                        posAntesX = posAntesX - 1;
                        posAntesY = posAntesY;
                    }

                }
            }
            if (posAntesY >= 1 && existe != TRUE) {
                if (dados->memPar->mapaJogo.mapa[posAntesX][posAntesY - 1].tubos != _T(' ')) {
                    if ((dados->memPar->mapaJogo.mapa[posAntesX][posAntesY].tubos == _T('═') ||
                        dados->memPar->mapaJogo.mapa[posAntesX][posAntesY].tubos == _T('╗') ||
                        dados->memPar->mapaJogo.mapa[posAntesX][posAntesY].tubos == _T('╝') ||
                        dados->memPar->mapaJogo.mapa[posAntesX][posAntesY].tubos == _T('I')) &&
                        (dados->memPar->mapaJogo.mapa[posAntesX][posAntesY - 1].tubos == _T('━') ||
                            dados->memPar->mapaJogo.mapa[posAntesX][posAntesY - 1].tubos == _T('┏') ||
                            dados->memPar->mapaJogo.mapa[posAntesX][posAntesY - 1].tubos == _T('┗') ||
                            dados->memPar->mapaJogo.mapa[posAntesX][posAntesY - 1].tubos == _T('D'))) {

                        dados->memPar->mapaJogo.mapa[posAntesX][posAntesY - 1].agua = TRUE;
                        existe = TRUE;
                        meteAgua(dados, posAntesX, posAntesY - 1);
                        posAntesX = posAntesX;
                        posAntesY = posAntesY - 1;
                    }

                }
            }

            if (posAntesX + 1 < dados->memPar->mapaJogo.tamMap && existe != TRUE) {

                if (dados->memPar->mapaJogo.mapa[posAntesX + 1][posAntesY].tubos != _T(' ')) {

                    if ((dados->memPar->mapaJogo.mapa[posAntesX][posAntesY].tubos == _T('║') ||
                        dados->memPar->mapaJogo.mapa[posAntesX][posAntesY].tubos == _T('╔') ||
                        dados->memPar->mapaJogo.mapa[posAntesX][posAntesY].tubos == _T('╗') ||
                        dados->memPar->mapaJogo.mapa[posAntesX][posAntesY].tubos == _T('I')) &&
                        (dados->memPar->mapaJogo.mapa[posAntesX + 1][posAntesY].tubos == _T('┃') ||
                            dados->memPar->mapaJogo.mapa[posAntesX + 1][posAntesY].tubos == _T('┛') ||
                            dados->memPar->mapaJogo.mapa[posAntesX + 1][posAntesY].tubos == _T('┗') ||
                            dados->memPar->mapaJogo.mapa[posAntesX + 1][posAntesY].tubos == _T('D'))) {

                        dados->memPar->mapaJogo.mapa[posAntesX + 1][posAntesY].agua = TRUE;
                        meteAgua(dados, posAntesX + 1, posAntesY);
                        existe = TRUE;
                        posAntesX = posAntesX + 1;
                        posAntesY = posAntesY;
                    }

                }
            }

            if (posAntesY + 1 < dados->memPar->mapaJogo.tamMap && existe != TRUE) {

                if (dados->memPar->mapaJogo.mapa[posAntesX][posAntesY + 1].tubos != _T(' ')) {

                    if ((dados->memPar->mapaJogo.mapa[posAntesX][posAntesY].tubos == _T('═') ||
                        dados->memPar->mapaJogo.mapa[posAntesX][posAntesY].tubos == _T('╔') ||
                        dados->memPar->mapaJogo.mapa[posAntesX][posAntesY].tubos == _T('╚') ||
                        dados->memPar->mapaJogo.mapa[posAntesX][posAntesY].tubos == _T('I')) &&
                        (dados->memPar->mapaJogo.mapa[posAntesX][posAntesY + 1].tubos == _T('━') ||
                            dados->memPar->mapaJogo.mapa[posAntesX][posAntesY + 1].tubos == _T('┓') ||
                            dados->memPar->mapaJogo.mapa[posAntesX][posAntesY + 1].tubos == _T('┛') ||
                            dados->memPar->mapaJogo.mapa[posAntesX][posAntesY + 1].tubos == _T('D'))) {

                        dados->memPar->mapaJogo.mapa[posAntesX][posAntesY + 1].agua = TRUE;
                        meteAgua(dados, posAntesX, posAntesY + 1);
                        existe = TRUE;
                        posAntesX = posAntesX;
                        posAntesY = posAntesY + 1;
                    }

                }
            }

            if (posAntesX == dados->memPar->mapaJogo.destinoX && posAntesY == dados->memPar->mapaJogo.destinoY) {
                _tprintf(_T("\nO jogador GANHOU!\nProximo Nivel >> %d\n"), dados->cli[0].nivel + 1);
                //dados->terminar = 1;
                mudaNivel(dados);
                muda = TRUE;
            }
            else if (!existe) {
                _tprintf(_T("\nO jogador PERDEU!\n"));
                dados->terminar = 1;
                SetEvent(dados->comecaCliente);
            }

        }

        if (primeira) {
            for (int i = 0; i < dados->memPar->mapaJogo.tamMap; i++) {
                for (int j = 0; j < dados->memPar->mapaJogo.tamMap; j++) {
                    if (dados->memPar->mapaJogo.mapa[i][j].tubos == _T('I')) {
                        dados->memPar->mapaJogo.mapa[i][j].agua = TRUE;
                        posAntesX = i;
                        posAntesY = j;
                    }

                }
            }
            primeira = FALSE;
        }

        if (muda) {
            primeira = TRUE;
            muda = FALSE;
        }

        Sleep((4 - dados->cli[0].nivel) * 1000);
    }

    return 0;
}

void gereComandos(DadosThreads* dados, Comandos* cel) {
    if (cel->pararAgua[0] == 1) {
        _tprintf(_T("\n*Comando recebido*\n"));
        _tprintf(_T("Parar agua durante %ds\n"), cel->pararAgua[1] / 1000);
        /*SuspendThread(getcurrentthr);
        Sleep(cel->pararAgua[1]);
        ResumeThread(thread);*/
        dados->memPar->mapaJogo.paraAguaMon = cel->pararAgua[1];
    }
    else if (cel->modoAleatorio == 1) {
        _tprintf(_T("\n*Comando recebido*: modo aleatorio\n"));

        dados->memPar->mapaJogo.aleatorio = !dados->memPar->mapaJogo.aleatorio;

        for (int i = 0; i < dados->nClientes; i++)
            dados->cli[i].aleatorio = dados->memPar->mapaJogo.aleatorio;

    }
    else if (cel->inserirParedes[0] == 1) {
        _tprintf(_T("\n*Comando recebido*\n"));
        _tprintf(_T("Inserir parede na linha %d, coluna %d\n"), cel->inserirParedes[1], cel->inserirParedes[2]);
        DWORD x, y;
        x = cel->inserirParedes[1]; y = cel->inserirParedes[2];
        dados->memPar->mapaJogo.mapa[x][y].tubos = _T('■');
    }

    SetEvent(dados->atualizaJogo);
}

DWORD WINAPI ThreadCriaPipes(LPVOID param) {
    DadosThreads* dados = (DadosThreads*)param;

    HANDLE hPipe, hEventTemp;
    DWORD offset, nBytes;
    int i;


    for (int i = 0; i < 2; i++) {

        // aqui passamos a constante FILE_FLAG_OVERLAPPED para o named pipe aceitar comunicações assincronas
        hPipe = CreateNamedPipe(PIPE_NAME, PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
            PIPE_WAIT | PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE,
            2,
            sizeof(dados->cli[0]),
            sizeof(dados->cli[0]),
            1000,
            NULL);

        if (hPipe == INVALID_HANDLE_VALUE) {
            _tprintf(TEXT("[ERRO] Criar Named Pipe! (CreateNamedPipe)"));
            exit(-1);
        }

        // criar evento que vai ser associado à esturtura overlaped
        // os eventos aqui tem de ter sempre reset manual e nao automático porque temos de delegar essas responsabilidades ao sistema operativo
        hEventTemp = CreateEvent(NULL, TRUE, FALSE, NULL);

        if (hEventTemp == NULL) {
            _tprintf(TEXT("[ERRO] ao criar evento\n"));
            return -1;
        }

        dados->hPipes[i].hPipe = hPipe;
        dados->hPipes[i].activo = FALSE;
        //temos de garantir que a estrutura overlap está limpa
        ZeroMemory(&dados->hPipes[i].overlap, sizeof(dados->hPipes[i].overlap));
        //preenchemos agora o evento
        dados->hPipes[i].overlap.hEvent = hEventTemp;
        dados->hEvents[i] = hEventTemp;

        // aqui passamos um ponteiro para a estrutura overlap
        ConnectNamedPipe(hPipe, &dados->hPipes[i].overlap);
    }

    while (!dados->terminar && dados->nClientes < 2) {
        //permite estar bloqueado , à espera que 1 evento do array de enventos seja assinalado
        offset = WaitForMultipleObjects(2, dados->hEvents, FALSE, INFINITE);
        i = offset - WAIT_OBJECT_0; // devolve o indice da instancia do named pipe que está ativa, aqui sabemos em que indice o cliente se ligou

        // se é um indice válido ...
        if (i >= 0 && i < 2) {

            _tprintf(TEXT("[Servidor] Cliente %d chegou\n"), i);
            if (GetOverlappedResult(dados->hPipes[i].hPipe, &dados->hPipes[i].overlap, &nBytes, FALSE)) {
                // se entrarmos aqui significa que a funcao correu tudo bem
                // fazemos reset do evento porque queremos que o WaitForMultipleObject desbloqueio com base noutro evento e nao neste
                ResetEvent(dados->hEvents[i]);

                //vamos esperar que o mutex esteja livre
                WaitForSingleObject(dados->hMutex, INFINITE);
                dados->hPipes[i].activo = TRUE; // dizemos que esta instancia do named pipe está ativa
                ReleaseMutex(dados->hMutex);

                dados->nClientes++;

                
               SetEvent(dados->comecaCliente);
                // if (i == 1)
                  //   SetEvent(dados->clienteIniciou2);
            }
        }
    }


    dados->terminar = 1;

}

void mexeTubos(DadosThreads* dados, int i) {

    if (dados->cli[i].clickTipo == 0) {
        if (dados->memPar->mapaJogo.mapa[dados->cli[i].linhaColuna[0]][dados->cli[i].linhaColuna[1]].tubos == _T(' ')) {

            if (dados->memPar->mapaJogo.aleatorio) {
                int pos;
                pos = num_aleatorio(0, 5);
                dados->memPar->mapaJogo.mapa[dados->cli[i].linhaColuna[0]][dados->cli[i].linhaColuna[1]].tubos = dados->cli[i].ordem[pos];

                _tprintf(TEXT("[Servidor] Jogador %d -> Peca %c inserida na linha %d x coluna %d\n"), dados->nClientes, dados->cli[i].ordem[pos], dados->cli[i].linhaColuna[0], dados->cli[i].linhaColuna[1]);

            }
            else {
                dados->memPar->mapaJogo.mapa[dados->cli[i].linhaColuna[0]][dados->cli[i].linhaColuna[1]].tubos = dados->cli[i].ordem[0];
                _tprintf(TEXT("[Servidor] Jogador %d -> Peca %c inserida na linha %d x coluna %d\n"), dados->nClientes, dados->cli[i].ordem[0], dados->cli[i].linhaColuna[0], dados->cli[i].linhaColuna[1]);

                TCHAR peca = dados->cli[i].ordem[0];                        // #1

                for (int j = 1; j < 6; j++) {        // #2
                    dados->cli[i].ordem[j - 1] = dados->cli[i].ordem[j];
                }

                dados->cli[i].ordem[5] = peca;

                //dados->msgCli.totalClicks++;
            }
        }

        else {
            if (!dados->memPar->mapaJogo.mapa[dados->cli[i].linhaColuna[0]][dados->cli[i].linhaColuna[1]].agua) {
                if (dados->memPar->mapaJogo.mapa[dados->cli[i].linhaColuna[0]][dados->cli[i].linhaColuna[1]].tubos == dados->cli[i].pecas[0])
                    dados->memPar->mapaJogo.mapa[dados->cli[i].linhaColuna[0]][dados->cli[i].linhaColuna[1]].tubos = dados->cli[i].pecas[1];
                else  if (dados->memPar->mapaJogo.mapa[dados->cli[i].linhaColuna[0]][dados->cli[i].linhaColuna[1]].tubos == dados->cli[i].pecas[1])
                    dados->memPar->mapaJogo.mapa[dados->cli[i].linhaColuna[0]][dados->cli[i].linhaColuna[1]].tubos = dados->cli[i].pecas[2];
                else  if (dados->memPar->mapaJogo.mapa[dados->cli[i].linhaColuna[0]][dados->cli[i].linhaColuna[1]].tubos == dados->cli[i].pecas[2])
                    dados->memPar->mapaJogo.mapa[dados->cli[i].linhaColuna[0]][dados->cli[i].linhaColuna[1]].tubos = dados->cli[i].pecas[3];
                else  if (dados->memPar->mapaJogo.mapa[dados->cli[i].linhaColuna[0]][dados->cli[i].linhaColuna[1]].tubos == dados->cli[i].pecas[3])
                    dados->memPar->mapaJogo.mapa[dados->cli[i].linhaColuna[0]][dados->cli[i].linhaColuna[1]].tubos = dados->cli[i].pecas[4];
                else  if (dados->memPar->mapaJogo.mapa[dados->cli[i].linhaColuna[0]][dados->cli[i].linhaColuna[1]].tubos == dados->cli[i].pecas[4])
                    dados->memPar->mapaJogo.mapa[dados->cli[i].linhaColuna[0]][dados->cli[i].linhaColuna[1]].tubos = dados->cli[i].pecas[5];
                else  if (dados->memPar->mapaJogo.mapa[dados->cli[i].linhaColuna[0]][dados->cli[i].linhaColuna[1]].tubos == dados->cli[i].pecas[5])
                    dados->memPar->mapaJogo.mapa[dados->cli[i].linhaColuna[0]][dados->cli[i].linhaColuna[1]].tubos = dados->cli[i].pecas[0];

                _tprintf(TEXT("[Servidor] Jogador %d -> Peca da linha %d x coluna %d rodou\n"), dados->nClientes, dados->cli[i].linhaColuna[0], dados->cli[i].linhaColuna[1]);

            }
        }
    }

    else  if (dados->cli[i].clickTipo == 1) {
        if (!dados->memPar->mapaJogo.mapa[dados->cli[i].linhaColuna[0]][dados->cli[i].linhaColuna[1]].agua && dados->memPar->mapaJogo.mapa[dados->cli[i].linhaColuna[0]][dados->cli[i].linhaColuna[1]].tubos != _T('I') && dados->memPar->mapaJogo.mapa[dados->cli[i].linhaColuna[0]][dados->cli[i].linhaColuna[1]].tubos != _T('D') && dados->memPar->mapaJogo.mapa[dados->cli[i].linhaColuna[0]][dados->cli[i].linhaColuna[1]].tubos != _T('■')) {
            dados->memPar->mapaJogo.mapa[dados->cli[i].linhaColuna[0]][dados->cli[i].linhaColuna[1]].tubos = _T(' ');
            _tprintf(TEXT("[Servidor] Jogador %d -> Peca da linha %d x coluna %d foi removida\n"), dados->nClientes, dados->cli[i].linhaColuna[0], dados->cli[i].linhaColuna[1]);
        }

    }

    else if (dados->cli[i].clickTipo == 2) {
        if (dados->memPar->mapaJogo.paraAgua == 0) {
            if (dados->cli[i].hover < 3) {
                dados->memPar->mapaJogo.paraAgua = 2;
                _tprintf(TEXT("[Servidor] Jogador %d -> Jogo suspenso\n"), dados->nClientes);
                dados->cli[i].hover++;
            }
        }
    }

    else if (dados->cli[i].clickTipo == 3) {
        if (dados->memPar->mapaJogo.paraAgua == 2) {
            dados->memPar->mapaJogo.paraAgua = 0;
            _tprintf(TEXT("[Servidor] Jogador %d -> Jogo retomado\n"), dados->nClientes);
        }
    }
    else if (dados->cli[i].clickTipo == 4) {
        _tprintf(TEXT("[Servidor] Jogador %d desconectou-se\n"), dados->nClientes);
        dados->hPipes[i].activo = FALSE;
        //ZeroMemory(dados->hPipes[i].hPipe, sizeof(dados->hPipes[i].hPipe));
        dados->nClientes--;
        if (dados->cli[i].clis != 0) {
            dados->terminar = 1;
            SetEvent(dados->comecaCliente);
        }
        ZeroMemory(&dados->cli[i], sizeof(dados->cli[i]));
    }

    SetEvent(dados->atualizaJogo);
}

DWORD WINAPI ThreadLeDoCliente(LPVOID param) {
    DadosThreads* dados = (DadosThreads*)param;
    HANDLE hPipe = dados->hPipes[dados->nClientes - 1].hPipe;
    int nr = dados->nClientes - 1;

    BOOL ret;
    DWORD n, offset, i;

    //WaitForSingleObject(dados->comecaCliente, INFINITE);
    //ResetEvent(dados->comecaCliente);

    WaitForSingleObject(dados->hMutex, INFINITE);

    //offset = WaitForMultipleObjects(2, dados->hEvents, FALSE, INFINITE);
    //i = offset - WAIT_OBJECT_0;
    if (dados->nClientes != 0) {

        ret = ReadFile(hPipe, &dados->cli[nr], sizeof(dados->cli[nr]), &n, NULL);
        if (dados->cli[nr].clickTipo == 4) {
            mexeTubos(dados, nr);
            return;
        }
        _tprintf(TEXT("\nO jogador %s escolheu o modo de jogo %s\n"), dados->cli[nr].nome, dados->cli[nr].tipoJogo);

        ReleaseMutex(dados->hMutex);

        if (_tccmp(dados->cli[nr].tipoJogo, _T("individual")) == 0) {
            _tprintf(TEXT("\n>>>>> Nivel %d <<<<<\n"), dados->cli[nr].nivel);
            _tprintf(_T("\nTamanho do mapa -> %d X %d\n"), dados->memPar->mapaJogo.tamMap, dados->memPar->mapaJogo.tamMap);
            _tprintf(_T("Tempo que a agua demora a comecar a correr : %ds\n"), dados->memPar->mapaJogo.tempAgua / 1000);
            iniciaJogo(dados);
            //SetEvent(dados->inicia);
            CreateThread(NULL, 0, ThreadAgua, dados, 0, NULL);
            SetEvent(dados->clienteEscreveu); //clienteComeca;
            //SetEvent(dados->atualizaJogo);
        }
        else {
            //SetEvent(dados->inicia);
            dados->cli[nr].clis++;
            _tprintf(_T("CLIS %d"), dados->cli[nr].clis);
            SetEvent(dados->clienteEscreveu); //clienteComeca;
        }

        while (!dados->terminar) {

            WaitForSingleObject(dados->leDoCliente, INFINITE); //CLIENTEESCREVEU
            ResetEvent(dados->leDoCliente); //CLIENTEESCREVEU

            WaitForSingleObject(dados->hMutex, INFINITE);

            ret = ReadFile(hPipe, &dados->cli[nr], sizeof(dados->cli[nr]), &n, NULL);
            if (dados->cli[nr].clickTipo == 4) {
                mexeTubos(dados, nr);
                return;
            }
            //_tprintf(TEXT("\nX -> %d | Y -> %d \n"), dados->cli[nr].linhaColuna[0], dados->cli[nr].linhaColuna[1]);

            if (dados->cli[nr].linhaColuna[0] >= 0 && dados->cli[nr].linhaColuna[0] < dados->memPar->mapaJogo.tamMap && dados->cli[nr].linhaColuna[1] >= 0 && dados->cli[nr].linhaColuna[1] < dados->memPar->mapaJogo.tamMap && dados->memPar->mapaJogo.mapa[dados->cli[nr].linhaColuna[0]][dados->cli[nr].linhaColuna[1]].tubos != 'I' && dados->memPar->mapaJogo.mapa[dados->cli[nr].linhaColuna[0]][dados->cli[nr].linhaColuna[1]].tubos != 'D')
                mexeTubos(dados, nr);
            //dados->memPar->mapaJogo.mapa[dados->msgCli.linhaColuna[0]][dados->msgCli.linhaColuna[1]].tubos = dados->msgCli.pecas[0];

            ReleaseMutex(dados->hMutex);

        }
    }

}


DWORD WINAPI ThreadEscreveNoCliente(LPVOID param) {
    DadosThreads* dados = (DadosThreads*)param;
    BOOL ret;
    TCHAR buf[256];
    DWORD n;

    WaitForSingleObject(dados->clienteEscreveu, INFINITE); //clienteComeca;

    while (!dados->terminar) {

        for (int i = 0; i < dados->nClientes; i++) {
            //_tprintf(TEXT("[ERRO] Escrever no pipe! (WriteFile)1\n"));
            if (dados->hPipes[i].activo) {

                WaitForSingleObject(dados->hMutex, INFINITE);

                dados->cli[i].mapaJogo = dados->memPar->mapaJogo;

                dados->cli[i].clis = dados->nClientes;

                if (!WriteFile(dados->hPipes[i].hPipe, (LPVOID)&dados->cli[i], sizeof(dados->cli[i]), &n, NULL)) {
                    exit(-1);
                }

                ReleaseMutex(dados->hMutex);
            }
        }
        //ResetEvent(dados->clienteEscreveu);

    }
}


DWORD WINAPI ThreadServidor(LPVOID param) {
    DadosThreads* dados = (DadosThreads*)param;
    Comandos cel;

    while (!dados->terminar) {
        //esperamos por uma posicao para lermos
        WaitForSingleObject(dados->hSemLeitura, INFINITE);

        //vamos copiar da proxima posicao de leitura do buffer circular para a nossa variavel cel
        //CopyMemory(&cel, &dados->memPar->buffer[dados->memPar->posL], sizeof(CelulaBuffer));
        CopyMemory(&cel, &dados->memPar->comandos[dados->memPar->posL], sizeof(Comandos));
        dados->memPar->posL++; //incrementamos a posicao de leitura para o proximo consumidor ler na posicao seguinte

        gereComandos(dados, &cel);

        //se apos o incremento a posicao de leitura chegar ao fim, tenho de voltar ao inicio
        if (dados->memPar->posL == TAM_BUFFER)
            dados->memPar->posL = 0;

        //libertamos o semaforo. temos de libertar uma posicao de escrita
        ReleaseSemaphore(dados->hSemEscrita, 1, NULL);

    }

    return 0;
}

DWORD WINAPI ThreadTeclado(LPVOID param) {
    DadosThreads* dados = (DadosThreads*)param;
    TCHAR comando[100];
    HANDLE hThreadAgua;

    do {
        _fgetts(comando, 100, stdin);
        comando[_tcslen(comando) - 1] = '\0';

        /*if (_tcscmp(comando, _T("iniciar")) == 0) {
            SetEvent(dados->inicia);
            iniciaJogo(dados);
            _tprintf(_T("\nTamanho do mapa -> %d X %d\n"), dados->memPar->mapaJogo.tamMap, dados->memPar->mapaJogo.tamMap);
            _tprintf(_T("Tempo que a agua demora a comecar a correr : %ds\n\n"), dados->memPar->mapaJogo.tempAgua / 1000);

            //ResumeThread(hThreads[1]);

        }*/

        if (_tcscmp(comando, _T("lista")) == 0) {
            if(dados->cli[0].clis == 0)
                _tprintf(_T("\nNao existem utilizadores a jogar neste momento!\n"));

            for (int i = 0; i < dados->cli[0].clis; i++) {
                if(dados->hPipes[i].activo)
                    _tprintf(_T("\nJogador-> %s | Nivel -> %d\n"), dados->cli[i].nome, dados->cli[i].nivel);
            }
        }
        else if (_tcscmp(comando, _T("suspende")) == 0) {
            if (dados->cli[0].clis == 0)
                _tprintf(_T("\nNao existe nenhum jogo a decorrer!\n"));
            else if (dados->memPar->mapaJogo.paraAgua == 0 && dados->cli[0].clis != 0) {
                dados->memPar->mapaJogo.paraAgua = 3;
                _tprintf(TEXT("[Servidor] Jogo suspenso\n"), dados->nClientes);
            }
        }
        else if (_tcscmp(comando, _T("retoma")) == 0) {
            if (dados->cli[0].clis == 0)
                _tprintf(_T("\nNao existe nenhum jogo a decorrer!\n"));
            else if ((dados->memPar->mapaJogo.paraAgua == 2 || dados->memPar->mapaJogo.paraAgua == 3) && dados->cli[0].clis != 0) {
                dados->memPar->mapaJogo.paraAgua = 0;
                _tprintf(TEXT("[Servidor] Jogo retomado\n"), dados->nClientes);
            }
        }

    } while (_tcscmp(comando, _T("encerra")) != 0 && !dados->terminar);

    dados->terminar = 1;
    SetEvent(dados->comecaCliente);

    /*if (dados->cli[0].clis == 0) {
        SetEvent(dados->comecaCliente);
    }
    dados->terminar = 1;*/

    return 0;
}

int _tmain(int argc, TCHAR* argv[])
{
    HANDLE hServidor; //handle para o semaforo
    HANDLE hFileMap; //handle para o file map
    HANDLE hThreads[7];
    HANDLE hPipe;
    DadosThreads dados;
    BOOL primeiroProcesso = FALSE, ret;
    TCHAR comando[100], buf[256];
    DWORD tamMap;
    DWORD tempAgua, n;
    HKEY chave;

#ifdef UNICODE
    _setmode(_fileno(stdin), _O_WTEXT);
    _setmode(_fileno(stdout), _O_WTEXT);
    _setmode(_fileno(stderr), _O_WTEXT);
#endif

    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);

    srand((unsigned int)time(NULL));

    // Semaforo que controla o nr de instancias de servidores
    hServidor = CreateSemaphore(NULL, 1, 1, TEXT("SEMAPHORE_UNIQUE_SERVIDOR_NAME"));

    // Se ERROR_ALREADY_EXISTS , entao nao deixa iniciar outro servidor
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        _ftprintf(stderr, TEXT("[ERRO] Já existe um servidor aberto!\n"));
        return -1;
    }

    //criar semaforo que conta as escritas -> TAM_BUFFER posicoes vazias inicialmente, onde se pode escrever
    dados.hSemEscrita = CreateSemaphore(NULL, TAM_BUFFER, TAM_BUFFER, TEXT("SO2_SEMAFORO_ESCRITA"));

    //criar semaforo que conta as leituras
    //0 porque nao ha nada para ser lido e depois podemos ir até um maximo de 20 posicoes para serem lidas
    dados.hSemLeitura = CreateSemaphore(NULL, 0, TAM_BUFFER, TEXT("SO2_SEMAFORO_LEITURA"));

    //criar mutex para os produtores
    dados.hMutex = CreateMutex(NULL, FALSE, TEXT("SO2_MUTEX_Servidor"));

    if (dados.hMutex == NULL) {
        _tprintf(TEXT("Erro no CreateMutex\n"));
        return -1;
    }

    if (dados.hSemEscrita == NULL || dados.hSemLeitura == NULL) {
        _tprintf(TEXT("Erro no CreateSemaphore\n"));
        return -1;
    }

    if (argc < 3) // se nao forem passados os argumentos suficientes, vou ver se a chave existe
    {
        if (!checkKey(&chave, _T("Software\\TPSO2"))) { //FALSE ->  NAO EXISTE
            _tprintf(_T("\nA CHAVE nao existe e nao foram passados argumentos suficientes!\n"));
            return -1;
        }
        else //TRUE -> EXISTE ---> IR BUSCAR OS VALORES DE tamMap e tempAgua
            copiaRegistry(&chave, _T("Software\\TPSO2"), &tamMap, &tempAgua);
    }
    else { // foram passados os valores do mapa e do tempo da agua
        tamMap = _wtoi(argv[1]);
        tempAgua = _wtoi(argv[2]);
    }

    criaFileMap(&hFileMap, &primeiroProcesso, _T("SO2_MEM_PARTILHADA"));

    //mapeamos o bloco de memoria para o espaco de endera�amento do nosso processo
    dados.memPar = (BufferCircular*)MapViewOfFile(hFileMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);

    if (dados.memPar == NULL) {
        _tprintf(TEXT("Erro no MapViewOfFile\n"));
        return -1;
    }

    if (tamMap > 20) {
        _tprintf(_T("Tamanho do mapa invalido!"));
        return -1;
    }

    dados.memPar->mapaJogo.tamMap = tamMap;
    dados.memPar->mapaJogo.tempAgua = tempAgua;

    if (primeiroProcesso == TRUE) {
        dados.memPar->nMonitores = 0;
        dados.nClientes = 0;
        dados.memPar->posE = 0;
        dados.memPar->posL = 0;

        dados.terminar = 0;
        dados.cli[0].clis = 0;
        dados.cli[2].clis = 0;

    }

    dados.inicia = CreateEvent(NULL, //A pointer to a SECURITY_ATTRIBUTES structure. If this parameter is NULL, the handle cannot be inherited by child processes
        FALSE,//If this parameter is TRUE, the function creates a manual-reset event object, which requires the use of the ResetEvent function to set the event state to nonsignaled
        //If this parameter is FALSE, the function creates an auto-reset event object, and system automatically resets the event state to nonsignaled after a single waiting thread has been released. 
        FALSE, //If this parameter is TRUE, the initial state of the event object is signaled; otherwise, it is nonsignaled.
        NULL); //optional the name of the event object

    dados.acabou = CreateEvent(NULL, //A pointer to a SECURITY_ATTRIBUTES structure. If this parameter is NULL, the handle cannot be inherited by child processes
        TRUE,//If this parameter is TRUE, the function creates a manual-reset event object, which requires the use of the ResetEvent function to set the event state to nonsignaled
        //If this parameter is FALSE, the function creates an auto-reset event object, and system automatically resets the event state to nonsignaled after a single waiting thread has been released. 
        FALSE, //If this parameter is TRUE, the initial state of the event object is signaled; otherwise, it is nonsignaled.
        _T("SO2_SERVIDOR_ACABOU")); //optional the name of the event object

    ResetEvent(dados.acabou);

    dados.comecaCliente = CreateEvent(NULL, //A pointer to a SECURITY_ATTRIBUTES structure. If this parameter is NULL, the handle cannot be inherited by child processes
        TRUE,//If this parameter is TRUE, the function creates a manual-reset event object, which requires the use of the ResetEvent function to set the event state to nonsignaled
        //If this parameter is FALSE, the function creates an auto-reset event object, and system automatically resets the event state to nonsignaled after a single waiting thread has been released. 
        FALSE, //If this parameter is TRUE, the initial state of the event object is signaled; otherwise, it is nonsignaled.
        _T("SO2_CLIENTE_COMECOU")); //optional the name of the event object

    dados.leDoCliente = CreateEvent(NULL, //A pointer to a SECURITY_ATTRIBUTES structure. If this parameter is NULL, the handle cannot be inherited by child processes
        TRUE,//If this parameter is TRUE, the function creates a manual-reset event object, which requires the use of the ResetEvent function to set the event state to nonsignaled
        //If this parameter is FALSE, the function creates an auto-reset event object, and system automatically resets the event state to nonsignaled after a single waiting thread has been released. 
        FALSE, //If this parameter is TRUE, the initial state of the event object is signaled; otherwise, it is nonsignaled.
        _T("SO2_LE_CLIENTE")); //optional the name of the event object

    dados.clienteEscreveu = CreateEvent(NULL, //A pointer to a SECURITY_ATTRIBUTES structure. If this parameter is NULL, the handle cannot be inherited by child processes
        FALSE,//If this parameter is TRUE, the function creates a manual-reset event object, which requires the use of the ResetEvent function to set the event state to nonsignaled
        //If this parameter is FALSE, the function creates an auto-reset event object, and system automatically resets the event state to nonsignaled after a single waiting thread has been released. 
        FALSE, //If this parameter is TRUE, the initial state of the event object is signaled; otherwise, it is nonsignaled.
        NULL); //optional the name of the event object

    dados.clienteIniciou2 = CreateEvent(NULL, //A pointer to a SECURITY_ATTRIBUTES structure. If this parameter is NULL, the handle cannot be inherited by child processes
        TRUE,//If this parameter is TRUE, the function creates a manual-reset event object, which requires the use of the ResetEvent function to set the event state to nonsignaled
        //If this parameter is FALSE, the function creates an auto-reset event object, and system automatically resets the event state to nonsignaled after a single waiting thread has been released. 
        FALSE, //If this parameter is TRUE, the initial state of the event object is signaled; otherwise, it is nonsignaled.
        _T("SO2_CLIENTE2_COMECOU")); //optional the name of the event object

    ResetEvent(dados.clienteIniciou2);

    dados.atualizaJogo = CreateEvent(NULL, //A pointer to a SECURITY_ATTRIBUTES structure. If this parameter is NULL, the handle cannot be inherited by child processes
        TRUE,//If this parameter is TRUE, the function creates a manual-reset event object, which requires the use of the ResetEvent function to set the event state to nonsignaled
        //If this parameter is FALSE, the function creates an auto-reset event object, and system automatically resets the event state to nonsignaled after a single waiting thread has been released. 
        FALSE, //If this parameter is TRUE, the initial state of the event object is signaled; otherwise, it is nonsignaled.
        _T("SO2_ATUALIZA_JOGO")); //optional the name of the event object

    ResetEvent(dados.atualizaJogo);


    //lancamos a thread

    hThreads[0] = CreateThread(NULL, 0, ThreadServidor, &dados, 0, NULL);
    if (hThreads[0] == NULL) {
        _tprintf(_T("Erro ao criar a thread Servidor!"));
        return -1;
    }

    hThreads[1] = CreateThread(NULL, 0, ThreadAgua, &dados, CREATE_SUSPENDED, NULL);
    if (hThreads[1] == NULL) {
        _tprintf(_T("Erro ao criar a thread Agua!"));
        return -1;
    }

    hThreads[2] = CreateThread(NULL, 0, ThreadTeclado, &dados, 0, NULL);
    if (hThreads[2] == NULL) {
        _tprintf(_T("Erro ao criar a thread Agua!"));
        return -1;
    }

    hThreads[3] = CreateThread(NULL, 0, ThreadCriaPipes, &dados, 0, NULL);
    if (hThreads[3] == NULL) {
        _tprintf(_T("Erro ao criar a thread Pipes!"));
        return -1;
    }

    _tprintf(_T("  --> Jogo dos Tubos (Servidor) <--  \n\n"));

    //_tprintf(TEXT("\nEscreva 'iniciar' para comecar...\n"));

    _tprintf(TEXT("Esperarando jogador...\n"));

    while (!dados.terminar &&  dados.nClientes < 2) {

    //espera um cliente se connectar 
    WaitForSingleObject(dados.comecaCliente, INFINITE); //clienteEscreveu
    ResetEvent(dados.comecaCliente); ////clienteEscreveu

        CreateThread(NULL, 0, ThreadLeDoCliente, &dados, 0, NULL);
        

       CreateThread(NULL, 0, ThreadEscreveNoCliente, &dados, 0, NULL);
        

        /*hThreads[6] = CreateThread(NULL, 0, ThreadLeDoCliente2, &dados, 0, NULL);
        if (hThreads[6] == NULL) {
            _tprintf(_T("Erro ao criar a thread Agua!"));
            return -1;
        }*/

       //CreateThread(NULL, 0, ThreadAgua, &dados, 0, NULL);
     
       // WaitForSingleObject(dados.inicia, INFINITE);
        //ResumeThread(hThreads[1]);

    }

    WaitForMultipleObjects(7, hThreads, FALSE, INFINITE);

    tamMap = dados.memPar->mapaJogo.tamMap;
    tempAgua = dados.memPar->mapaJogo.tempAgua;
    createKEY(&chave, _T("Software\\TPSO2"), &tamMap, &tempAgua);

    _tprintf(TEXT("[SERVIDOR] A terminar...\n"));
    for (int i = 0; i < dados.nClientes; i++) {
        _tprintf(TEXT("[SERVIDOR] Desligar o pipe (DisconnectNamedPipe)\n"));
        //desliga a instancia ao named pipe
        /*if (!DisconnectNamedPipe(dados.hPipe[i])) {
            _tprintf(TEXT("[ERRO] Desligar o pipe! (DisconnectNamedPipe)"));
            exit(-1);
        }*/
    }

    SetEvent(dados.acabou);
    CloseHandle(dados.hSemEscrita);
    CloseHandle(dados.hSemLeitura);
    CloseHandle(hServidor);
    CloseHandle(dados.acabou);
    CloseHandle(hFileMap);
    CloseHandle(dados.inicia);
    UnmapViewOfFile(dados.memPar);

    return 0;
}