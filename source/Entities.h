#ifndef Entities_H
#define Entities_H

#pragma once

#include "Base.h"
#include "Constants.h"
#include "Mesh.h"

#include <map>

enum class OblastCode {
    Zythomyr,
    Zaporizhia,
    Zakarpatia,
    Volyn,
    Vinnytsia,
    Ternopil,
    Sumy,
    Rivne,
    Poltava,
    Odessa,
    Mykolaiv,
    Lviv,
    Luhansk,
    Kirovohrad,
    Kiev,
    Chmielnicki,
    Kherson,
    Kharkiv,
    IvanoFrankivsk,
    Donetsk,
    Dnipropetrovsk,
    Crimea,
    Chernihiv,
    Chernivtsi,
    Cherkasy,
    /*  Capitol,
      Sevastopol,*/

};

const std::map<OblastCode, const char*> OBLAST_NAMES = {
    { OblastCode::Zythomyr, "Zythomyr" },
    { OblastCode::Zaporizhia, "Zaporizhia" },
    { OblastCode::Zakarpatia, "Zakarpatia" },
    { OblastCode::Volyn, "Volyn" },
    { OblastCode::Vinnytsia, "Vinnytsia" },
    { OblastCode::Ternopil, "Ternopil" },
    { OblastCode::Sumy, "Sumy" },
    { OblastCode::Rivne, "Rivne" },
    { OblastCode::Poltava, "Poltava" },
    { OblastCode::Odessa, "Odessa" },
    { OblastCode::Mykolaiv, "Mykolaiv" },
    { OblastCode::Lviv, "Lviv" },
    { OblastCode::Luhansk, "Luhansk" },
    { OblastCode::Kirovohrad, "Kirovohrad" },
    { OblastCode::Kiev, "Kiev" },
    { OblastCode::Chmielnicki, "Chmielnicki" },
    { OblastCode::Kherson, "Kherson" },
    { OblastCode::Kharkiv, "Kharkiv" },
    { OblastCode::IvanoFrankivsk, "IvanoFrankivsk" },
    { OblastCode::Donetsk, "Donetsk" },
    { OblastCode::Dnipropetrovsk, "Dnipropetrovsk" },
    { OblastCode::Crimea, "Crimea" },
    { OblastCode::Chernihiv, "Chernihiv" },
    { OblastCode::Chernivtsi, "Chernivtsi" },
    { OblastCode::Cherkasy, "Cherkasy" },
};

class Oblast {
    public:
    const char* name = "";
    MeshInBuffer mesh;
    OblastCode code = OblastCode::Kiev;
    f32 ukrainian_control = 0.0f;

    Oblast() { }
    Oblast(MeshInBuffer mesh, OblastCode code, const char* name, f32 ukrainian_control = 0.0f)
    {
        this->mesh = mesh;
        this->code = code;
        this->name = name;
        this->ukrainian_control = ukrainian_control;
    }
};

enum class CountryCode {
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

struct Country {
    CountryCode code = CountryCode::UK;
    const char* name = "";
    f32 support = 0.0f;

    Country() { }
    Country(CountryCode code, const char* name, f32 support)
    {
        this->code = code;
        this->name = name;
        this->support = support;
    }
};

#endif
