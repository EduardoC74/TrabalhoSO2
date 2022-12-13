// CLIENTEGRAFICO.cpp : Define o ponto de entrada para o aplicativo.
//
#include "framework.h"
#include "CLIENTEGRAFICO.h"

#include <windows.h>
#include <windowsx.h>
#include <tchar.h>

#include <stdio.h>
#include <time.h>       // for time()

#include "..\Estruturas\estruturas.h"

#define PIPE_NAME TEXT("\\\\.\\pipe\\teste")

/* ===================================================== */
/* Programa base (esqueleto) para aplicações Windows     */
/* ===================================================== */
// Cria uma janela de nome "Janela Principal" e pinta fundo de branco
// Modelo para programas Windows:
//  Composto por 2 funções: 
//	WinMain()     = Ponto de entrada dos programas windows
//			1) Define, cria e mostra a janela
//			2) Loop de recepção de mensagens provenientes do Windows
//     TrataEventos()= Processamentos da janela (pode ter outro nome)
//			1) É chamada pelo Windows (callback) 
//			2) Executa código em função da mensagem recebida

LRESULT CALLBACK TrataEventos(HWND, UINT, WPARAM, LPARAM);

// Nome da classe da janela (para programas de uma só janela, normalmente este nome é 
// igual ao do próprio programa) "szprogName" é usado mais abaixo na definição das 
// propriedades do objecto janela
TCHAR szProgName[] = TEXT("Cliente");

// ============================================================================
// FUNÇÃO DE INÍCIO DO PROGRAMA: WinMain()
// ============================================================================
// Em Windows, o programa começa sempre a sua execução na função WinMain()que desempenha
// o papel da função main() do C em modo consola WINAPI indica o "tipo da função" (WINAPI
// para todas as declaradas nos headers do Windows e CALLBACK para as funções de
// processamento da janela)
// Parâmetros:
//   hInst: Gerado pelo Windows, é o handle (número) da instância deste programa 
//   hPrevInst: Gerado pelo Windows, é sempre NULL para o NT (era usado no Windows 3.1)
//   lpCmdLine: Gerado pelo Windows, é um ponteiro para uma string terminada por 0
//              destinada a conter parâmetros para o programa 
//   nCmdShow:  Parâmetro que especifica o modo de exibição da janela (usado em  
//        	   ShowWindow()

int WINAPI WinMain(HINSTANCE hInst, // instancia atual app
	HINSTANCE hPrevInst,//
	LPSTR lpCmdLine, int nCmdShow) {
	HWND hWnd;		// hWnd é o handler da janela, gerado mais abaixo por CreateWindow()
	MSG lpMsg;		// MSG é uma estrutura definida no Windows para as mensagens
	WNDCLASSEX wcApp;	// WNDCLASSEX é uma estrutura cujos membros servem para 
			  // definir as características da classe da janela

	// ============================================================================
	// 1. Definição das características da janela "wcApp" 
	//    (Valores dos elementos da estrutura "wcApp" do tipo WNDCLASSEX)
	// ============================================================================
	wcApp.cbSize = sizeof(WNDCLASSEX);      // Tamanho da estrutura WNDCLASSEX
	wcApp.hInstance = hInst;		         // Instância da janela actualmente exibida 
								   // ("hInst" é parâmetro de WinMain e vem 
										 // inicializada daí)
	wcApp.lpszClassName = szProgName;       // Nome da janela (neste caso = nome do programa)
	wcApp.lpfnWndProc = TrataEventos;       // Endereço da função de processamento da janela
											// ("TrataEventos" foi declarada no início e
											// encontra-se mais abaixo)
	wcApp.style = CS_HREDRAW | CS_VREDRAW;  // Estilo da janela: Fazer o redraw se for
											// modificada horizontal ou verticalmente

	wcApp.hIcon = LoadIcon(NULL, IDI_WARNING);   // "hIcon" = handler do ícon normal
										   // "NULL" = Icon definido no Windows
										   // "IDI_AP..." Ícone "aplicação"
	wcApp.hIconSm = LoadIcon(NULL, IDI_WARNING); // "hIconSm" = handler do ícon pequeno
										   // "NULL" = Icon definido no Windows
										   // "IDI_INF..." Ícon de informação
	wcApp.hCursor = LoadCursor(NULL, IDC_ARROW);	// "hCursor" = handler do cursor (rato) 
							  // "NULL" = Forma definida no Windows
							  // "IDC_ARROW" Aspecto "seta" 
	wcApp.lpszMenuName = NULL;			// Classe do menu que a janela pode ter
							  // (NULL = não tem menu)
	wcApp.cbClsExtra = 0;				// Livre, para uso particular
	wcApp.cbWndExtra = 0;				// Livre, para uso particular
	wcApp.hbrBackground = CreateSolidBrush(RGB(255, 255, 255));
	// "hbrBackground" = handler para "brush" de pintura do fundo da janela. Devolvido por
	// "GetStockObject".Neste caso o fundo será branco

	// ============================================================================
	// 2. Registar a classe "wcApp" no Windows
	// ============================================================================
	if (!RegisterClassEx(&wcApp))
		return(0);

	// ============================================================================
	// 3. Criar a janela
	// ============================================================================
	hWnd = CreateWindow(
		szProgName,			// Nome da janela (programa) definido acima
		TEXT("Cliente"),// Texto que figura na barra do título
		WS_OVERLAPPEDWINDOW,	// Estilo da janela (WS_OVERLAPPED= normal)
		200,		// Posição x pixels (default=à direita da última) 
		200,		// Posição y pixels (default=abaixo da última)
		830,		// Largura da janela (em pixels)
		700,		// Altura da janela (em pixels)
		(HWND)HWND_DESKTOP,	// handle da janela pai (se se criar uma a partir de
						// outra) ou HWND_DESKTOP se a janela for a primeira, 
						// criada a partir do "desktop"
		(HMENU)NULL,			// handle do menu da janela (se tiver menu)
		(HINSTANCE)hInst,		// handle da instância do programa actual ("hInst" é 
						// passado num dos parâmetros de WinMain()
		0);				// Não há parâmetros adicionais para a janela
	  // ============================================================================
	  // 4. Mostrar a janela
	  // ============================================================================
	ShowWindow(hWnd, nCmdShow);	// "hWnd"= handler da janela, devolvido por 
					  // "CreateWindow"; "nCmdShow"= modo de exibição (p.e. 
					  // normal/modal); é passado como parâmetro de WinMain()
	UpdateWindow(hWnd);		// Refrescar a janela (Windows envia à janela uma 
					  // mensagem para pintar, mostrar dados, (refrescar)… 
	// ============================================================================
	// 5. Loop de Mensagens
	// ============================================================================
	// O Windows envia mensagens às janelas (programas). Estas mensagens ficam numa fila de
	// espera até que GetMessage(...) possa ler "a mensagem seguinte"	
	// Parâmetros de "getMessage":
	// 1)"&lpMsg"=Endereço de uma estrutura do tipo MSG ("MSG lpMsg" ja foi declarada no  
	//   início de WinMain()):
	//			HWND hwnd		handler da janela a que se destina a mensagem
	//			UINT message		Identificador da mensagem
	//			WPARAM wParam		Parâmetro, p.e. código da tecla premida
	//			LPARAM lParam		Parâmetro, p.e. se ALT também estava premida
	//			DWORD time		Hora a que a mensagem foi enviada pelo Windows
	//			POINT pt		Localização do mouse (x, y) 
	// 2)handle da window para a qual se pretendem receber mensagens (=NULL se se pretendem
	//   receber as mensagens para todas as
	// janelas pertencentes à thread actual)
	// 3)Código limite inferior das mensagens que se pretendem receber
	// 4)Código limite superior das mensagens que se pretendem receber

	// NOTA: GetMessage() devolve 0 quando for recebida a mensagem de fecho da janela,
	// 	  terminando então o loop de recepção de mensagens, e o programa 

	while (GetMessage(&lpMsg, NULL, 0, 0)) {
		TranslateMessage(&lpMsg);	// Pré-processamento da mensagem (p.e. obter código 
					   // ASCII da tecla premida)
		DispatchMessage(&lpMsg);	// Enviar a mensagem traduzida de volta ao Windows, que
					   // aguarda até que a possa reenviar à função de 
					   // tratamento da janela, CALLBACK TrataEventos (abaixo)
	}

	// ============================================================================
	// 6. Fim do programa
	// ============================================================================
	return((int)lpMsg.wParam);	// Retorna sempre o parâmetro wParam da estrutura lpMsg
}

