void ServoInit(unsigned int uiServoFrequency, int uiOffset);
void ServoCallib(void);
void ServoGoTo(unsigned int uiPosition);
enum ServoState {CALLIB, IDLE, IN_PROGRESS};
struct Servo
{
	enum ServoState eState;
	unsigned int uiCurrentPosition;
	unsigned int uiDesiredPosition;
	unsigned int uiOffset;
};
extern struct Servo sServo;
