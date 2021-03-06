#ifndef TS_DNP3_GPIOCOMMANDHANDLER_H
#define TS_DNP3_GPIOCOMMANDHANDLER_H

#include <opendnp3/outstation/ICommandHandler.h>

#include <vector>
#include <map>

class GPIOCommandHandler final : public opendnp3::ICommandHandler
{

public:
    GPIOCommandHandler(const std::vector<uint16_t> gpiopins);
    
    void Start() override {}
    void End() override {}

    opendnp3::CommandStatus Select(const opendnp3::ControlRelayOutputBlock& command, uint16_t index) override;

    opendnp3::CommandStatus Operate(const opendnp3::ControlRelayOutputBlock& command, uint16_t index, opendnp3::OperateType opType) override;

    opendnp3::CommandStatus Select(const opendnp3::AnalogOutputInt16& command, uint16_t index) override
    { return opendnp3::CommandStatus::NOT_SUPPORTED; }

    opendnp3::CommandStatus Operate(const opendnp3::AnalogOutputInt16& command, uint16_t index, opendnp3::OperateType opType) override
    { return opendnp3::CommandStatus::NOT_SUPPORTED; }
    
    opendnp3::CommandStatus Select(const opendnp3::AnalogOutputInt32& command, uint16_t index) override
    { return opendnp3::CommandStatus::NOT_SUPPORTED; }

    opendnp3::CommandStatus Operate(const opendnp3::AnalogOutputInt32& command, uint16_t index, opendnp3::OperateType opType) override
    { return opendnp3::CommandStatus::NOT_SUPPORTED; }

    opendnp3::CommandStatus Select(const opendnp3::AnalogOutputFloat32& command, uint16_t index) override
    { return opendnp3::CommandStatus::NOT_SUPPORTED; }

    opendnp3::CommandStatus Operate(const opendnp3::AnalogOutputFloat32& command, uint16_t index, opendnp3::OperateType opType)
    { return opendnp3::CommandStatus::NOT_SUPPORTED; }

    opendnp3::CommandStatus Select(const opendnp3::AnalogOutputDouble64& command, uint16_t index) override
    { return opendnp3::CommandStatus::NOT_SUPPORTED; }

    opendnp3::CommandStatus Operate(const opendnp3::AnalogOutputDouble64& command, uint16_t index, opendnp3::OperateType opType)
    { return opendnp3::CommandStatus::NOT_SUPPORTED; }

private:

    opendnp3::CommandStatus GetPinAndState(uint16_t index, opendnp3::ControlCode code, uint16_t& gpio, bool& state);

    std::map<uint16_t, uint16_t> dnp2gpio;
    
    opendnp3::CommandStatus GetPinAndValue(uint16_t index, opendnp3::ControlCode code, uint16_t& analogpin, uint16_t& value);
    
    std::map<uint16_t, uint16_t> dnp2analogpin;
};

#endif //TS_DNP3_GPIOCOMMANDHANDLER_H
