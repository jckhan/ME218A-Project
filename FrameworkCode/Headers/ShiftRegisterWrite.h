// create your own header file comment block
// and protect against multiple inclusions

uint8_t LocalRegisterImage;
void SR_Init(void);
uint8_t SR_GetCurrentRegister(void);
void LED_SR_Write(uint8_t NewValue);
void AUDIO_SR_Write(uint8_t NewValue);