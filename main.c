#include "uart.h"
#include "string.h"
#include <LPC21xx.H>
#include "command_decoder.h"
#include "timer_interrupts.h"
#include "servo.h"
#include "adconverter.h"

#define MAX_SERWO_STEP 47
#define MAX_RESULT 1023

struct Watch {
	unsigned char ucMinutes, ucSeconds;
	unsigned char fSeccondsValueChanged, fMinutesValueChanged;
};

struct Watch sWatch = {0, 0, 0, 0};

void WatchUpdate(void) {
	sWatch.ucSeconds++;
	sWatch.fSeccondsValueChanged = 1;
	if (sWatch.ucSeconds == 4) {
		sWatch.ucSeconds = 0;
		sWatch.ucMinutes++;
		sWatch.fMinutesValueChanged = 1;
	}
}

int main() {
	unsigned int uiResult;
	unsigned int uiServoPosition;
	unsigned char fNumberReceived = 0;
	unsigned char fIdReceived = 0;
	unsigned char fUnknownCommand = 0;
	char pcMessage[TRANSMITER_SIZE] = "";
	char pcMessageReceived[RECIEVER_SIZE] = "";
	
	Timer0Interrupts_Init(1000000, &WatchUpdate);
	UART_InitWithInt(9600);
	ServoInit(200, 10);
	ADConverterInit();
	
	while(1) {
		if (Transmiter_GetStatus() == FREE) {
			if (sWatch.fMinutesValueChanged == 2) {
				sWatch.fMinutesValueChanged = 0;
				CopyString("min ", pcMessage);
				AppendUIntToString(sWatch.ucMinutes, pcMessage);
				Transmiter_SendString(pcMessage);
			}
			else if (sWatch.fSeccondsValueChanged == 2) {
				sWatch.fSeccondsValueChanged = 0;
				CopyString("sec ", pcMessage);
				AppendUIntToString(sWatch.ucSeconds, pcMessage);
				Transmiter_SendString(pcMessage);
			}
			else if (fNumberReceived == 1) {
				fNumberReceived = 0;
				CopyString("calc ", pcMessage);
				AppendUIntToString(asToken[1].uValue.uiNumber*2, pcMessage);
				Transmiter_SendString(pcMessage);
			}
			else if (fIdReceived == 1) {
				fIdReceived = 0;
				Transmiter_SendString("id Tomek");
			}
			else if (fUnknownCommand == 1) {
				fUnknownCommand = 0;
				Transmiter_SendString("unknowncommand");
			}
			else if(fADConverter_IfComplete()) {
				uiResult = ADConverter_GetResult();
				CopyString("Voltage(0-0x03FF): ", pcMessage);
				AppendUIntToString(uiResult, pcMessage);
				Transmiter_SendString(pcMessage);
				uiServoPosition = MAX_SERWO_STEP-((uiResult*MAX_SERWO_STEP)/MAX_RESULT);
				ServoGoTo(uiServoPosition);
			}
		}
			
		if (eReciever_GetStatus() == READY) {
			Reciever_GetStringCopy(pcMessageReceived);
			DecodeMsg(pcMessageReceived);
			if (asToken[0].eType == KEYWORD) {
				switch(asToken[0].uValue.eKeyword) {
					case CC:
						fNumberReceived = 1;
					break;
					case ID:
						fIdReceived = 1;
					break;
					case CL:
						ServoCallib();
					break;
					case GT:
						ServoGoTo(asToken[1].uValue.uiNumber);
					break;
					default:
						fUnknownCommand = 1;
					break;
				}	
			}
			else {
				fUnknownCommand = 1;
			}
		}
	}
}
