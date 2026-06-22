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

#include <ppc_type.h>
#include <ppcomp.h>
#include <ppcomp_ex.h>

#include <updateTables.h>

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

int checkEvents(char *szInput, char *szOutput){

	int iRet = PP_StartCheckEvent(szInput);

	OsLog(LOG_DEBUG, "PP_StartCheckEvent(%s) = [%d]", szInput, iRet);
	if(iRet != 0) return iRet;

	do {
		// PP_CheckEvent cartão com tarja, inserção e remoção de cartão com chip.

		iRet = PP_CheckEvent(szOutput);
		if (iRet != 1) OsLog(LOG_DEBUG, "PP_CheckEvent(%s) = [%d]", szOutput, iRet);

		if (!XuiHasKey())
			continue;

		int key = XuiGetKey();
		if (key == XUI_KEYFUNC || key == XUI_KEYENTER){
			PP_Abort();									// PP_Abort cancela a operação em andamento
			sprintf(szOutput, "004");
			return 0;
		}
		else if(key == XUI_KEYALPHA){
			PP_Abort();
			sprintf(szOutput, "005");
			return 0;
		}
		else if(key >= 2 && key <= 11){
			PP_Abort();
			return key-1;
		}
		else {
			PP_Abort();
			sprintf(szOutput, "013");
			return 0;
		}

	} while(iRet == 1);

	OsLog(LOG_DEBUG, "[PP_CheckEvent] return");
	OsLog(LOG_DEBUG, "Output [%s]", szOutput);
	OsLog(LOG_DEBUG, "Retorno: %d \n", iRet);

	return 0;
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

	updateTables();

	char *checkEventIn = "1110"; /* 1 verif. pressionamento de tecla
								 1 verif. passag. cart. magnetico
								 1 verif. remoção de cart. com chip
								 0 nao ativa a antena (1 ativa antena e veri. a presença de cartao CTLS) */
	char retCKE[100];

	checkEvents(checkEventIn, retCKE);

	//closeBc();

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
