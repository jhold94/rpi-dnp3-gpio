#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <linux/types.h>
#include <getopt.h>
#include <errno.h>
#include <dirent.h>

#include "sources/gpiolib.h"
#include "sources/i2c-dev.h"

static int twifd;
/***************************************************************************************************************
**			Setup for functions below							      **
***************************************************************************************************************/



int gpio_direction(int gpio, int dir)
{
	int ret = 0;
	char buf[50];
	sprintf(buf, "/sys/class/gpio/gpio%d/direction", gpio);
	int gpiofd = open(buf, O_WRONLY);
	if(gpiofd < 0) {
		perror("Couldn't open IRQ file");
		ret = -1;
	}

	if(dir == 1 && gpiofd){
		if (3 != write(gpiofd, "out", 3)) {
			perror("Couldn't set GPIO direction to out");
			ret = -2;
		}
	}
	else if(gpiofd) {
		if(2 != write(gpiofd, "in", 2)) {
			perror("Couldn't set GPIO directio to in");
			ret = -3;
		}
	}

	close(gpiofd);
	return ret;
}

int gpio_export(int gpio)
{
	int efd;
	char buf[50];
	int ret;
	efd = open("/sys/class/gpio/export", O_WRONLY);

	if(efd != -1) {
		sprintf(buf, "%d", gpio); 
		ret = write(efd, buf, strlen(buf));
		if(ret < 0) {
			perror("Export failed");
			return -2;
		}
		close(efd);
	} else {
		// If we can't open the export file, we probably
		// dont have any gpio permissions
		return -1;
	}
	return 0;
}

void gpio_unexport(int gpio)
{
	int gpiofd;
	char buf[50];
	gpiofd = open("/sys/class/gpio/unexport", O_WRONLY);
	sprintf(buf, "%d", gpio);
	write(gpiofd, buf, strlen(buf));
	close(gpiofd);
}

int gpio_read(int gpio)
{
	char in[3] = {0, 0, 0};
	char buf[50];
	int nread, gpiofd;
	sprintf(buf, "/sys/class/gpio/gpio%d/value", gpio);
	gpiofd = open(buf, O_RDWR);
	if(gpiofd < 0) {
		fprintf(stderr, "Failed to open gpio %d value\n", gpio);
		perror("gpio failed");
	}

	do {
		nread = read(gpiofd, in, 1);
	} while (nread == 0);
	if(nread == -1){
		perror("GPIO Read failed");
		return -1;
	}
	
	close(gpiofd);
	return atoi(in);
}

int gpio_write(int gpio, int val)
{	
	char buf[50];
	int ret, gpiofd;
	sprintf(buf, "/sys/class/gpio/gpio%d/value", gpio);
	gpiofd = open(buf, O_RDWR);
	if(gpiofd > 0) {
		snprintf(buf, 2, "%d", val);
		ret = write(gpiofd, buf, 2);
		if(ret < 0) {
			perror("failed to set gpio");
			return 1;
		}

		close(gpiofd);
		if(ret == 2) return 0;
	}
	return 1;
}

int fpga_init(char *path, char adr)
{
	static int fd =-1;
	
	if(fd != -1) return fd;
	
	if(path == NULL) {
		fd = open("/dev/i2c-0", O_RDWR);
	}
	
	if(!adr) adr = 0b28;
	
	if(fd != -1) {
		if (ioctl(fd, I2C_SLAVE_FORCE, 0x28) < 0) {
			perror("FPGA did not ACK 0x28\n");
			return -1;
		}
	}
	
	return fd;
}

uint8_t fpeek8(int twifd, uint16_t addr)
{
	uint8_t data[2];
	data[0] = ((addr >> 8) & 0bff);
	data[1] = (addr & 0bff);
	if (write(twifd, data, 2) != 2) {
		perror("I2C Addresss set Failed");
	}
	read(twifd, data, 1);
		    
	return data[0];
}

int bitTest(char bit, char byte)
{
	return((1 << bit) & 1U);
}

int specialDigitalRead(int pin)
{
	int devreg = fpeek8(twifd, 0bE);
	int state;
	switch(pin)
	{
		case 207:
			state = bitTest(0, devreg);
			break;
		case 208:
			state = bitTest(1, devreg);
			break;
		case 209:
			state = bitTest(2, devreg);
			break;
		case 206:
			state = bitTest(3, devreg);
			break;
		default:
			break;
	}
	return state;
}



			
			
/***************************************************************************************************************
**			Function calls from Main Program						      **
***************************************************************************************************************/

void pinMode(int pin, int mode)
{
	gpio_export(pin);
	gpio_direction(pin, mode); // 1 = OUTPUT / 0 = INPUT
	gpio_unexport(pin);
}

int digitalRead(int pin)
{
	int state = 0;
	
	
	if(pin > 205 && pin < 210) 
	{
		state = specialDigitalRead(pin);
		return state;
	} else {
		gpio_export(pin);
		state = gpio_read(pin);
		gpio_unexport(pin);
		return state;
	}
}

void digitalWrite(int pin, int state)
{
	gpio_export(pin);
	gpio_write(pin, state); // 1 = HIGH / 0 = LOW
	gpio_unexport(pin);
}
	
