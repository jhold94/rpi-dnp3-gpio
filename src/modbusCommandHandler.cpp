#include <modbus/modbus.h>
#include <modbus/modbus-tcp.h>
#include <stdint.h>
#include <cstring>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <iostream>

#include "sources/modbusCommands.h"

#define MAX_READ_BITS 10
#define MAX_READ_REGISTERS 80

uint8_t *tab_bit;
uint8_t *tab_input_bit;
uint16_t *tab_reg;
uint16_t *tab_hold_reg;
modbus_t *ctx;
int nb_points;
int nb_points_reg;




void modbus_init(void)
{
        ctx = modbus_new_tcp("192.168.1.100", 2000);
        
        modbus_set_slave(ctx, 5);
        
        if(modbus_connect(ctx) == -1) {
                modbus_free(ctx);
        }
        
        /* Allocate and initialize the memory to store Digital Output Bits */
        tab_bit = (uint8_t *) malloc(MAX_READ_BITS * sizeof(uint8_t));
        memset(tab_bit, 0, MAX_READ_BITS * sizeof(uint8_t));
        
        /* Allocate and initialize the memory to store Digital Input Bits */
        tab_input_bit = (uint8_t *) malloc(MAX_READ_BITS * sizeof(uint8_t));
        memset(tab_input_bit, 0, MAX_READ_BITS * sizeof(uint8_t));
        
        /* Allocate and initialize the memory to store Analog Input Registers */
        tab_reg = (uint16_t *) malloc(MAX_READ_REGISTERS * sizeof(uint16_t));
        memset(tab_reg, 0, MAX_READ_REGISTERS * sizeof(uint16_t));
        
        /* Allocate and initialize the memory to store Analog Holding Registers */
        tab_hold_reg = (uint16_t *) malloc(MAX_READ_REGISTERS * sizeof(uint16_t));
        memset(tab_hold_reg, 0, MAX_READ_REGISTERS * sizeof(uint16_t));
}

int dmReadBit(int index)
{
        index = index - 10000;
        
        nb_points = MAX_READ_BITS;
        modbus_read_input_bits(ctx, 0, nb_points, tab_input_bit);
        
        return tab_input_bit[index];
}

int dmReadOutBit(int index)
{        
        index = index - 1000;
        
        nb_points = MAX_READ_BITS;
        modbus_read_bits(ctx, 0, nb_points, tab_bit);
        
        return tab_bit[index];        
}

void dmWriteBit(int index, bool state)
{
        index = index - 1000; 
        
        modbus_write_bit(ctx, index, state);
}

long dmReadReg(int index)
{
        index = index - 30000;
        
        nb_points_reg = MAX_READ_REGISTERS;
        modbus_read_input_registers(ctx, 0, nb_points_reg, tab_reg);
                
        return tab_reg[index];        
}

long dmReadHoldReg(int index)
{
        index = index - 40000;
        
        nb_points_reg = MAX_READ_REGISTERS;
        modbus_read_registers(ctx, 0, nb_points_reg, tab_hold_reg);
        
        return tab_hold_reg[index];
}

void dmWriteReg(int index, long value)
{
        index = index - 40000;
        
        modbus_write_register(ctx, index, value);
}

void modbus_exit(void)
{
        modbus_close(ctx);
        modbus_free(ctx);
}
        
