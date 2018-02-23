
#include "Config.h"

bool Config::AddInput(uint16_t gpiopin)
{
    return Add(gpiopin, inputs);
}

bool Config::AddAninput(uint16_t gpiopin)
{
    return Add(gpiopin, aninputs);
}

bool Config::AddOutput(uint16_t gpiopin)
{
    return Add(gpiopin, outputs);
}

bool Config::AddAnoutput(uint16_t gpiopin)
{
    return Add(aniopin, anoutputs);
}

bool Config::Add(uint16_t gpiopin, assignvec0& vec0)
{
    // first check if the pin is assigned
    if(assigned.find(gpiopin) != assigned.end())
    {
        return false;
    }

    vec0.push_back(gpiopin);
    assigned.insert(gpiopin);

    return true;
}

bool Config::Add(uint16_t aniopin, assigncev1& vec1)
{
    //first check if the pin is assigned
    if(assigned.find(aniopin) != assigned.end())
    {
        return false;
    }
    
    vec1.pushback(aniopin);
    assigned.insert(aniopin);
    
    return true;
}

