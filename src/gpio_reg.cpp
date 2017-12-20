
#include <stdio.h>
#include <unistd.h>
#include <dirent.h> 
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>
#include <assert.h>

#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <linux/types.h>
#include <math.h>

#include "fpga.h"
#include "gpiolib.h"
#include "crossbar-ts7680.h"

static int twifd;


#include "i2c-dev.h"

int fpga_init(char *path, char adr)
{
	static int fd = -1;

	if(fd != -1)
		return fd;

	if(path == NULL) {
		// Will always be I2C0 on the 7680
		fd = open("/dev/i2c-0", O_RDWR);
	} else {
		
		fd = open(path, O_RDWR);
	}

	if(!adr) adr = 0x28;

	if(fd != -1) {
		if (ioctl(fd, I2C_SLAVE_FORCE, 0x28) < 0) {
			perror("FPGA did not ACK 0x28\n");
			return -1;
		}
	}

	return fd;
}

void fpoke8(int twifd, uint16_t addr, uint8_t value)
{
	uint8_t data[3];
	data[0] = ((addr >> 8) & 0xff);
	data[1] = (addr & 0xff);
	data[2] = value;
	if (write(twifd, data, 3) != 3) {
		perror("I2C Write Failed");
	}
}

uint8_t fpeek8(int twifd, uint16_t addr)
{
	uint8_t data[2];
	data[0] = ((addr >> 8) & 0xff);
	data[1] = (addr & 0xff);
	if (write(twifd, data, 2) != 2) {
		perror("I2C Address set Failed");
	}
	read(twifd, data, 1);

	return data[0];
}

int get_model()
{
	FILE *proc;
	char mdl[256];
	char *ptr;

	proc = fopen("/proc/device-tree/model", "r");
	if (!proc) {
		perror("model");
		return 0;
	}
	fread(mdl, 256, 1, proc);
	ptr = strstr(mdl, "TS-");
	return strtoull(ptr+3, NULL, 16);
}

void usage(char **argv) {
	fprintf(stderr,
		"Usage: %s [OPTIONS] ...\n"
		"Technologic Systems I2C FPGA Utility\n"
		"\n"
		"  -i, --info             Display board info\n"
		"  -m, --addr <address>   Sets up the address for a peek/poke\n"
		"  -v, --poke <value>     Writes the value to the specified address\n"
		"  -t, --peek             Reads from the specified address\n"
		"\n",
		argv[0]
	);
}

