#ifndef TS_DNP3_CONFIG_H
#define TS_DNP3_CONFIG_H

#include <cstdint>
#include <vector>
#include <set>

#include <opendnp3/link/LinkConfig.h>

class Config
{
    typedef std::vector<uint8_t> assignvec0;
    typedef std::vector<uint8_t> assignvec1;
   
public:

    Config() : link(false, false), sample_period_ms(100), deadband(50)
    {}

    bool AddInput(uint8_t gpiopin);
    bool AddAninput(uint8_t gpiopin);
    bool AddOutput(uint8_t gpiopin);
    bool AddAnoutput(uint8_t gpiopin);

    std::vector<uint16_t> inputs;
    std::vector<uint16_t> aninputs;
    std::vector<uint16_t> outputs;
    std::vector<uint16_t> anoutputs;

    opendnp3::LinkConfig link;
    int sample_period_ms;
    int deadband;
    int port;

private:

    bool Add(uint8_t gpiopin, assignvec0& vec0);
    //bool Add1(uint8_t analogpin, assignvec1& vec1);

    std::set<uint8_t> assigned;
};

#endif //TS_DNP3_CONFIG_H
