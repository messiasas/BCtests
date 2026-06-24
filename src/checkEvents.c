#include <checkEvents.h>
#include <osal.h>
#include <xui.h>
#include "stdbool.h"

#include <ppc_type.h>
#include <ppcomp.h>
#include <ppcomp_ex.h>

void clearTransBuf() {
	inUnitTest = 0; // bool
	transaction_type_aplic = 0; // 1 credito a vista | 2 debito
	transaction_type_val = 0; // cartao presente?

	// Zera memória: Todo o array fica zerado \0\0\0\0...
	memset(adq_NumParc, 0, sizeof(adq_NumParc));
	memset(adq_PINData, 0, sizeof(adq_PINData));
	memset(trans_val_parcel, 0, sizeof(trans_val_parcel));

	sprintf(trans_val_parcel, "000000000000"); // trans_val_parcel[13] preencchendo somente 12 bytes com '0'
	sprintf((char *)adq_PINData, "0000000000000000"); // adq_PINData[16] totalmente preenchido com 16 bytes '0'
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
