#ifndef TS_DNP3_CONFIG_H
#define TS_DNP3_CONFIG_H

#include <cstdint>
#include <vector>
#include <set>

#include <opendnp3/link/LinkConfig.h>

class Config
{
    typedef std::vector<uint8_t> assignvec;
   
public:

    Config() : link(false, false), sample_period_ms(100)
    {}

    bool AddInput(uint8_t gpiopin);
    bool AddAninput(uint8_t gpiopin);
    bool AddOutput(uint8_t gpiopin);
    bool AddAnoutput(uint8_t gpiopin);

    std::vector<uint8_t> inputs;
    std::vector<uint8_t> aninputs;
    std::vector<uint8_t> outputs;
    std::vector<uint8_t> anoutputs;

    opendnp3::LinkConfig link;
    int sample_period_ms;
    int port;

private:

    bool Add(uint8_t gpiopin, assignvec& vec);

    std::set<uint8_t> assigned;
};

#endif //TS_DNP3_CONFIG_H
