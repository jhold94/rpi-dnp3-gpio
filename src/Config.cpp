
#include "Config.h"

bool Config::AddInput(uint8_t gpiopin)
{
    return Add(gpiopin, inputs);
}

bool Config::AddAninput(uint8_t gpiopin)
{
    return Add(gpiopin, aninputs);
}

bool Config::AddOutput(uint8_t gpiopin)
{
    return Add(gpiopin, outputs);
}

bool Config::AddAnoutput(uint8_t analogpin)
{
    return Add(analogpin, anoutputs);
}

bool Config::Add(uint8_t gpiopin, assignvec& vec)
{
    // first check if the pin is assigned
    if(assigned.find(gpiopin) != assigned.end())
    {
        return false;
    }

    vec.push_back(gpiopin);
    assigned.insert(gpiopin);

    return true;
}

bool Config::Add(uint8_t analogpin, assignvec& vec)
{
    // first check if the pin is assigned
    if(assigned.find(analogpin) != assigned.end())
    {
        return false;
    }
    
    vec.pushback(analogpin);
    assigned.insert(analogpin);
    
    return true;
}


