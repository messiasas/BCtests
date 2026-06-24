#include <osal.h>
#include <xui.h>
#include "stdbool.h"
#include <inputValor.h>

#include <ppc_type.h>
#include <ppcomp.h>
#include <ppcomp_ex.h>

int inputValorIni() {
	int i, j;
	XuiGetStrAttr strattr;

	memset(adq_TranAmount, 0, sizeof(adq_TranAmount));
	memset(transaction_value, 0, sizeof(transaction_value));

	*transaction_value = "100.50";

	if(XuiGetString(strattr, transaction_value, 0xe7, 3, 13) != 0) {
		if (transaction_type_val == TRANSACTION_CARD_INI) 	// #define TRANSACTION_CARD_INI 0
			OsLog(LOG_DEBUG,"PAGAMENTO CANCELADO");
		return -1;
	}

	for(i = 0, j = 0; i < strlen(transaction_value); i++)
	{
		if(transaction_value[i] != '.') // adq_TranAmount = "10050"      (sem ponto - apenas dígitos)
		{
			adq_TranAmount[j] = transaction_value[i];
			j++;
		}
		OsLog(LOG_DEBUG,"String adquirida: %s",adq_TranAmount);
	}

	//doTransaction();
	//clearScreen();

	return 0;
}
