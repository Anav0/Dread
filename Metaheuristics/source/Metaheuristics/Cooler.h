#pragma once;

#include "Misc/Constants.h"

class Cooler {
protected:
    f32 initial_temperature;

public:
    f32 temperature;

    Cooler(f32 temperature)
        : temperature(temperature)
    {
        initial_temperature = temperature;
    }

    void Reset();

    virtual void Cool() = 0;
};

class QuadriaticCooler : public Cooler {
private:
    f32 multp;

public:
    QuadriaticCooler(f32 temperature, f32 multp)
        : Cooler(temperature)
        , multp(multp)
    {
    }

    // Inherited via Cooler
    void Cool() override;
};