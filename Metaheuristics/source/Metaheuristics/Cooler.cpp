#include "Cooler.h"

void Cooler::Reset() {
    this->temperature = this->initial_temperature;
}

void QuadriaticCooler::Cool()
{
    this->temperature *= this->multp;
}
