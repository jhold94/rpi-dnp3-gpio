
#include <iostream>
#include <ini.h>
#include <stdexcept>
#include <modbus/modbus.h>
#include <modbus/modbus-tcp.h>

#include <asiodnp3/DNP3Manager.h>
#include <asiodnp3/UpdateBuilder.h>
#include <asiodnp3/PrintingChannelListener.h>
#include <asiopal/UTCTimeSource.h>
#include <asiodnp3/ConsoleLogger.h>
#include <opendnp3/LogLevels.h>

#include <thread>

#include "Config.h"
#include "GPIOCommandHandler.h"
#include "sources/gpiolib.h"
#include "sources/modbusCommands.h"

// prototypes for handling ini file reading
int cfg_handler(void* user, const char* section, const char* name, const char* value);
bool safe_handler(Config& config, const std::string& section, const std::string& name, const std::string& value);

using namespace opendnp3;
using namespace asiopal;
using namespace asiodnp3;

int main(int argc, char *argv[])
{
	if(argc != 2)
	{
		std::cerr << "usage: rpi-dnp3-gpio <ini file path>" << std::endl;
		return -1;
	}

	auto filename = argv[1];

	Config config;
	if(ini_parse(filename, cfg_handler, &config) != 0)
	{
		std::cerr << "error parsing ini file: " << filename << std::endl;
		return -1;
	}
	
	// setup inputs and outputs
	for(int pin : config.inputs) {
		if (pin > 205 && pin < 210)
		{
			pinMode(pin, 1);
		} else if ((pin == 0) || (pin > 0 && pin < 100)) {
			continue;
		} else {
			pinMode(pin, 0);
		}
		std::cout << "pin " << static_cast<int>(pin) << " set as INPUT" << std::endl;
	}
	
	analog_init();
	
	for(int pin : config.aninputs) {
		if (pin < 5) 
		{
			analogPinMode(pin);
		} else {
			continue;
		}
		std::cout << "pin " << static_cast<int>(pin) << " set as ANALOG INPUT" << std::endl;
	}

	for(int pin : config.outputs) {
		if (pin < 1000) 
		{
			pinMode(pin, 1);
		} else {
			continue;
		}
		std::cout << "pin " << static_cast<int>(pin) << " set as OUTPUT" << std::endl;
	}
	
	/*for(int pin : config.anoutputs) {
		
		std::cout << "pin " << static_cast<int>(pin) << " set as ANALOG OUTPUT" << std::endl;
	}*/

	const auto commandHandler = std::make_shared<GPIOCommandHandler>(config.outputs);
	//const auto commandHandler = std::make_shared<GPIOCommandHandler>(config.anoutputs);

	const auto LOG_LEVELS = levels::NORMAL | levels::ALL_APP_COMMS;

	DNP3Manager manager(1, ConsoleLogger::Create());

	auto channel = manager.AddTCPServer("server", LOG_LEVELS, ChannelRetry::Default(), "0.0.0.0", config.port, PrintingChannelListener::Create());

	/* Modbus Link stuff */
	modbus_init();
	
	OutstationStackConfig stack(
		DatabaseSizes(
			config.inputs.size(), // binary
			0, 
			config.aninputs.size(), //analog
			0, 0,
			config.outputs.size(), // binary output status
			0, //config.anoutputs.size(), //analog output status
			0
		) /* cpp/libs/include/opendnp3/outstation/DatabaseSizes.h */
	);
	stack.link = config.link;

	stack.outstation.eventBufferConfig = EventBufferConfig(50,0,150,0,0,50,0,0);
	stack.outstation.params.allowUnsolicited = true;
	stack.dbConfig.analog[0].deadband = config.deadband;
	stack.dbConfig.analog[1].deadband = config.deadband;

	auto outstation = channel->AddOutstation("outstation", commandHandler, DefaultOutstationApplication::Create(), stack);

	outstation->Enable();

	std::cout << "Sample period is: " << config.sample_period_ms << std::endl;
        const auto SAMPLE_PERIOD = std::chrono::milliseconds(config.sample_period_ms);

	while(true) {

		DNPTime time(asiopal::UTCTimeSource::Instance().Now().msSinceEpoch);

		UpdateBuilder builder;

		uint16_t index = 0;
		for(int pin : config.inputs) {
			bool value;
			if (pin > 100)
			{
				value = digitalRead(pin);
			} else {
				value = dmReadBit(pin);
			}
			builder.Update(Binary(value, 0x01, time), index);
			++index;
		}
		
		index = 0;
		for(int pin : config.aninputs) {
			int anValue;
			if (pin < 5) 
			{
				anValue = analogRead(pin);
			} else {
				anValue = dmReadInReg(pin);
			}
			builder.Update(Analog(anValue, 0x01, time), index);
			++index;
		}

		index = 0;
		for(int pin : config.outputs) {
			bool outValue;
			if (pin < 1000)
			{
				outValue = digitalRead(pin);
			} else {
				outValue = dmReadOutBit(pin);
			}
			builder.Update(BinaryOutputStatus(outValue, 0x01, time), index);
			++index;
		}

		outstation->Apply(builder.Build());

		
		// determines the sampling rate
		std::this_thread::sleep_for(SAMPLE_PERIOD);
	}
	
	/* More modbus stuff */
	modbusExit();
}

bool safe_handler(Config& config, const std::string& section, const std::string& name, const std::string& value)
{
	try
	{		
		if(section == "input")
		{
			return config.AddInput(std::stoi(name));
		}
		else if(section == "aninput")
		{
			return config.AddAninput(std::stoi(name));
		}
		else if(section == "output")
		{
			return config.AddOutput(std::stoi(name));
		}
		else if(section == "program")
		{
			if(name == "sample-period-ms")
			{
				config.sample_period_ms = std::stoi(value);
				return true;
			}
			else if(name == "deadband")
			{
				config.deadband = std::stoi(value);
				return true;
			}
		}
		else if(section == "link")
		{
			if(name == "remote-addr")
			{
				config.link.RemoteAddr = std::stoi(value);
				return true;
			}
			else if(name == "local-addr")
			{
				config.link.LocalAddr = std::stoi(value);
				return true;
			}
			else if (name == "port")
			{
				config.port = std::stoi(value);
				return true;
			}
		}

		std::cerr << "Unknown parameter, section: " << section << " name: " << name << " value: " << value << std::endl;
		return false;
	}
	catch(std::invalid_argument)
	{
		std::cerr << "Bad integer conversion, section: " << section << " name: " << name << " value: " << value << std::endl;
		return false;
	}
}

int cfg_handler(void* user, const char* section, const char* name, const char* value)
{
	return safe_handler(*(Config*)user, section, name, value) ? 1 : 0;
}