// ============================================================================
// FUNÇÃO DE PROCESSAMENTO DA JANELA
// Esta função pode ter um nome qualquer: Apenas é necesário que na inicialização da
// estrutura "wcApp", feita no início de // WinMain(), se identifique essa função. Neste
// caso "wcApp.lpfnWndProc = WndProc"
//
// WndProc recebe as mensagens enviadas pelo Windows (depois de lidas e pré-processadas
// no loop "while" da função WinMain()
// Parâmetros:
//		hWnd	O handler da janela, obtido no CreateWindow()
//		messg	Ponteiro para a estrutura mensagem (ver estrutura em 5. Loop...
//		wParam	O parâmetro wParam da estrutura messg (a mensagem)
//		lParam	O parâmetro lParam desta mesma estrutura
//
// NOTA:Estes parâmetros estão aqui acessíveis o que simplifica o acesso aos seus valores
//
// A função EndProc é sempre do tipo "switch..." com "cases" que descriminam a mensagem
// recebida e a tratar.
// Estas mensagens são identificadas por constantes (p.e. 
// WM_DESTROY, WM_CHAR, WM_KEYDOWN, WM_PAINT...) definidas em windows.h
// ============================================================================

typedef struct {
	int xPos, yPos;
	int numClicks;
	BITMAP bmps;
}PosBitmap;

DWORD WINAPI ThreadAcaba(LPVOID param) {
	DadosThreads* dados = (DadosThreads*)param;

	WaitForSingleObject(dados->acabou, INFINITE);
	MessageBox(dados->hWnd, _T("Perdeu!"), _T("Jogo terminado!"), MB_OK);

	SendMessage(dados->hWnd, WM_DESTROY, NULL, NULL);

	//_tprintf(_T("\n\nJogo terminado!\n"));
	dados->terminar = 1;

	return;
}

DWORD WINAPI ThreadLeMensagens(LPVOID param) {
	DadosThreads* dados = (DadosThreads*)param;
	DWORD n;
	BOOL ret;

	OVERLAPPED ov;
	HANDLE hEvento;
	hEvento = CreateEvent(NULL, TRUE, FALSE, NULL);

	//WaitForSingleObject(dados->inicia, INFINITE);

	//aqui , o servidor já recebeu um cliente
	while (!dados->terminar) {
		ZeroMemory(&ov, sizeof(ov));
		ov.hEvent = hEvento;

		//bloqueamos aqui porque é uma regiao critica
		WaitForSingleObject(dados->hMutex, INFINITE);
		//le do named pipe

		ret = ReadFile(dados->hPipeCli, &dados->cli[0], sizeof(dados->cli[0]), &n, & ov);
		if (!ret && GetLastError() == ERROR_IO_PENDING)
			_tprintf(_T("A aguardar informação..."));
		/*......*/
		WaitForSingleObject(ov.hEvent, INFINITE);
		GetOverlappedResult(dados->hPipeCli, &ov, &n, FALSE);

		ReleaseMutex(dados->hMutex);
	}

	return 0;

}

DWORD WINAPI leJogo(LPVOID lparam)
{
	DadosThreads* dados = (DadosThreads*)lparam;

	while (!dados->terminar)
	{
		WaitForSingleObject(dados->atualizaJogo, INFINITE);
		ResetEvent(dados->atualizaJogo);
		InvalidateRect(dados->hWnd, NULL, TRUE);
		//Sleep(100);
	}
}

//ESTRUTURA PERDE OS VALORES

