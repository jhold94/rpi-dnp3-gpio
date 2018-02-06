#include <modbus/modbus.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>

#include "sources/modbusCommands.h"

void dmWriteBit(int index, bool state)
{
        modbus_write_bit(mb, index, state);
}

int dmReadBit(int index)
{
        index = index - 1000;
        
        int state;
        uint8_t = inbit_tab[100];
        modbus_read_bits(mb, index, index, inbit_tab);
        
        state = inbit_tab[index];
        
        return state;
}

int dmReadOutBit(int index)
{
        index = index - 1000;
        
        int state;
        uint8_t outbit_tab[100]
        modbus_read_bits(mb, index, index, outbit_tab);
        
        state = outbit_tab[index];
        
        return state;
}

int dmReadInReg(int index)
{
        index = index - 1000;
        
        int value;
        uint16_t inreg_tab[100];
        modbus_read_input_registers(mb, index, index, inreg_tab);
        
        value = inreg_tab[index];
        
        return value;
}
