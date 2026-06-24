/*------------------------------------------------------------
* FileName: main.c
* Author: LuX
* Date: 2013-10-24
* Example of how to use Prolin OSAL API. It can only run on Prolin OS 2.4 or higher.
*
*                            Warning
* This code is for reference only. I do not guarantee the correctness, safety, efficiency or completeness.
* You are welcome to send an e-mail to me(lux@paxsz.com) if you have any questions.
------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <osal.h>
#include <xui.h>
#include "stdbool.h"

#include <ppc_type.h>
#include <ppcomp.h>
#include <ppcomp_ex.h>

#include <updateTables.h>
#include <inputValor.h>

static int GuiInit(int statusbar_height)
{
    char value[16];
    char rotate_str[32];
    char statusbar_str[32];
    int ret;
    char *xui_argv[10];
    int xui_argc;

    ret = OsRegGetValue("ro.fac.lcd.rotate", value);
    if (ret > 0) {
        snprintf(rotate_str, sizeof(rotate_str), "ROTATE=%s", value);
    }
    else {
        strcpy(rotate_str, "ROTATE=0");
    }

    xui_argv[0] = rotate_str;
    xui_argv[1] = statusbar_str;
    xui_argv[2] = NULL;
    xui_argc = 2;

    ret = XuiOpen(xui_argc, xui_argv);
    if (ret == XUI_RET_OK) {
        return RET_OK;
    }
    else {
        return -1;
    }
}

static void GuiDeinit(void)
{
    XuiClose();
}

static void CrashReportInit(void)
{
    signal(SIGILL,    OsSaveCrashReport);
    signal(SIGABRT,   OsSaveCrashReport);
    signal(SIGBUS,    OsSaveCrashReport);
    signal(SIGFPE,    OsSaveCrashReport);
    signal(SIGSEGV,   OsSaveCrashReport);
    signal(SIGSTKFLT, OsSaveCrashReport);
    signal(SIGPIPE,   OsSaveCrashReport);
}

int closeBc( void ) {
	int iRet = PP_Close("");
	OsLog(LOG_DEBUG, "[PP_Close]");

	OsLog(LOG_DEBUG, "Return PP_Close: %d", iRet);
	return iRet;
}


int getCard(char *szInput, char *szOutput){

	int ret;
	char szAuxInput[42], szTimeStamp[11];

	ST_TIME stTime;
	char szNotify[32+1]={0};

	char g_szDateTransaction[11];
	char g_szTimeTransaction[9];

	memset(szTimeStamp, 0, sizeof(szTimeStamp));
	memset(&stTime, 0, sizeof(stTime));

	OsGetTime(&stTime);
	sprintf(g_szDateTransaction, "%02d/%02d/%d", stTime.Day, stTime.Month, stTime.Year);
	sprintf(g_szTimeTransaction, "%02d:%02d:%02d", stTime.Hour, stTime.Minute, stTime.Second);

	PP_GetTimeStamp("00", szTimeStamp); // Bytes [28..37] → TimeStamp da Biblioteca (10 bytes) -> Copia 10 bytes do timestamp gerado pela biblioteca

	memcpy(szAuxInput, szInput, 16); // copia os primeiros 16 bytes de szInput (param. den entrada)

	// Resultado: YYMMDDHHmmss (12 caracteres)
	sprintf(szAuxInput+16, "%02d%02d%02d%02d%02d%02d", stTime.Year % 100, stTime.Month, stTime.Day, stTime.Hour, stTime.Minute, stTime.Second);

	memcpy(szAuxInput+28, szTimeStamp, 10);
	memcpy(szAuxInput+38, "00", 2); // Coloca "00" → possivelmente flags, versão ou espaço reservado.
	memcpy(szAuxInput+40, "0", 1); // Byte [40] → Flag Final (1 byte)
	szAuxInput[40] = '\0';

	/*
	 * Nesse fluxo acima estamos passando dados da transação(16 bytes)
	 * Data e hora local(12 bytes)
	 * Token de sicronização(10 bytes)
	 * Flags de controle(2 bytes)
	 * Terminador da string(1 byte null)
	 *
	 * Total: 41 bytes + '\0' = 42 bytes
	 *
	┌─────────────────────────────────────────────────────────────┐
	│ szAuxInput[42] - ESTRUTURA COMPLETA                         │
	├──────────┬──────────┬──────────────┬──────────┬──────┬──────┤
	│ szInput  │   Data   │ TimeStamp    │ Padding  │ "00" │ "0"  │
	│ 16 bytes │ 12 bytes │ 10 bytes     │ 2 bytes  │      │      │
	│[0..15]   │[16..27]  │[28..37]      │[38..39]  │[40]  │      │
	└──────────┴──────────┴──────────────┴──────────┴──────┴──────┘*/

	ret = PP_StartGetCard(szAuxInput);
	OsLog(LOG_DEBUG,"PP_StartGetCard = %d", ret);

	while( ret == 1 || ret == 41 || ret == 2 ){
		ret = PP_GetCard(szOutput, szNotify);
		if(ret == 20){
			ret = PP_ResumeGetCard();
			OsLog(LOG_DEBUG, "PP_ResumeGetCard() %d", ret);
			ret = 1;
		}
		else if(ret == 86){
			OsLog(LOG_DEBUG,"Condicao else");
			PP_StartGetCard(szAuxInput);
			ret = 1;
		}
	return ret;
	}
}

