#include "GPIOCommandHandler.h"
#include "sources/gpiolib.h"
#include "sources/modbusCommands.h"

using namespace opendnp3;

GPIOCommandHandler::GPIOCommandHandler(const std::vector<uint8_t> gpiopins)
{
    uint16_t index = 0;

    for(auto pin : gpiopins) {
       dnp2gpio[index] = pin;
       ++index;
    }
}

GPIOCommandHandler::GPIOCommandHandler(const std::vector<uint8_t> analogpins)
{
    uint16_t index = 0;
    
    for(auto pin : analogpins) {
        dnp2analogpin[index] = pin;
        ++index;
    }
}

CommandStatus GPIOCommandHandler::Select(const ControlRelayOutputBlock& command, uint16_t index)
{
    uint8_t gpio = 0;
    bool state = false;

    return GetPinAndState(index, command.functionCode, gpio, state);
}

CommandStatus GPIOCommandHandler::Operate(const ControlRelayOutputBlock& command, uint16_t index, OperateType opType)
{
    uint8_t gpio = 0;
    bool state = false;

    auto ret = GetPinAndState(index, command.functionCode, gpio, state);

    if(ret == CommandStatus::SUCCESS)
    {
        if (gpio < 1000) 
        {
            digitalWrite(gpio, state);
        } else {
           // dmWriteBit((gpio - 1000), state);
        }
    }

    return ret;
}

opendnp3::CommandStatus Select(const opendnp3::AnalogOutputInt16& command, uint16_t index)
{
    uint8_t analogpin = 0;
    uint16_t value = 0;
    
    return GetPinAndValue(index, command.functoinCode, analogpin, value);
}

opendnp3::CommandStatus Operate(const opendnp3::AnalogOutputInt16& command, uint16_t index, opendnp3::OperateType opType)
{
    uint8_t analogpin = 0;
    uint16_t value = 0;
    
    auto ret = GetPinAndValue(index, command.functionCode, analogpin, value);
    
    if(ret == CommandStatus::SUCCESS)
    {
        if(gpio < 10)
        {
            analogWrite(analogpin, value);
        } else {
            dmWriteRegs((analogpin  - 10), value);
        }
    }
    
    return ret;
}

CommandStatus GPIOCommandHandler::GetPinAndState(uint16_t index, opendnp3::ControlCode code, uint8_t& gpio, bool& state)
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

CommandStatus GPIOCommandHandler::GetPinAndValue(uint16_t index, opendnp3::ControlCode code, uint8_t& analogpin, uint16_t& value)
{
    auto iter = dnp2analogpin.find(index);
    if(iter == dnp2analogpin.end()) {
        return CommandStatus::NOT_SUPPORTED;
    }
    
    analogpin = iter->second;
    return CommandStatus::SUCCESS;
}
