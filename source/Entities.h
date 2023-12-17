#pragma once

#include "Base.h"

enum OblastCode {
    Cherkasy,
    Chernihiv,
    Chernivtsi,
    Dnipropetrovsk,
    Donetsk,
    IvanoFrankivsk,
    Kharkiv,
    Kherson,
    Khmelnytskyi,
    Kiev,
    Kirovohrad,
    Luhansk,
    Lviv,
    Mykolaiv,
    Odessa,
    Poltava,
    Rivne,
    Sumy,
    Ternopil,
    Vinnytsia,
    Volyn,
    Zakarpattia,
    Zaporizhia,
    Zhytomyr
};

struct Oblast : public Entity {
    Oblast(OblastCode code, char* name, float ukrainian_control)
    {
        this->code = code;
        this->name = name;
        this->ukrainian_control = ukrainian_control;
    }

    OblastCode code;
    char* name;
    float ukrainian_control;

    // Inherited via Entity
    bool Update() override;
};

enum CountryCode {
    USA,
    DE,
    UK,
    PL,
    FR,
    SP,
    SE,
    CZ,
    HU,
    FI,
};

struct Country : public Entity {
    Country(CountryCode code, char* name, float support)
    {
        this->code = code;
        this->name = name;
        this->support = support;
    }

    CountryCode code;
    char* name;
    float support;

    // Inherited via Entity
    bool Update() override;
};
