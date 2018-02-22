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

    auto ret = GetPinAndState(index, command.functionCode, gpio, state);

    if(ret == CommandStatus::SUCCESS)
    {
        if (gpio < 1000) 
        {
            digitalWrite(gpio, state);
        } else {
            dmWriteBit(gpio, state);
        }
    }

    return ret;
}

CommandStatus GPIOCommandHandler::Select(const AnalogOutputInt16& command, uint16_t index)
{
    uint8_t gpio = 0;
    uint16_t value = 0;
    
    return GetPinAndValue(index, command.functionCode, gpio, value);
}

 CommandStatus GPIOCommandHandler::Operate(const AnalogOutputInt16& command, uint16_t index, OperateType opType)
{
    uint8_t gpio = 0;
    uint16_t value = 0;
    
    auto ret = GetPinAndValue(index, gpio, value);
    
    if(ret == CommandStatus::SUCCESS)
    {
        if(gpio < 40000)
        {
            analogWrite(gpio, value);
        } else {
            dmWriteRegs(gpio, value);
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
            break;
        case(ControlCode::LATCH_OFF):
            state = false;
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

CommandStatus GPIOCommandHandler::GetPinAndValue(uint16_t index, uint16_t& gpio, uint16_t& value)
{
    switch(code)
    {
        case(ControlCode::LATCH_ON):
            state = true;
            break;
        case(ControlCode::LATCH_OFF):
            state = false;
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
