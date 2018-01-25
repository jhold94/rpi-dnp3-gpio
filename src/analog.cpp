
#include <iostream>
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "sources/gpiolib.h"

volatile unsigned int *mxlradcregs;
unsigned int i, x;
int devmem;

void analogPinMode(int pin)
{
	if (pin == 0)
	{
		gpio_export(231);
		gpio_direction(231, 1);
		gpio_write(231, 1);
		gpio_unexport(231);
	} else if (pin == 2)
	{
		gpio_export(232);
		gpio_direction(232, 1);
		gpio_write(232, 1);
		gpio_unexport(232);
	} else
	{
		printf("Pin is not supported for current loop");
	}
}

void analog_init(void)
{
	devmem = open("/dev/mem", O_RDWR|O_SYNC);
	assert(devmem != -1);

	mxlradcregs = (unsigned int *) mmap(0, getpagesize(),
	  PROT_READ | PROT_WRITE, MAP_SHARED, devmem, 0x80050000);

	mxlradcregs[0x148/4] = 0xfffffff; //Clear LRADC6:0 assignments
	mxlradcregs[0x144/4] = 0x6543210; //Set LRDAC6:0 to channel 6:0
	mxlradcregs[0x28/4] = 0xff000000; //Set 1.8v range
	for(x = 0; x < 4; x++)
	  mxlradcregs[(0x50+(x * 0x10))/4] = 0x0; //Clear LRADCx reg
}

int analogRead(int pin)
{	
	unsigned long long chan[4] = {0,0,0,0};
	
	for(x = 0; x < 10; x++) {
		mxlradcregs[0x18/4] = 0x7f; //Clear interrupt ready
		mxlradcregs[0x4/4] = 0x7f; //Schedule conversaion of chan 6:0
		while(!((mxlradcregs[0x10/4] & 0x7f) == 0x7f)); //Wait
		chan[pin] += (mxlradcregs[(0x50+(pin * 0x10))/4] & 0xffff);
		//for(i = 0; i < 4; i++)
		  //chan[i] += (mxlradcregs[(0x50+(i * 0x10))/4] & 0xffff);
	}
	
	//float value = chan[pin];
	
        int meas_mV=((((chan[pin]/10)*45177)*6235)/100000000);
	int meas_uA=(((meas_mV)*1000)/240);
	
	//1.1736608125[x] or (281678595/240000000)[x]
	
	return meas_uA;
	//return value;
		
}

/* void analogWrite(int pin, int value)
{
	int twifd = fpga_init(NULL, 0);
	
	int value = value * 360;
	
	int opt_dac = (value & 0xfff)<<1)|0x1;
	
	char buf[2];
	buf[0] = ((opt_dac >> 9) & 0xf);
	buf[1] = ((opt_dac >> 1) & 0xff);
	
	switch(pin)
	{
		case 0:
			fpoke8(twifd, 0x2E, buf[0]);
			fpoke8(twifd, 0x2F, buf[1]);
			break;
		case 1:
			fpoke8(twifd, 0x30, buf[0]);
			fpoke8(twifd, 0x31, buf[1]);
			break;
		case 2:
			fpoke8(twifd, 0x32, buf[0]);
			fpoke8(twifd, 0x33, buf[1]);
			break;
		case 3:
			fpoke8(twifd, 0x34, buf[0]);
			fpoke8(twifd, 0x35, buf[1]);
			break;
		default:
			break;
	}
	
	fpoke(twifd, (0x2E + (2*pin)), buf[0]);
	fpoke(twifd, (0x2F + (2*pin)), buf[1]);
			
} */
