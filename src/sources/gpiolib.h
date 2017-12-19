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
extern    void digitalWrite(int pin, bool value);
extern    int digitalRead(int pin);
  
//extern    int dac(int dacpin, int value);
//extern    int analogInMode(int adcpin, int mode);

  
  
#ifdef __cplusplus
}
#endif
#endif //_GPIOLIB_H_