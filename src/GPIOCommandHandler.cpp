#include "GPIOCommandHandler.h"
#include "sources/gpiolib.h"
#include "sources/modbusCommands.h"

using namespace opendnp3;

GPIOCommandHandler::GPIOCommandHandler(const std::vector<uint16_t> gpiopins)
{
    uint16_t index = 0;

    for(auto pin : gpiopins) {
       dnp2gpio[index] = pin;
       ++index;
    }
}

CommandStatus GPIOCommandHandler::Select(const ControlRelayOutputBlock& command, uint16_t index)
{
    uint16_t gpio = 0;
    bool state = false;

    return GetPinAndState(index, command.functionCode, gpio, state);
}

CommandStatus GPIOCommandHandler::Operate(const ControlRelayOutputBlock& command, uint16_t index, OperateType opType)
{
    uint16_t gpio = 0;
    bool state = false;

    digitalWrite(58, 0);
    
    auto ret = GetPinAndState(index, command.functionCode, gpio, state);

    digitalWrite(58, 0);
    
    if(ret == CommandStatus::SUCCESS)
    {
        if (gpio < 1000) 
        {
            digitalWrite(gpio, state);
            //digitalWrite(58, 0);
        } else {
            dmWriteBit(gpio, state);
            //digitalWrite(58, 0);
        }
    }
    
    return ret;
}

CommandStatus GPIOCommandHandler::GetPinAndState(uint16_t index, opendnp3::ControlCode code, uint16_t& gpio, bool& state)
{
    switch(code)
    {
        case(ControlCode::LATCH_ON):
            state = true;
            digitalWrite(58, 1);
            break;
        case(ControlCode::LATCH_OFF):
            state = false;
            digitalWrite(58, 1);
            break;
        default:
            return CommandStatus::NOT_SUPPORTED;
    }

    auto iter = dnp2gpio.find(index);
    if(iter == dnp2gpio.end()) {
        return CommandStatus::NOT_SUPPORTED;
    }

    gpio = iter->second;
    return CommandStatus::SUCCESS;
}
