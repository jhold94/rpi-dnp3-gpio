#include <modbus/modbus.h>
#include <modbus/modbus-tcp.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <iostream>

#include "sources/modbusCommands.h"

modbus_t *mb;
int s;
//modbus_mapping_t *mb_mapping;

//uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH];
int rc;
int state = 0;

void modbus_init(void)
{
        mb = modbus_new_tcp("192.168.24.24", 2000);
	
	modbus_set_slave(mb, 5);
        
	/*mb_mapping = modbus_mapping_new(10,10,10,10);
	if(mb_mapping == NULL) 
	{
		std::cout << "Failed to allocate the mapping" << std::endl;
		modbus_free(mb);
	}*/
	modbus_connect(mb);
	s = modbus_tcp_listen(mb, 1);
	modbus_tcp_accept(mb, &s);
}

void dmWriteBit(int index, bool state)
{
	modbus_write_bit(mb, index, state);
}

int dmReadBit(int index)
{
	//modbus_init();
	
	/*rc = modbus_receive(mb, query);
	if(rc > 0) 
	{
		modbus_reply(mb, query, rc, mb_mapping);
	}*/
	
        index = index - 10;
       
	//state = mb_mapping->tab_bits[index];
	
	uint8_t inBit[(index + 1)];
        modbus_read_input_bits(mb, 0, (index + 1), inBit);
        
        state = inBit[index];
        
        return state;
	
	//modbusExit();
}

int dmReadOutBit(int index)
{
	/*rc = modbus_receive(mb, query);
	if(rc > 0) 
	{
		modbus_reply(mb, query, rc, mb_mapping);
	} else if (rc == -1) {
		
	}*/
	
        index = index - 10;
	
        int state;
        uint8_t outbit_tab[100];
        modbus_read_bits(mb, index, index, outbit_tab);
        
        state = outbit_tab[index];
        
        return state;
}

int dmReadInReg(int index)
{
        index = index - 5;
        
        int value;
        uint16_t inreg_tab[100];
        modbus_read_input_registers(mb, index, index, inreg_tab);
        
        value = inreg_tab[index];
        
        return value;
}

void modbusExit(void)
{
        modbus_close(mb);
	modbus_free(mb);
}