LRESULT CALLBACK TrataEventos(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam) {
	HDC hdc;
	RECT rect;
	int xPos, yPos, i;
	static  int numClicks = 1;
	static int totalPos = 0;
	PAINTSTRUCT ps;
	static int aux = 1;
	static TCHAR c = 'x';
	static PosBitmap posicoes[1000];
	DWORD n;
	BOOL ret;
	HANDLE hThread[3];
	DWORD tam = 5;

	static DadosThreads dados;

	static HDC bmpDC[17];
	HBITMAP hBmp[17];
	static BITMAP bmp[17];
	static HDC memDC = NULL;

	dados.terminar = 0;
	dados.hWnd = hWnd;
	//dados.msgCli.totalClicks = 0;
	dados.cli[0].nivel = 1;
	DWORD nivel = dados.cli[0].nivel;

	BOOL g_fMouseTracking = FALSE;

	static HWND hBorda2, hNome, hIndividual, hCompeticao, hPecas, hNivel, hEspera, hBorda;
	HMENU hMenu;

	static BOOL joga = TRUE;

	OVERLAPPED ov;
	HANDLE hEvento = CreateEvent(NULL, TRUE, FALSE, NULL);;

	switch (messg) {

	case WM_COMMAND:
	{
		switch (wParam) {
		case FILE_MENU_BITMAP1:
		{
			
			hBmp[0] = (HBITMAP)LoadImage(NULL, TEXT("0.bmp"), IMAGE_BITMAP, 35, 35, LR_LOADFROMFILE);
			GetObject(hBmp[0], sizeof(bmp[0]), &bmp[0]);
			hBmp[1] = (HBITMAP)LoadImage(NULL, TEXT("1.bmp"), IMAGE_BITMAP, 35, 35, LR_LOADFROMFILE);
			GetObject(hBmp[1], sizeof(bmp[1]), &bmp[1]);
			hBmp[2] = (HBITMAP)LoadImage(NULL, TEXT("2.bmp"), IMAGE_BITMAP, 35, 35, LR_LOADFROMFILE);
			GetObject(hBmp[2], sizeof(bmp[2]), &bmp[2]);
			hBmp[3] = (HBITMAP)LoadImage(NULL, TEXT("3.bmp"), IMAGE_BITMAP, 35, 35, LR_LOADFROMFILE);
			GetObject(hBmp[3], sizeof(bmp[3]), &bmp[3]);
			hBmp[4] = (HBITMAP)LoadImage(NULL, TEXT("4.bmp"), IMAGE_BITMAP, 35, 35, LR_LOADFROMFILE);
			GetObject(hBmp[4], sizeof(bmp[4]), &bmp[4]);
			hBmp[5] = (HBITMAP)LoadImage(NULL, TEXT("5.bmp"), IMAGE_BITMAP, 35, 35, LR_LOADFROMFILE);
			GetObject(hBmp[5], sizeof(bmp[5]), &bmp[5]);

			hBmp[6] = (HBITMAP)LoadImage(NULL, TEXT("6.bmp"), IMAGE_BITMAP, 35, 35, LR_LOADFROMFILE);
			GetObject(hBmp[6], sizeof(bmp[6]), &bmp[6]);
			hBmp[7] = (HBITMAP)LoadImage(NULL, TEXT("7.bmp"), IMAGE_BITMAP, 35, 35, LR_LOADFROMFILE);
			GetObject(hBmp[7], sizeof(bmp[7]), &bmp[7]);
			hBmp[8] = (HBITMAP)LoadImage(NULL, TEXT("8.bmp"), IMAGE_BITMAP, 35, 35, LR_LOADFROMFILE);
			GetObject(hBmp[8], sizeof(bmp[8]), &bmp[8]);
			hBmp[9] = (HBITMAP)LoadImage(NULL, TEXT("9.bmp"), IMAGE_BITMAP, 35, 35, LR_LOADFROMFILE);
			GetObject(hBmp[9], sizeof(bmp[9]), &bmp[9]);
			hBmp[10] = (HBITMAP)LoadImage(NULL, TEXT("10.bmp"), IMAGE_BITMAP, 35, 35, LR_LOADFROMFILE);
			GetObject(hBmp[10], sizeof(bmp[10]), &bmp[10]);
			hBmp[11] = (HBITMAP)LoadImage(NULL, TEXT("11.bmp"), IMAGE_BITMAP, 35, 35, LR_LOADFROMFILE);
			GetObject(hBmp[11], sizeof(bmp[11]), &bmp[11]);

			hBmp[12] = (HBITMAP)LoadImage(NULL, TEXT("12.bmp"), IMAGE_BITMAP, 35, 35, LR_LOADFROMFILE);
			GetObject(hBmp[12], sizeof(bmp[12]), &bmp[12]);
			hBmp[13] = (HBITMAP)LoadImage(NULL, TEXT("15.bmp"), IMAGE_BITMAP, 35, 35, LR_LOADFROMFILE);
			GetObject(hBmp[13], sizeof(bmp[13]), &bmp[13]);

			hBmp[14] = (HBITMAP)LoadImage(NULL, TEXT("14.bmp"), IMAGE_BITMAP, 35, 35, LR_LOADFROMFILE);
			GetObject(hBmp[14], sizeof(bmp[14]), &bmp[14]);

			hBmp[15] = (HBITMAP)LoadImage(NULL, TEXT("17.bmp"), IMAGE_BITMAP, 35, 35, LR_LOADFROMFILE);
			GetObject(hBmp[15], sizeof(bmp[15]), &bmp[15]);


			hdc = GetDC(hWnd);

			bmpDC[0] = CreateCompatibleDC(hdc);
			SelectObject(bmpDC[0], hBmp[0]);
			bmpDC[1] = CreateCompatibleDC(hdc);
			SelectObject(bmpDC[1], hBmp[1]);
			bmpDC[2] = CreateCompatibleDC(hdc);
			SelectObject(bmpDC[2], hBmp[2]);
			bmpDC[3] = CreateCompatibleDC(hdc);
			SelectObject(bmpDC[3], hBmp[3]);
			bmpDC[4] = CreateCompatibleDC(hdc);
			SelectObject(bmpDC[4], hBmp[4]);
			bmpDC[5] = CreateCompatibleDC(hdc);
			SelectObject(bmpDC[5], hBmp[5]);

			bmpDC[6] = CreateCompatibleDC(hdc);
			SelectObject(bmpDC[6], hBmp[6]);
			bmpDC[7] = CreateCompatibleDC(hdc);
			SelectObject(bmpDC[7], hBmp[7]);
			bmpDC[8] = CreateCompatibleDC(hdc);
			SelectObject(bmpDC[8], hBmp[8]);
			bmpDC[9] = CreateCompatibleDC(hdc);
			SelectObject(bmpDC[9], hBmp[9]);
			bmpDC[10] = CreateCompatibleDC(hdc);
			SelectObject(bmpDC[10], hBmp[10]);
			bmpDC[11] = CreateCompatibleDC(hdc);
			SelectObject(bmpDC[11], hBmp[11]);

			bmpDC[12] = CreateCompatibleDC(hdc);
			SelectObject(bmpDC[12], hBmp[12]);
			bmpDC[13] = CreateCompatibleDC(hdc);
			SelectObject(bmpDC[13], hBmp[13]);

			bmpDC[14] = CreateCompatibleDC(hdc);
			SelectObject(bmpDC[14], hBmp[14]);

			bmpDC[15] = CreateCompatibleDC(hdc);
			SelectObject(bmpDC[15], hBmp[15]);

			ReleaseDC(hWnd, hdc);


		}
		break;
		case FILE_MENU_BITMAP2:
		{
			//MessageBeep(MB_OK);
			hBmp[0] = (HBITMAP)LoadImage(NULL, TEXT("0a.bmp"), IMAGE_BITMAP, 35, 35, LR_LOADFROMFILE);
			GetObject(hBmp[0], sizeof(bmp[0]), &bmp[0]);
			hBmp[1] = (HBITMAP)LoadImage(NULL, TEXT("1a.bmp"), IMAGE_BITMAP, 35, 35, LR_LOADFROMFILE);
			GetObject(hBmp[1], sizeof(bmp[1]), &bmp[1]);
			hBmp[2] = (HBITMAP)LoadImage(NULL, TEXT("2a.bmp"), IMAGE_BITMAP, 35, 35, LR_LOADFROMFILE);
			GetObject(hBmp[2], sizeof(bmp[2]), &bmp[2]);
			hBmp[3] = (HBITMAP)LoadImage(NULL, TEXT("3a.bmp"), IMAGE_BITMAP, 35, 35, LR_LOADFROMFILE);
			GetObject(hBmp[3], sizeof(bmp[3]), &bmp[3]);
			hBmp[4] = (HBITMAP)LoadImage(NULL, TEXT("4a.bmp"), IMAGE_BITMAP, 35, 35, LR_LOADFROMFILE);
			GetObject(hBmp[4], sizeof(bmp[4]), &bmp[4]);
			hBmp[5] = (HBITMAP)LoadImage(NULL, TEXT("5a.bmp"), IMAGE_BITMAP, 35, 35, LR_LOADFROMFILE);
			GetObject(hBmp[5], sizeof(bmp[5]), &bmp[5]);

			hBmp[6] = (HBITMAP)LoadImage(NULL, TEXT("6a.bmp"), IMAGE_BITMAP, 35, 35, LR_LOADFROMFILE);
			GetObject(hBmp[6], sizeof(bmp[6]), &bmp[6]);
			hBmp[7] = (HBITMAP)LoadImage(NULL, TEXT("7a.bmp"), IMAGE_BITMAP, 35, 35, LR_LOADFROMFILE);
			GetObject(hBmp[7], sizeof(bmp[7]), &bmp[7]);
			hBmp[8] = (HBITMAP)LoadImage(NULL, TEXT("8a.bmp"), IMAGE_BITMAP, 35, 35, LR_LOADFROMFILE);
			GetObject(hBmp[8], sizeof(bmp[8]), &bmp[8]);
			hBmp[9] = (HBITMAP)LoadImage(NULL, TEXT("9a.bmp"), IMAGE_BITMAP, 35, 35, LR_LOADFROMFILE);
			GetObject(hBmp[9], sizeof(bmp[9]), &bmp[9]);
			hBmp[10] = (HBITMAP)LoadImage(NULL, TEXT("10a.bmp"), IMAGE_BITMAP, 35, 35, LR_LOADFROMFILE);
			GetObject(hBmp[10], sizeof(bmp[10]), &bmp[10]);
			hBmp[11] = (HBITMAP)LoadImage(NULL, TEXT("11a.bmp"), IMAGE_BITMAP, 35, 35, LR_LOADFROMFILE);
			GetObject(hBmp[11], sizeof(bmp[11]), &bmp[11]);

			hBmp[12] = (HBITMAP)LoadImage(NULL, TEXT("12a.bmp"), IMAGE_BITMAP, 35, 35, LR_LOADFROMFILE);
			GetObject(hBmp[12], sizeof(bmp[12]), &bmp[12]);
			hBmp[13] = (HBITMAP)LoadImage(NULL, TEXT("13a.bmp"), IMAGE_BITMAP, 35, 35, LR_LOADFROMFILE);
			GetObject(hBmp[13], sizeof(bmp[13]), &bmp[13]);

			hBmp[14] = (HBITMAP)LoadImage(NULL, TEXT("14a.bmp"), IMAGE_BITMAP, 35, 35, LR_LOADFROMFILE);
			GetObject(hBmp[14], sizeof(bmp[14]), &bmp[14]);

			hBmp[15] = (HBITMAP)LoadImage(NULL, TEXT("17a.bmp"), IMAGE_BITMAP, 35, 35, LR_LOADFROMFILE);
			GetObject(hBmp[15], sizeof(bmp[15]), &bmp[15]);


			hdc = GetDC(hWnd);

			bmpDC[0] = CreateCompatibleDC(hdc);
			SelectObject(bmpDC[0], hBmp[0]);
			bmpDC[1] = CreateCompatibleDC(hdc);
			SelectObject(bmpDC[1], hBmp[1]);
			bmpDC[2] = CreateCompatibleDC(hdc);
			SelectObject(bmpDC[2], hBmp[2]);
			bmpDC[3] = CreateCompatibleDC(hdc);
			SelectObject(bmpDC[3], hBmp[3]);
			bmpDC[4] = CreateCompatibleDC(hdc);
			SelectObject(bmpDC[4], hBmp[4]);
			bmpDC[5] = CreateCompatibleDC(hdc);
			SelectObject(bmpDC[5], hBmp[5]);

			bmpDC[6] = CreateCompatibleDC(hdc);
			SelectObject(bmpDC[6], hBmp[6]);
			bmpDC[7] = CreateCompatibleDC(hdc);
			SelectObject(bmpDC[7], hBmp[7]);
			bmpDC[8] = CreateCompatibleDC(hdc);
			SelectObject(bmpDC[8], hBmp[8]);
			bmpDC[9] = CreateCompatibleDC(hdc);
			SelectObject(bmpDC[9], hBmp[9]);
			bmpDC[10] = CreateCompatibleDC(hdc);
			SelectObject(bmpDC[10], hBmp[10]);
			bmpDC[11] = CreateCompatibleDC(hdc);
			SelectObject(bmpDC[11], hBmp[11]);

			bmpDC[12] = CreateCompatibleDC(hdc);
			SelectObject(bmpDC[12], hBmp[12]);
			bmpDC[13] = CreateCompatibleDC(hdc);
			SelectObject(bmpDC[13], hBmp[13]);

			bmpDC[14] = CreateCompatibleDC(hdc);
			SelectObject(bmpDC[14], hBmp[14]);

			bmpDC[15] = CreateCompatibleDC(hdc);
			SelectObject(bmpDC[15], hBmp[15]);

			ReleaseDC(hWnd, hdc);




		}
		break;
		case 3:
		{
			joga = TRUE;

			GetWindowTextW(hNome, dados.cli[0].nome, 100);
			memcpy(dados.cli[0].tipoJogo, _T("individual"), 100);


			DestroyWindow(hBorda);
			DestroyWindow(hNome);
			DestroyWindow(hBorda2);
			DestroyWindow(hIndividual);
			DestroyWindow(hCompeticao);

			TCHAR texto[10];
			wsprintf(texto, _T("Nivel %d"), dados.cli[0].nivel);
			hNivel = CreateWindowW(L"static", texto, WS_VISIBLE | WS_CHILD | SS_CENTER, 280, 20, 200, 30, hWnd, NULL, NULL, NULL);

			//bloqueamos aqui porque é uma regiao critica
			//WaitForSingleObject(dados.hMutex, INFINITE);
			//escreve no named pipe
			//for (int i = 0; i < dados->nClientes; i++) {
			ZeroMemory(&ov, sizeof(ov));
			ov.hEvent = hEvento;

			WriteFile(dados.hPipeCli, (LPVOID)&dados.cli[0], sizeof(dados.cli[0]), &n, &ov);
			//WaitForSingleObject(ov.hEvent, INFINITE);
			//GetOverlappedResult(dados.hPipeCli, &ov, n, FALSE);


			/*if (!WriteFile(dados.hPipeCli, (LPVOID)&dados.cli[0], sizeof(dados.cli[0]), &n, NULL)) {
				_tprintf(TEXT("[ERRO] Escrever no pipe! (WriteFile)\n"));
				exit(-1);
			}*/

			//ReleaseMutex(dados.hMutex);

			//SetEvent(dados.comecaCliente);

			hThread[0] = CreateThread(NULL, 0, ThreadLeMensagens, &dados, 0, NULL);
			if (hThread[0] == NULL) {
				_tprintf(_T("Erro ao criar a thread le mensagens!"));
				return -1;
			}

			hThread[1] = CreateThread(NULL, 0, leJogo, &dados, 0, NULL);
			if (hThread[1] == NULL) {
				_tprintf(_T("Erro ao criar a thread mostra jogo!"));
				return -1;
			}

		}
		break;
		case 4:
		{
			joga = TRUE;

			GetWindowTextW(hNome, dados.cli[0].nome, 100);
			memcpy(dados.cli[0].tipoJogo, _T("competicao"), 100);

			DestroyWindow(hBorda);
			DestroyWindow(hNome);
			DestroyWindow(hBorda2);
			DestroyWindow(hIndividual);
			DestroyWindow(hCompeticao);

			hNivel = CreateWindowW(L"static", _T("Modo Competicao"), WS_VISIBLE | WS_CHILD | SS_CENTER, 215, 20, 200, 30, hWnd, NULL, NULL, NULL);

			//bloqueamos aqui porque é uma regiao critica
			WaitForSingleObject(dados.hMutex, INFINITE);
			//escreve no named pipe
			//for (int i = 0; i < dados->nClientes; i++) {
			ZeroMemory(&ov, sizeof(ov));
			ov.hEvent = hEvento;

			WriteFile(dados.hPipeCli, (LPVOID)&dados.cli[0], sizeof(dados.cli[0]), &n, &ov);
			//WaitForSingleObject(ov.hEvent, INFINITE);
			//GetOverlappedResult(dados.hPipeCli, &ov, n, FALSE);

			/*if (!WriteFile(dados.hPipeCli, (LPVOID)&dados.cli[0], sizeof(dados.cli[0]), &n, NULL)) {
				_tprintf(TEXT("[ERRO] Escrever no pipe! (WriteFile)\n"));
				exit(-1);
			}*/

			ReleaseMutex(dados.hMutex);

			//SetEvent(dados.clienteIniciou2);
			//SetEvent(dados.comecaCliente);

			/*WaitForSingleObject(dados.hMutex, INFINITE);
			//le do named pipe
			ret = ReadFile(dados.hPipeCli, &dados.cli[0], sizeof(dados.cli[0]), &n, NULL);

			ReleaseMutex(dados.hMutex);

			if (dados.cli[0].clis < 2)
				joga = FALSE;

			hThread[0] = CreateThread(NULL, 0, ThreadLeMensagens, &dados, 0, NULL);
			if (hThread[0] == NULL) {
				_tprintf(_T("Erro ao criar a thread le mensagens!"));
				return -1;
			}

			hThread[1] = CreateThread(NULL, 0, leJogo, &dados, 0, NULL);
			if (hThread[1] == NULL) {
				_tprintf(_T("Erro ao criar a thread mostra jogo!"));
				return -1;
			}

			hThread[2] = CreateThread(NULL, 0, ThreadAcaba, &dados, 0, NULL);
			if (hThread[2] == NULL) {
				_tprintf(_T("Erro ao criar a thread acaba!"));
				return -1;
			}*/

		}
		break;
		}
	}
	break;
	case WM_CREATE:
	{
		joga = FALSE;

		ZeroMemory(&dados.cli[0], sizeof(dados.cli[0]));

		dados.cli[0].hover = 0;
		
		dados.hMutex = CreateMutex(NULL, FALSE, TEXT("SO2_MUTEX_CLIENTE"));

		if (dados.hMutex == NULL) {
			_tprintf(TEXT("Erro no CreateMutex\n"));
			return -1;
		}

		//espera que exista um named pipe para ler do mesmo
		//bloqueia aqui
		if (!WaitNamedPipe(PIPE_NAME, NMPWAIT_WAIT_FOREVER)) {
			_tprintf(TEXT("[ERRO] Ligar ao pipe '%s'! (WaitNamedPipe)\n"), PIPE_NAME);
			exit(-1);
		}

		while (1) {
			//ligamo-nos ao named pipe que ja existe nesta altura
			//1º nome do named pipe, 2ºpermissoes (têm de ser iguais ao CreateNamedPipe do servidor), 3ºshared mode 0 aqui,
			//4º security atributes, 5ºflags de criação OPEN_EXISTING, 6º o default é FILE_ATTRIBUTE_NORMAL e o 7º é o template é NULL
			dados.hPipeCli = CreateFile(PIPE_NAME, PIPE_ACCESS_DUPLEX, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);

			if (dados.hPipeCli == NULL) {
				_tprintf(TEXT("[ERRO] Ligar ao pipe '%s'! (CreateFile)\n"), PIPE_NAME);
				exit(-1);
			}
			if (dados.hPipeCli != INVALID_HANDLE_VALUE)
				break;
			if (!WaitNamedPipe(PIPE_NAME, NMPWAIT_USE_DEFAULT_WAIT))
				_tprintf(_T("[ERRO] Named Pipe não existe !!"));
		}

		dados.comecaCliente = OpenEvent(EVENT_ALL_ACCESS, TRUE, _T("SO2_CLIENTE_COMECOU"));

		dados.leDoCliente = OpenEvent(EVENT_ALL_ACCESS, TRUE, _T("SO2_LE_CLIENTE"));

		dados.acabou = OpenEvent(EVENT_ALL_ACCESS, TRUE, _T("SO2_SERVIDOR_ACABOU"));

		dados.clienteIniciou2 = OpenEvent(EVENT_ALL_ACCESS, TRUE, _T("SO2_CLIENTE2_COMECOU"));

		dados.atualizaJogo = OpenEvent(EVENT_ALL_ACCESS, TRUE, _T("SO2_ATUALIZA_JOGO"));

		hBmp[0] = (HBITMAP)LoadImage(NULL, TEXT("0.bmp"), IMAGE_BITMAP, 35, 35, LR_LOADFROMFILE);
		GetObject(hBmp[0], sizeof(bmp[0]), &bmp[0]);
		hBmp[1] = (HBITMAP)LoadImage(NULL, TEXT("1.bmp"), IMAGE_BITMAP, 35, 35, LR_LOADFROMFILE);
		GetObject(hBmp[1], sizeof(bmp[1]), &bmp[1]);
		hBmp[2] = (HBITMAP)LoadImage(NULL, TEXT("2.bmp"), IMAGE_BITMAP, 35, 35, LR_LOADFROMFILE);
		GetObject(hBmp[2], sizeof(bmp[2]), &bmp[2]);
		hBmp[3] = (HBITMAP)LoadImage(NULL, TEXT("3.bmp"), IMAGE_BITMAP, 35, 35, LR_LOADFROMFILE);
		GetObject(hBmp[3], sizeof(bmp[3]), &bmp[3]);
		hBmp[4] = (HBITMAP)LoadImage(NULL, TEXT("4.bmp"), IMAGE_BITMAP, 35, 35, LR_LOADFROMFILE);
		GetObject(hBmp[4], sizeof(bmp[4]), &bmp[4]);
		hBmp[5] = (HBITMAP)LoadImage(NULL, TEXT("5.bmp"), IMAGE_BITMAP, 35, 35, LR_LOADFROMFILE);
		GetObject(hBmp[5], sizeof(bmp[5]), &bmp[5]);

		hBmp[6] = (HBITMAP)LoadImage(NULL, TEXT("6.bmp"), IMAGE_BITMAP, 35, 35, LR_LOADFROMFILE);
		GetObject(hBmp[6], sizeof(bmp[6]), &bmp[6]);
		hBmp[7] = (HBITMAP)LoadImage(NULL, TEXT("7.bmp"), IMAGE_BITMAP, 35, 35, LR_LOADFROMFILE);
		GetObject(hBmp[7], sizeof(bmp[7]), &bmp[7]);
		hBmp[8] = (HBITMAP)LoadImage(NULL, TEXT("8.bmp"), IMAGE_BITMAP, 35, 35, LR_LOADFROMFILE);
		GetObject(hBmp[8], sizeof(bmp[8]), &bmp[8]);
		hBmp[9] = (HBITMAP)LoadImage(NULL, TEXT("9.bmp"), IMAGE_BITMAP, 35, 35, LR_LOADFROMFILE);
		GetObject(hBmp[9], sizeof(bmp[9]), &bmp[9]);
		hBmp[10] = (HBITMAP)LoadImage(NULL, TEXT("10.bmp"), IMAGE_BITMAP, 35, 35, LR_LOADFROMFILE);
		GetObject(hBmp[10], sizeof(bmp[10]), &bmp[10]);
		hBmp[11] = (HBITMAP)LoadImage(NULL, TEXT("11.bmp"), IMAGE_BITMAP, 35, 35, LR_LOADFROMFILE);
		GetObject(hBmp[11], sizeof(bmp[11]), &bmp[11]);

		hBmp[12] = (HBITMAP)LoadImage(NULL, TEXT("12.bmp"), IMAGE_BITMAP, 35, 35, LR_LOADFROMFILE);
		GetObject(hBmp[12], sizeof(bmp[12]), &bmp[12]);
		hBmp[13] = (HBITMAP)LoadImage(NULL, TEXT("15.bmp"), IMAGE_BITMAP, 35, 35, LR_LOADFROMFILE);
		GetObject(hBmp[13], sizeof(bmp[13]), &bmp[13]);

		hBmp[14] = (HBITMAP)LoadImage(NULL, TEXT("14.bmp"), IMAGE_BITMAP, 35, 35, LR_LOADFROMFILE);
		GetObject(hBmp[14], sizeof(bmp[14]), &bmp[14]);

		hBmp[15] = (HBITMAP)LoadImage(NULL, TEXT("17.bmp"), IMAGE_BITMAP, 35, 35, LR_LOADFROMFILE);
		GetObject(hBmp[15], sizeof(bmp[15]), &bmp[15]);


		hdc = GetDC(hWnd);

		bmpDC[0] = CreateCompatibleDC(hdc);
		SelectObject(bmpDC[0], hBmp[0]);
		bmpDC[1] = CreateCompatibleDC(hdc);
		SelectObject(bmpDC[1], hBmp[1]);
		bmpDC[2] = CreateCompatibleDC(hdc);
		SelectObject(bmpDC[2], hBmp[2]);
		bmpDC[3] = CreateCompatibleDC(hdc);
		SelectObject(bmpDC[3], hBmp[3]);
		bmpDC[4] = CreateCompatibleDC(hdc);
		SelectObject(bmpDC[4], hBmp[4]);
		bmpDC[5] = CreateCompatibleDC(hdc);
		SelectObject(bmpDC[5], hBmp[5]);

		bmpDC[6] = CreateCompatibleDC(hdc);
		SelectObject(bmpDC[6], hBmp[6]);
		bmpDC[7] = CreateCompatibleDC(hdc);
		SelectObject(bmpDC[7], hBmp[7]);
		bmpDC[8] = CreateCompatibleDC(hdc);
		SelectObject(bmpDC[8], hBmp[8]);
		bmpDC[9] = CreateCompatibleDC(hdc);
		SelectObject(bmpDC[9], hBmp[9]);
		bmpDC[10] = CreateCompatibleDC(hdc);
		SelectObject(bmpDC[10], hBmp[10]);
		bmpDC[11] = CreateCompatibleDC(hdc);
		SelectObject(bmpDC[11], hBmp[11]);

		bmpDC[12] = CreateCompatibleDC(hdc);
		SelectObject(bmpDC[12], hBmp[12]);
		bmpDC[13] = CreateCompatibleDC(hdc);
		SelectObject(bmpDC[13], hBmp[13]);

		bmpDC[14] = CreateCompatibleDC(hdc);
		SelectObject(bmpDC[14], hBmp[14]);

		bmpDC[15] = CreateCompatibleDC(hdc);
		SelectObject(bmpDC[15], hBmp[15]);

		ReleaseDC(hWnd, hdc);

		hThread[2] = CreateThread(NULL, 0, ThreadAcaba, &dados, 0, NULL);
		if (hThread[2] == NULL) {
			_tprintf(_T("Erro ao criar a thread acaba!"));
			return -1;
		}

		hMenu = CreateMenu();
		HMENU hFileMenu = CreateMenu();

		AppendMenu(hFileMenu, MF_STRING, FILE_MENU_BITMAP1, _T("Bitmap1"));
		AppendMenu(hFileMenu, MF_STRING, FILE_MENU_BITMAP2, _T("Bitmap2"));
		AppendMenu(hFileMenu, MF_SEPARATOR, NULL, NULL);

		AppendMenu(hMenu, MF_POPUP,(UINT_PTR)hFileMenu, _T("Menu"));
		SetMenu(hWnd, hMenu);

		hBorda = CreateWindowW(L"static", L"Insert your name: ", WS_VISIBLE | WS_CHILD | WS_BORDER | SS_CENTER, 240, 115, 300, 350, hWnd, NULL, NULL, NULL);
		hNome = CreateWindowW(L"Edit", L"...", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL, 255, 150, 270, 50, hWnd, NULL, NULL, NULL);
		hBorda2 = CreateWindowW(L"static", L"Type of game", WS_VISIBLE | WS_CHILD | WS_BORDER | SS_CENTER, 255, 235, 270, 200, hWnd, NULL, NULL, NULL);
		hIndividual = CreateWindowW(L"Button", L"Individual", WS_VISIBLE | WS_CHILD, 270, 285, 240, 50, hWnd, (HMENU)3, NULL, NULL);
		hCompeticao = CreateWindowW(L"Button", L"Competicao", WS_VISIBLE | WS_CHILD, 270, 355, 240, 50, hWnd, (HMENU)4, NULL, NULL);

	}
	break;
	case WM_LBUTTONDOWN:
	{// <-BOTAO ESQUERDO, BOTADO DIREITO -> WM_RBUTTONDOWN

		if (joga) {

			posicoes[totalPos].xPos = GET_X_LPARAM(lParam);
			posicoes[totalPos].yPos = GET_Y_LPARAM(lParam);

			int i;
			BOOL existe = FALSE;
			for (int i = 0; i <= dados.cli[0].mapaJogo.tamMap, existe == FALSE; i++) {
				if (80 + (i * 35) > posicoes[totalPos].yPos) {
					posicoes[totalPos].yPos = i - 1;
					existe = TRUE;
				}
			}

			int j;
			BOOL existe2 = FALSE;
			for (int j = 0; j <= dados.cli[0].mapaJogo.tamMap, existe2 == FALSE; j++) {
				if (40 + (j * 35) > posicoes[totalPos].xPos) {
					posicoes[totalPos].xPos = j - 1;
					existe2 = TRUE;
				}
			}

			numClicks++;
			//posicoes[totalPos].c = c;
			//posicoes[totalPos].bmps = bmp[0];
			hdc = GetDC(hWnd); //A função GetDC recupera um identificador para um contexto de dispositivo (DC)
			GetClientRect(hWnd, &rect);
			//SetTextColor(hdc, RGB(255, 255, 255));
			//SetBkMode(hdc, TRANSPARENT);
			rect.left = posicoes[totalPos].xPos;
			rect.top = posicoes[totalPos].yPos;
			totalPos++;

			if (rect.left >= 0 && rect.left < dados.cli[0].mapaJogo.tamMap && rect.top >= 0 && rect.top < dados.cli[0].mapaJogo.tamMap) {

				//bloqueamos aqui porque é uma regiao critica
				WaitForSingleObject(dados.hMutex, INFINITE);

				dados.cli[0].linhaColuna[0] = rect.top;
				dados.cli[0].linhaColuna[1] = rect.left;
				dados.cli[0].clickTipo = 0;

				//escreve no named pipe
				//for (int i = 0; i < dados->nClientes; i++) {
				ZeroMemory(&ov, sizeof(ov));
				ov.hEvent = hEvento;

				WriteFile(dados.hPipeCli, (LPVOID)&dados.cli[0], sizeof(dados.cli[0]), &n, &ov);
				//WaitForSingleObject(ov.hEvent, INFINITE);
				//GetOverlappedResult(dados.hPipeCli, &ov, n, FALSE);

				/*if (!WriteFile(dados.hPipeCli, (LPVOID)&dados.cli[0], sizeof(dados.cli[0]), &n, NULL)) {
					_tprintf(TEXT("[ERRO] Escrever no pipe! (WriteFile)\n"));
					exit(-1);
				}*/

				ReleaseMutex(dados.hMutex);

				SetEvent(dados.leDoCliente);
			}

		}
	}
	break;
	case WM_RBUTTONDOWN:
	{
		if (joga) {

			posicoes[totalPos].xPos = GET_X_LPARAM(lParam);
			posicoes[totalPos].yPos = GET_Y_LPARAM(lParam);

			int i;
			BOOL existe = FALSE;
			for (int i = 0; i <= dados.cli[0].mapaJogo.tamMap, existe == FALSE; i++) {
				if (80 + (i * 35) > posicoes[totalPos].yPos) {
					posicoes[totalPos].yPos = i - 1;
					existe = TRUE;
				}
			}

			int j;
			BOOL existe2 = FALSE;
			for (int j = 0; j <= dados.cli[0].mapaJogo.tamMap, existe2 == FALSE; j++) {
				if (40 + (j * 35) > posicoes[totalPos].xPos) {
					posicoes[totalPos].xPos = j - 1;
					existe2 = TRUE;
				}
			}

			hdc = GetDC(hWnd); //A função GetDC recupera um identificador para um contexto de dispositivo (DC)
			GetClientRect(hWnd, &rect);
			//SetTextColor(hdc, RGB(255, 255, 255));
			//SetBkMode(hdc, TRANSPARENT);

			rect.left = posicoes[totalPos].xPos;
			rect.top = posicoes[totalPos].yPos;
			totalPos++;

			if (rect.left >= 0 && rect.left < dados.cli[0].mapaJogo.tamMap && rect.top >= 0 && rect.top < dados.cli[0].mapaJogo.tamMap) {

				//bloqueamos aqui porque é uma regiao critica
				WaitForSingleObject(dados.hMutex, INFINITE);

				dados.cli[0].linhaColuna[0] = rect.top;
				dados.cli[0].linhaColuna[1] = rect.left;
				dados.cli[0].clickTipo = 1;

				//escreve no named pipe
				//for (int i = 0; i < dados->nClientes; i++) {
				ZeroMemory(&ov, sizeof(ov));
				ov.hEvent = hEvento;

				WriteFile(dados.hPipeCli, (LPVOID)&dados.cli[0], sizeof(dados.cli[0]), &n, &ov);
				//WaitForSingleObject(ov.hEvent, INFINITE);
				//GetOverlappedResult(dados.hPipeCli, &ov, n, FALSE);

				/*if (!WriteFile(dados.hPipeCli, (LPVOID)&dados.cli[0], sizeof(dados.cli[0]), &n, NULL)) {
					_tprintf(TEXT("[ERRO] Escrever no pipe! (WriteFile)\n"));
					exit(-1);
				}*/

				ReleaseMutex(dados.hMutex);

				SetEvent(dados.leDoCliente);
			}

		}

	}
	break;
	case WM_MOUSEMOVE:
	{
		if (joga) {
			if (!g_fMouseTracking)
			{
				// start tracking if we aren't already
				TRACKMOUSEEVENT tme;
				tme.cbSize = sizeof(TRACKMOUSEEVENT);
				tme.dwFlags = TME_HOVER | TME_LEAVE;
				tme.hwndTrack = hWnd;
				tme.dwHoverTime = 2000;
				g_fMouseTracking = TrackMouseEvent(&tme);

			}
		}
	}
		break;
	case WM_MOUSEHOVER:
	{
		if (joga) {
			g_fMouseTracking = FALSE; // tracking now cancelled

			DWORD posX = GET_X_LPARAM(lParam);
			DWORD posY = GET_Y_LPARAM(lParam);

			/*int i;
			BOOL existe = FALSE;
			for (int i = 0; i <= dados.cli[0].mapaJogo.tamMap, existe == FALSE; i++) {
				if (80 + (i * 35) > posicoes[totalPos].yPos) {
					posicoes[totalPos].yPos = i - 1;
					existe = TRUE;
				}
			}

			int j;
			BOOL existe2 = FALSE;
			for (int j = 0; j <= dados.cli[0].mapaJogo.tamMap, existe2 == FALSE; j++) {
				if (40 + (j * 35) > posicoes[totalPos].xPos) {
					posicoes[totalPos].xPos = j - 1;
					existe2 = TRUE;
				}
			}

			hdc = GetDC(hWnd); //A função GetDC recupera um identificador para um contexto de dispositivo (DC)
			GetClientRect(hWnd, &rect);
			//SetTextColor(hdc, RGB(255, 255, 255));
			//SetBkMode(hdc, TRANSPARENT);

			rect.left = posicoes[totalPos].xPos;
			rect.top = posicoes[totalPos].yPos;
			totalPos++;

			WCHAR szTest[10];
			wsprintf(szTest, _T("%d"), rect.left);
			WCHAR szTest2[10];
			wsprintf(szTest, _T("%d"), rect.top);
			MessageBox(hWnd, szTest, szTest2, MB_OK);*/

			//dados.cli[0].linhaColuna[0] = rect.top;
			//dados.cli[0].linhaColuna[1] = rect.left;

			//bloqueamos aqui porque é uma regiao critica
			WaitForSingleObject(dados.hMutex, INFINITE);

			dados.cli[0].clickTipo = 2;

			//escreve no named pipe
			//for (int i = 0; i < dados->nClientes; i++) {
			ZeroMemory(&ov, sizeof(ov));
			ov.hEvent = hEvento;

			WriteFile(dados.hPipeCli, (LPVOID)&dados.cli[0], sizeof(dados.cli[0]), &n, &ov);
			//WaitForSingleObject(ov.hEvent, INFINITE);
			//GetOverlappedResult(dados.hPipeCli, &ov, n, FALSE);

			/*if (!WriteFile(dados.hPipeCli, (LPVOID)&dados.cli[0], sizeof(dados.cli[0]), &n, NULL)) {
				_tprintf(TEXT("[ERRO] Escrever no pipe! (WriteFile)\n"));
				exit(-1);
			}*/

			ReleaseMutex(dados.hMutex);

			SetEvent(dados.leDoCliente);

			//MessageBox(hWnd, _T("edefr"), _T("nnnn"), MB_OK);

			TRACKMOUSEEVENT tme;
			tme.cbSize = sizeof(TRACKMOUSEEVENT);
			tme.dwFlags = TME_LEAVE;
			tme.hwndTrack = hWnd;
			TrackMouseEvent(&tme);
		}
	}
		break;
	case WM_MOUSELEAVE:
	{
		if (joga) {
			g_fMouseTracking = FALSE; // tracking now cancelled

			//bloqueamos aqui porque é uma regiao critica
			WaitForSingleObject(dados.hMutex, INFINITE);
			
			dados.cli[0].clickTipo = 3;

			//escreve no named pipe
			//for (int i = 0; i < dados->nClientes; i++) {
			ZeroMemory(&ov, sizeof(ov));
			ov.hEvent = hEvento;

			WriteFile(dados.hPipeCli, (LPVOID)&dados.cli[0], sizeof(dados.cli[0]), &n, &ov);
			//WaitForSingleObject(ov.hEvent, INFINITE);
			//GetOverlappedResult(dados.hPipeCli, &ov, n, FALSE);

			/*if (!WriteFile(dados.hPipeCli, (LPVOID)&dados.cli[0], sizeof(dados.cli[0]), &n, NULL)) {
				_tprintf(TEXT("[ERRO] Escrever no pipe! (WriteFile)\n"));
				exit(-1);
			}*/

			ReleaseMutex(dados.hMutex);

			SetEvent(dados.leDoCliente);
		}
	}
		break;
	case WM_CLOSE:
		if (MessageBox(hWnd, _T("Queres sair?"), _T("Sair"), MB_ICONQUESTION | MB_YESNO | MB_HELP) == IDYES)
		{

			WaitForSingleObject(dados.hMutex, INFINITE);

			dados.cli[0].clickTipo = 4;

			//escreve no named pipe
			//for (int i = 0; i < dados->nClientes; i++) {
			ZeroMemory(&ov, sizeof(ov));
			ov.hEvent = hEvento;

			WriteFile(dados.hPipeCli, (LPVOID)&dados.cli[0], sizeof(dados.cli[0]), &n, &ov);
			//WaitForSingleObject(ov.hEvent, INFINITE);
			//GetOverlappedResult(dados.hPipeCli, &ov, n, FALSE);

			/*if (!WriteFile(dados.hPipeCli, (LPVOID)&dados.cli[0], sizeof(dados.cli[0]), &n, NULL)) {
				_tprintf(TEXT("[ERRO] Escrever no pipe! (WriteFile)\n"));
				exit(-1);
			}*/

			ReleaseMutex(dados.hMutex);

			SetEvent(dados.leDoCliente);

			DestroyWindow(hWnd);

		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		GetClientRect(hWnd, &rect);
		//SetTextColor(hdc, RGB(255, 255, 255));
		//SetBkMode(hdc, TRANSPARENT);


		WaitForSingleObject(dados.hMutex, INFINITE);

		if (dados.cli[0].clis == 2)
			joga = TRUE;

		if (joga) {

			int Lx1 = 40, Lx2 = 75; //75
			int Cy1 = 80, Cy2 = 115; // 115
			for (int i = 0; i < dados.cli[0].mapaJogo.tamMap; i++) {
				for (int j = 0; j < dados.cli[0].mapaJogo.tamMap; j++) {
					Rectangle(hdc, Lx1, Cy1, Lx2, Cy2);

					if (dados.cli[0].mapaJogo.mapa[i][j].tubos == _T('I')) {
						if (dados.cli[0].mapaJogo.mapa[i][j].agua)
							BitBlt(hdc, Lx1, Cy1, bmp[14].bmWidth, bmp[14].bmHeight, bmpDC[14], 0, 0, SRCCOPY);
						else
							BitBlt(hdc, Lx1, Cy1, bmp[12].bmWidth, bmp[12].bmHeight, bmpDC[12], 0, 0, SRCCOPY);
					}
					else if (dados.cli[0].mapaJogo.mapa[i][j].tubos == _T('D'))
						BitBlt(hdc, Lx1, Cy1, bmp[13].bmWidth, bmp[13].bmHeight, bmpDC[13], 0, 0, SRCCOPY);

					else if (dados.cli[0].mapaJogo.mapa[i][j].tubos == dados.cli[0].pecas[0])
						BitBlt(hdc, Lx1, Cy1, bmp[0].bmWidth, bmp[0].bmHeight, bmpDC[0], 0, 0, SRCCOPY);
					else if (dados.cli[0].mapaJogo.mapa[i][j].tubos == dados.cli[0].pecas[1])
						BitBlt(hdc, Lx1, Cy1, bmp[1].bmWidth, bmp[1].bmHeight, bmpDC[1], 0, 0, SRCCOPY);
					else if (dados.cli[0].mapaJogo.mapa[i][j].tubos == dados.cli[0].pecas[2])
						BitBlt(hdc, Lx1, Cy1, bmp[2].bmWidth, bmp[2].bmHeight, bmpDC[2], 0, 0, SRCCOPY); //upper left //30 de cima ate ao 1 // de lado 40 ate ao primeiro
					else if (dados.cli[0].mapaJogo.mapa[i][j].tubos == dados.cli[0].pecas[3])
						BitBlt(hdc, Lx1, Cy1, bmp[3].bmWidth, bmp[3].bmHeight, bmpDC[3], 0, 0, SRCCOPY);
					else if (dados.cli[0].mapaJogo.mapa[i][j].tubos == dados.cli[0].pecas[4])
						BitBlt(hdc, Lx1, Cy1, bmp[4].bmWidth, bmp[4].bmHeight, bmpDC[4], 0, 0, SRCCOPY);
					else if (dados.cli[0].mapaJogo.mapa[i][j].tubos == dados.cli[0].pecas[5])
						BitBlt(hdc, Lx1, Cy1, bmp[5].bmWidth, bmp[5].bmHeight, bmpDC[5], 0, 0, SRCCOPY);

					else if (dados.cli[0].mapaJogo.mapa[i][j].tubos == dados.cli[0].pecas[6])
						BitBlt(hdc, Lx1, Cy1, bmp[6].bmWidth, bmp[6].bmHeight, bmpDC[6], 0, 0, SRCCOPY);
					else if (dados.cli[0].mapaJogo.mapa[i][j].tubos == dados.cli[0].pecas[7])
						BitBlt(hdc, Lx1, Cy1, bmp[7].bmWidth, bmp[7].bmHeight, bmpDC[7], 0, 0, SRCCOPY);
					else if (dados.cli[0].mapaJogo.mapa[i][j].tubos == dados.cli[0].pecas[8])
						BitBlt(hdc, Lx1, Cy1, bmp[8].bmWidth, bmp[8].bmHeight, bmpDC[8], 0, 0, SRCCOPY); //upper left //30 de cima ate ao 1 //de lado 40 ate ao primeiro
					else if (dados.cli[0].mapaJogo.mapa[i][j].tubos == dados.cli[0].pecas[9])
						BitBlt(hdc, Lx1, Cy1, bmp[9].bmWidth, bmp[9].bmHeight, bmpDC[9], 0, 0, SRCCOPY);
					else if (dados.cli[0].mapaJogo.mapa[i][j].tubos == dados.cli[0].pecas[10])
						BitBlt(hdc, Lx1, Cy1, bmp[10].bmWidth, bmp[10].bmHeight, bmpDC[10], 0, 0, SRCCOPY);
					else if (dados.cli[0].mapaJogo.mapa[i][j].tubos == dados.cli[0].pecas[11])
						BitBlt(hdc, Lx1, Cy1, bmp[11].bmWidth, bmp[11].bmHeight, bmpDC[11], 0, 0, SRCCOPY);
					else if (dados.cli[0].mapaJogo.mapa[i][j].tubos == dados.cli[0].pecas[12])
						BitBlt(hdc, Lx1, Cy1, bmp[15].bmWidth, bmp[15].bmHeight, bmpDC[15], 0, 0, SRCCOPY);

					Lx1 = Lx2;
					Lx2 = Lx2 + 35;
				}
				Cy1 = Cy2;
				Cy2 = Cy2 + 35;
				Lx1 = 40, Lx2 = 75;
			}

			if (dados.cli[0].mapaJogo.tamMap > 0 && !dados.cli[0].aleatorio) {

				DWORD x = 40 + ( 35 * dados.cli[0].mapaJogo.tamMap)  + 100, y = 120;
				for (int i = 0; i < 6; i++) {

					if (dados.cli[0].ordem[i] == dados.cli[0].pecas[0])
						BitBlt(hdc, x, y, bmp[0].bmWidth, bmp[0].bmHeight, bmpDC[0], 0, 0, SRCCOPY);
					else if (dados.cli[0].ordem[i] == dados.cli[0].pecas[1])
						BitBlt(hdc, x, y, bmp[1].bmWidth, bmp[1].bmHeight, bmpDC[1], 0, 0, SRCCOPY);
					else if (dados.cli[0].ordem[i] == dados.cli[0].pecas[2])
						BitBlt(hdc, x, y, bmp[2].bmWidth, bmp[2].bmHeight, bmpDC[2], 0, 0, SRCCOPY);
					else if (dados.cli[0].ordem[i] == dados.cli[0].pecas[3])
						BitBlt(hdc, x, y, bmp[3].bmWidth, bmp[3].bmHeight, bmpDC[3], 0, 0, SRCCOPY);
					else if (dados.cli[0].ordem[i] == dados.cli[0].pecas[4])
						BitBlt(hdc, x, y, bmp[4].bmWidth, bmp[4].bmHeight, bmpDC[4], 0, 0, SRCCOPY);
					else if (dados.cli[0].ordem[i] == dados.cli[0].pecas[5])
						BitBlt(hdc, x, y, bmp[5].bmWidth, bmp[5].bmHeight, bmpDC[5], 0, 0, SRCCOPY);
					//BitBlt(hdc, x, y, bmp[(i+dados.msgCli.totalClicks) % 6].bmWidth, bmp[(i + dados.msgCli.totalClicks) % 6].bmHeight, bmpDC[(i + dados.msgCli.totalClicks) % 6], 0, 0, SRCCOPY);
					y += 50;
				}
			}

			if (nivel != dados.cli[0].nivel) {
				nivel = dados.cli[0].nivel;
				TCHAR texto[10];
				wsprintf(texto, _T("Nivel %d"), nivel);
				SetWindowTextW(hNivel, texto);
			}

		}

		ReleaseMutex(dados.hMutex);

		//BitBlt(hdc, 0, 0, bmp.bmWidth, bmp.bmHeight, bmpDC, 0, 0, SRCCOPY);

		EndPaint(hWnd, &ps);

		break;
	case WM_CHAR:
		c = (TCHAR)wParam;
		break;

	case WM_HELP:
		MessageBox(hWnd, _T("janela de ajuda"), _T("Sair"), MB_OK);
		break;
	case WM_DESTROY:	// Destruir a janela e terminar o programa 		// "PostQuitMessage(Exit Status)"	

		PostQuitMessage(0);
		break;

	default:
		// Neste exemplo, para qualquer outra mensagem (p.e. "minimizar","maximizar","restaurar")
		// não é efectuado nenhum processamento, apenas se segue o "default" do Windows
		return(DefWindowProc(hWnd, messg, wParam, lParam));
		break;  // break tecnicamente desnecessário por causa do return
	}
	return(0);
}