int HelloWorld(void)
{
	XuiColor colorWhite = {0xFF,0xFF,0xFF,0xFF};
    XuiFont *font = XuiCreateFont("/usr/font/paxfont.ttf",0,0);

	int ret = PP_Open(NULL);
	if(ret != 0) return 0;

	XuiCanvasDrawText(XuiRootCanvas(), 10, 10, 20, font, XUI_TEXT_NORMAL,colorWhite,"Setis BC");

	OsLog(LOG_DEBUG, "PP_Open: %d", ret);

	char output[256] = {0};

	PP_GetInfo("00", output);
	OsLog(LOG_DEBUG,"00: %s", output);

	OsLog(LOG_DEBUG,"");
	OsLog(LOG_DEBUG,"================");
	OsLog(LOG_DEBUG,"CARREGAMENTO DE TABELAS");
	OsLog(LOG_DEBUG,"================");

	updateTables();

	OsLog(LOG_DEBUG,"================");
	OsLog(LOG_DEBUG,"CARREGAMENTO FINALIZADO");
	OsLog(LOG_DEBUG,"================");
	OsLog(LOG_DEBUG,"");


	while(1) {
		OsLog(LOG_DEBUG,"================");
		OsLog(LOG_DEBUG,"FUNC.CheckEventIn 1110");
		OsLog(LOG_DEBUG,"================");

		char *checkEventIn = "1110"; /* 1 verif. pressionamento de tecla
									 1 verif. passag. cart. magnetico
									 1 verif. remoção de cart. com chip
									 0 nao ativa a antena (1 ativa antena e veri. a presença de cartao CTLS)*/
		char retCKE[100];

		ret = checkEvents(checkEventIn, retCKE);

		OsLog(LOG_DEBUG,"================");
		OsLog(LOG_DEBUG,"RET.CheckEventIn = %d", ret);
		OsLog(LOG_DEBUG,"================");
		OsLog(LOG_DEBUG,"");
		clearTransBuf();

	if (ret == 0){
		if (retCKE[0] == '0') {
			// strstr essa expressão procura por dois códigos específicos dentro da string retCKE
			if (strstr(retCKE, "000") || strstr(retCKE, "004") ) {
				OsLog(LOG_DEBUG,"");
				OsLog(LOG_DEBUG,"Capturado 000 ou 004");
				OsLog(LOG_DEBUG,"");
				//screen = 1;
				break;
			}
			else if(strstr(retCKE, "013")) {
				OsLog(LOG_DEBUG,"");
				OsLog(LOG_DEBUG,"Capturado 013");
				OsLog(LOG_DEBUG,"");
				//screen = 0;
				break;
			}
		}
		else if (retCKE[0] == '2'){
			OsLog(LOG_DEBUG,"");
			OsLog(LOG_DEBUG,"Capturado 2");
			OsLog(LOG_DEBUG,"");
			/*transaction_type_val = TRANSACTION_CARD_INI;
			inputValorIni();*/
			}
		}else if (ret >= 1){
			OsLog(LOG_DEBUG,"");
			OsLog(LOG_DEBUG,"Capturado ret >= 1");
			OsLog(LOG_DEBUG,"");
			/*transaction_type_val = TRANSACTION_NOCARD_INI;
			inputValorIni();*/
		}
	}

	/* char szInputGetCard[16], szOutputGetCard[400], szAuxAmount[13], adq_TranAmount[13];

	// 04 - Função 1
	// Evento leitura da Tarja Magnética: 002-003 N2 Tamanho da trilha 1.

	memcpy(szInputGetCard, "04", 2);  	// szInputGetCard = 04000...
	memcpy(szInputGetCard+2, "01", 2);  // szInputGetCard = 0401000...

	padLeft(adq_TranAmount, 12, '0', szAuxAmount);
	memcpy(szInputGetCard+2+2, szAuxAmount, 12);

	ret = getCard(szInputGetCard, szOutputGetCard);
	OsLog(LOG_DEBUG, "getCard: %d", ret); */


	closeBc();

    while(1) {

        if (!XuiHasKey()) {
            continue;
        }
        int key = XuiGetKey();
        if (key == XUI_KEYCANCEL) {

        	ret = PP_Close("PP_close");

        	OsLog(LOG_DEBUG, "PP_Close: %d", ret);
            break;
        }
    }

    return 0;
}


int main(int argc, char **argv)
{
	OsLogSetTag("== LOG ==");

    CrashReportInit();
    GuiInit(18);
    HelloWorld();
    GuiDeinit();
    return 0;
}