int main(int argc, char **argv) 
{
	int c, i;
	uint16_t addr = 0x0;
	int opt_addr = 0;
	int opt_poke = 0, opt_peek = 0, opt_auto485 = -1;
	int opt_set = 0, opt_get = 0, opt_dump = 0;
	int opt_info = 0, opt_setmac = 0, opt_getmac = 0;
	int opt_cputemp = 0, opt_modbuspoweron = 0, opt_modbuspoweroff = 0;
	int opt_dac0 = 0, opt_dac1 = 0, opt_dac2 = 0, opt_dac3 = 0;
	char *opt_mac = NULL;
	int baud = 0;
	int model;
	uint8_t pokeval = 0;
	char *uartmode = 0;
	struct cbarpin *cbar_inputs, *cbar_outputs;
	int cbar_size, cbar_mask;

	static struct option long_options[] = {
		{ "addr", 1, 0, 'm' },
		{ "address", 1, 0, 'm' },
		{ "poke", 1, 0, 'v' },
		{ "peek", 1, 0, 't' },
		{ "pokef", 1, 0, 'v' },
		{ "peekf", 1, 0, 't' },
		{ "mode", 1, 0, 'i' },
		{ 0, 0, 0, 0 }
	};
	model = get_model();
	if(model == 0x7680) {
		cbar_inputs = ts7680_inputs;
		cbar_outputs = ts7680_outputs;
		cbar_size = 6;
		cbar_mask = 3;
	} else {
		fprintf(stderr, "Unsupported model TS-%X\n", model);
		return 1;
	}

	while((c = getopt_long(argc, argv, "+m:v:o:x:ta:cgsqhipl:e1Zb:d:f:j:",
	  long_options, NULL)) != -1) {
		switch(c) {

		case 'i':
			opt_info = 1;
			break;
		case 'm':
			opt_addr = 1;
			addr = strtoull(optarg, NULL, 0);
			break;
		case 'v':
			opt_poke = 1;
			pokeval = strtoull(optarg, NULL, 0);
			break;
		case 't':
			addr = strtoull(optarg, NULL, 0);
      opt_peek = 1;
			break;
		default:
			usage(argv);
			return 1;
		}
	}
  
  twifd = fpga_init(NULL, 0);
	if(twifd == -1) {
		perror("Can't open FPGA I2C bus");
		return 1;
	}

	
	if(opt_info) {
		printf("model=0x%X\n", model);
		gpio_export(44);
		printf("bootmode=0x%X\n", gpio_read(44) ? 1 : 0);
		printf("fpga_revision=0x%X\n", fpeek8(twifd, 0x7F));
	}

	if(opt_get) {
		for (i = 0; cbar_inputs[i].name != 0; i++)
		{
			uint8_t mode = fpeek8(twifd, cbar_inputs[i].addr) >> (8 - cbar_size);
			printf("%s=%s\n", cbar_inputs[i].name, cbar_outputs[mode].name);
		}
	}

	if(opt_set) {
		for (i = 0; cbar_inputs[i].name != 0; i++)
		{
			char *value = getenv(cbar_inputs[i].name);
			int j;
			if(value != NULL) {
				for (j = 0; cbar_outputs[j].name != 0; j++) {
					if(strcmp(cbar_outputs[j].name, value) == 0) {
						int mode = cbar_outputs[j].addr;
						uint8_t val = fpeek8(twifd, cbar_inputs[i].addr);
						fpoke8(twifd, cbar_inputs[i].addr, 
							   (mode << (8 - cbar_size)) | (val & cbar_mask));

						break;
					}
				}
				if(cbar_outputs[i].name == 0) {
					fprintf(stderr, "Invalid value \"%s\" for input %s\n",
						value, cbar_inputs[i].name);
				}
			}
		}
	}

	if(opt_dump) {
		i = 0;
		printf("%13s (DIR) (VAL) FPGA Output\n", "FPGA Pad");
		for (i = 0; cbar_inputs[i].name != 0; i++)
		{
			uint8_t value = fpeek8(twifd, cbar_inputs[i].addr);
			uint8_t mode = value >> (8 - cbar_size);
			char *dir = value & 0x1 ? "out" : "in";
			int val;

			// 4900 uses 5 bits for cbar, 7970/7990 use 6 and share
			// the data bit for input/output
			if(value & 0x1 || cbar_size == 6) {
				val = value & 0x2 ? 1 : 0;
			} else {
				val = value & 0x4 ? 1 : 0;
			}
			printf("%13s (%3s) (%3d) %s\n", 
				cbar_inputs[i].name,
				dir,
				val,
				cbar_outputs[mode].name);
		}
	}

	if(opt_modbuspoweron) {
		gpio_export(45);
		gpio_export(46);
		gpio_export(47);
		
		gpio_write(45, 0);
		gpio_write(47, 1);

		gpio_direction(45, 1);
		gpio_direction(46, 0);
		gpio_direction(47, 1);

		gpio_write(47, 0);
		usleep(10000);

		if(gpio_read(46)) {
			gpio_write(47, 1);
			printf("modbuspoweron=0\n");
		} else {
			gpio_write(47, 1);
			gpio_write(45, 1);
			printf("modbuspoweron=1\n");
		}

	}	

	if(opt_modbuspoweroff) {
		gpio_export(45);
		gpio_write(45, 0);
		gpio_direction(45, 1);
	}

	if(opt_poke) {
		fpoke8(twifd, addr, pokeval);
	}

	if(opt_peek) {
		printf("0x%X\n", fpeek8(twifd, addr));
	}

	if(opt_auto485 > -1) {
		auto485_en(opt_auto485, baud, uartmode);
	}

	if (opt_cputemp) {
		signed int temp[2] = {0,0}, x;
		volatile unsigned int *mxlradcregs;
		int devmem;

		devmem = open("/dev/mem", O_RDWR|O_SYNC);
		assert(devmem != -1);
		mxlradcregs = (unsigned int *) mmap(0, getpagesize(),
		  PROT_READ | PROT_WRITE, MAP_SHARED, devmem, 0x80050000);

		mxlradcregs[0x148/4] = 0xFF;
		mxlradcregs[0x144/4] = 0x98; //Set to temp sense mode
		mxlradcregs[0x28/4] = 0x8300; //Enable temp sense block
		mxlradcregs[0x50/4] = 0x0; //Clear ch0 reg
		mxlradcregs[0x60/4] = 0x0; //Clear ch1 reg
		temp[0] = temp[1] = 0;

		for(x = 0; x < 10; x++) {
			/* Clear interrupts
			 * Schedule readings
			 * Poll for sample completion
			 * Pull out samples*/
			mxlradcregs[0x18/4] = 0x3;
			mxlradcregs[0x4/4] = 0x3;
			while(!((mxlradcregs[0x10/4] & 0x3) == 0x3)) ;
			temp[0] += mxlradcregs[0x60/4] & 0xFFFF;
			temp[1] += mxlradcregs[0x50/4] & 0xFFFF;
		}
		temp[0] = (((temp[0] - temp[1]) * (1012/4)) - 2730000);
		printf("internal_temp=%d.%d\n",temp[0] / 10000,
		  abs(temp[0] % 10000));

		munmap((void *)mxlradcregs, getpagesize());
		close(devmem);
	}

	if (opt_setmac) {
		/* This uses one time programmable memory. */
		unsigned int a, b, c;
		int r, devmem;
		volatile unsigned int *mxocotpregs;

		devmem = open("/dev/mem", O_RDWR|O_SYNC);
		assert(devmem != -1);

		r = sscanf(opt_mac, "%*x:%*x:%*x:%x:%x:%x",  &a,&b,&c);
		assert(r == 3); /* XXX: user arg problem */

		mxocotpregs = (unsigned int *) mmap(0, getpagesize(),
		  PROT_READ | PROT_WRITE, MAP_SHARED, devmem, 0x8002C000);

		mxocotpregs[0x08/4] = 0x200;
		mxocotpregs[0x0/4] = 0x1000;
		while(mxocotpregs[0x0/4] & 0x100) ; //check busy flag
		if(mxocotpregs[0x20/4] & (0xFFFFFF)) {
			printf("MAC address previously set, cannot set\n");
		} else {
			assert(a < 0x100);
			assert(b < 0x100);
			assert(c < 0x100);
			mxocotpregs[0x0/4] = 0x3E770000;
			mxocotpregs[0x10/4] = (a<<16|b<<8|c);
		}
		mxocotpregs[0x0/4] = 0x0;

		munmap((void *)mxocotpregs, getpagesize());
		close(devmem);
	}

	if (opt_getmac) {
		unsigned char a, b, c;
		unsigned int mac;
		int devmem;
		volatile unsigned int *mxocotpregs;

		devmem = open("/dev/mem", O_RDWR|O_SYNC);
		assert(devmem != -1);
		mxocotpregs = (unsigned int *) mmap(0, getpagesize(),
		  PROT_READ | PROT_WRITE, MAP_SHARED, devmem, 0x8002C000);

		mxocotpregs[0x08/4] = 0x200;
		mxocotpregs[0x0/4] = 0x1000;
		while(mxocotpregs[0x0/4] & 0x100) ; //check busy flag
		mac = mxocotpregs[0x20/4] & 0xFFFFFF;
		if(!mac) {
			mxocotpregs[0x0/4] = 0x0; //Close the reg first
			mxocotpregs[0x08/4] = 0x200;
			mxocotpregs[0x0/4] = 0x1013;
			while(mxocotpregs[0x0/4] & 0x100) ; //check busy flag
			mac = (unsigned short) mxocotpregs[0x150/4];
			mac |= 0x4f0000;
		}
		mxocotpregs[0x0/4] = 0x0;

		a = mac >> 16;
		b = mac >> 8;
		c = mac;
		
		printf("mac=00:d0:69:%02x:%02x:%02x\n", a, b, c);
		printf("shortmac=%02x%02x%02x\n", a, b, c);

		munmap((void *)mxocotpregs, getpagesize());
		close(devmem);
	}

	if(opt_dac0) {
		char buf[2];
		buf[0] = ((opt_dac0 >> 9) & 0xf);
		buf[1] = ((opt_dac0 >> 1) & 0xff);
		fpoke8(twifd, 0x2E, buf[0]);
		fpoke8(twifd, 0x2F, buf[1]);
	}

	if(opt_dac1) {
		char buf[2];
		buf[0] = ((opt_dac1 >> 9) & 0xf);
		buf[1] = ((opt_dac1 >> 1) & 0xff);
		fpoke8(twifd, 0x30, buf[0]);
		fpoke8(twifd, 0x31, buf[1]);
	}

	if(opt_dac2) {
		char buf[2];
		buf[0] = ((opt_dac2 >> 9) & 0xf);
		buf[1] = ((opt_dac2 >> 1) & 0xff);
		fpoke8(twifd, 0x32, buf[0]);
		fpoke8(twifd, 0x33, buf[1]);
	}

	if(opt_dac3) {
		char buf[2];
		buf[0] = ((opt_dac3 >> 9) & 0xf);
		buf[1] = ((opt_dac3 >> 1) & 0xff);
		fpoke8(twifd, 0x34, buf[0]);
		fpoke8(twifd, 0x35, buf[1]);
	}


	close(twifd);

	return 0;
}
  
