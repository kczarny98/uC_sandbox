#include <LPC21xx.H>
#include "led.h"
#include "timer_interrupts.h"
#include "servo.h"

#define DETECTOR_bm (1<<10)

struct Servo sServo;

enum DetectorState {ACTIVE, INACTIVE};

enum DetectorState eReadDetector() {
	if ((IO0PIN & DETECTOR_bm) == 0) {
		return ACTIVE;
	}
	else {
		return INACTIVE;
	}
}

void DetectorInit() {
	IO0DIR = IO0DIR & (~DETECTOR_bm);
}

void Automat() {

	switch(sServo.eState) {
		case CALLIB:
			if (eReadDetector() == ACTIVE) {
				sServo.eState = IDLE;
				sServo.uiCurrentPosition = sServo.uiOffset;
				sServo.uiDesiredPosition = 0;
			}
			else {
				LedStepLeft();
				sServo.eState = CALLIB;
			}
		break;
		case IDLE:
			if (sServo.uiCurrentPosition != sServo.uiDesiredPosition) {
				sServo.eState = IN_PROGRESS;
			}
			else {
				sServo.eState = IDLE;
			}
		break;
		case IN_PROGRESS:
			if (sServo.uiCurrentPosition < sServo.uiDesiredPosition) {
				LedStepRight();
				sServo.uiCurrentPosition++;
				sServo.eState = IN_PROGRESS;
			}
			else if (sServo.uiCurrentPosition > sServo.uiDesiredPosition) {
				LedStepLeft();
				sServo.uiCurrentPosition--;
				sServo.eState = IN_PROGRESS;
			}
			else {
				sServo.eState = IDLE;
			}
		break;
	}
}

void ServoCallib(void) {
	sServo.eState = CALLIB;
	while (sServo.eState != IDLE) { }
}

void ServoGoTo(unsigned int uiPosition) {
	sServo.uiDesiredPosition = uiPosition;
	sServo.eState = IN_PROGRESS;
	while (sServo.eState != IDLE) { }
}

void ServoInit(unsigned int uiServoFrequency, int uiOffset) {
	if (uiOffset < 0) {
		sServo.uiOffset = 48-uiOffset;
	}
	else {
		sServo.uiOffset = uiOffset;
	}
	sServo.eState = CALLIB;
	LedInit();
	DetectorInit();
	Timer1Interrupts_Init(1000000/uiServoFrequency, &Automat);
	while (sServo.eState != IDLE) { }
}
