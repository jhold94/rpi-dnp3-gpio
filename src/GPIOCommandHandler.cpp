#include "GPIOCommandHandler.h"
#include "sources/gpiolib.h"
#include "sources/modbusCommands.h"

using namespace opendnp3;

GPIOCommandHandler::GPIOCommandHandler(const std::vector<uint16_t> gpiopins, const std::vector<uint16_t> aniopins)
{
    uint16_t index = 0;

    for(auto pin : gpiopins) {
       dnp2gpio[index] = pin;
       ++index;
    }
    
    index = 0;
    
    for( auto pin : aniopins) {
        dnp2anio[index] = pin;
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
    uint16_t anio = 0;
    //uint16_t value = 0;
    
    return GetPin(index, anio);
}

 CommandStatus GPIOCommandHandler::Operate(const AnalogOutputInt16& command, uint16_t index, OperateType opType)
{
    uint16_t anio = 0;
    uint16_t value = 0;
    
    auto ret = GetPin(index, anio);
    
    value = command.value;
     
    if(ret == CommandStatus::SUCCESS)
    {
        if(anio < 40000)
        {
            analogWrite(anio, value);
        } else {
            dmWriteReg(anio, value);
        }
    }
    
    return ret;
}

CommandStatus GPIOCommandHandler::Select(const AnalogOutputInt32& command, uint16_t index)
{
    uint16_t anio = 0;
    //uint16_t value = 0;
    
    return GetPin(index, anio);
}

 CommandStatus GPIOCommandHandler::Operate(const AnalogOutputInt32& command, uint16_t index, OperateType opType)
{
    uint16_t anio = 0;
    uint32_t value = 0;
    
    auto ret = GetPin(index, anio);
    
    value = command.value;
     
    if(ret == CommandStatus::SUCCESS)
    {
        if(anio < 40000)
        {
            analogWrite(anio, value);
        } else {
            dmWriteReg(anio, value);
        }
    }
    
    return ret;
}

CommandStatus GPIOCommandHandler::Select(const AnalogOutputFloat32& command, uint16_t index)
{
    uint16_t anio = 0;
    //uint16_t value = 0;
    
    return GetPin(index, anio);
}

 CommandStatus GPIOCommandHandler::Operate(const AnalogOutputFloat32& command, uint16_t index, OperateType opType)
{
    uint16_t anio = 0;
    float value = 0;
    
    auto ret = GetPin(index, anio);
    
    value = command.value;
     
    if(ret == CommandStatus::SUCCESS)
    {
        if(anio < 40000)
        {
            analogWrite(anio, value);
        } else {
            dmWriteReg(anio, value);
        }
    }
    
    return ret;
}

CommandStatus GPIOCommandHandler::Select(const AnalogOutputDouble64& command, uint16_t index)
{
    uint16_t anio = 0;
    //uint16_t value = 0;
    
    return GetPin(index, anio);
}

 CommandStatus GPIOCommandHandler::Operate(const AnalogOutputDouble64& command, uint16_t index, OperateType opType)
{
    uint16_t anio = 0;
    double value = 0;
    
    auto ret = GetPin(index, anio);
    
    value = command.value;
     
    if(ret == CommandStatus::SUCCESS)
    {
        if(anio < 40000)
        {
            analogWrite(anio, value);
        } else {
            dmWriteReg(anio, value);
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

CommandStatus GPIOCommandHandler::GetPin(uint16_t index, uint16_t& anio)
{
    
    auto iter = dnp2anio.find(index);
    if(iter == dnp2anio.end()) {
        return CommandStatus::NOT_SUPPORTED;
    }

    anio = iter->second;
    return CommandStatus::SUCCESS;
}
