// function prototypes
  void PLLinit(void);
  void initialize(void);
 // register Controll
  void writeReg(uint8_t address, uint8_t data);
  uint8_t readReg(uint8_t address);
  void writeData(uint8_t data);
  uint8_t readData(void);
  void writeCommand(uint8_t data);
  uint8_t readStatus(void);
  
  // Brightness Controll
  void displayOn(bool on);
  void GPIOX(bool on);
  void PWM1config(bool on, uint8_t clock);
  void PWM1out(uint8_t p);
  
  //Rectdraw Test
  void rectHelper(int16_t x, int16_t y, int16_t w, int16_t h,
                                 uint16_t color, bool filled);
  void fillScreen(uint16_t color);
  bool waitPoll(uint8_t regname, uint8_t waitflag);
  
  void fullinitLCD();