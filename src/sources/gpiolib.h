#ifndef _GPIOLIB_H_
#ifdef __cplusplus
extern "C" {
#endif

  extern    int gpio_direction(int gpio, int dir);
  extern    int gpio_export(int gpio);
  extern    void gpio_unexport(int gpio);
  extern    int gpio_read(int gpio);
  extern    int gpio_write(int gpio, int val);
  extern    void pinMode(int pin, int mode);
  extern    void digitalWrite(int pin, int state);
  extern    int digitalRead(int pin);
  extern    void analogPinMode(int pin);
  extern    int analogRead(int pin);
  extern    int fpga_init(char *path, char adr);
  extern    uint8_t fpeek8(int twifd, uint16_t addr);
  
#ifdef __cplusplus
}
#endif
#endif //_GPIOLIB_H_
