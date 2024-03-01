#ifndef Entities_H
#define Entities_H

#pragma once

#include "Base.h"
#include "Constants.h"
#include "Mesh.h"

#include <map>

//25 or 27
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

const std::map<OblastCode, f32> INITIAL_CONTROL = {
    { OblastCode::Zythomyr, 1.0f },
    { OblastCode::Zaporizhia, 1.0f },
    { OblastCode::Zakarpatia, 1.0f },
    { OblastCode::Volyn, 1.0f },
    { OblastCode::Vinnytsia, 1.0f },
    { OblastCode::Ternopil, 1.0f },
    { OblastCode::Sumy, 1.0f },
    { OblastCode::Rivne, 1.0f },
    { OblastCode::Poltava, 1.0f },
    { OblastCode::Odessa, 1.0f },
    { OblastCode::Mykolaiv, 1.0f },
    { OblastCode::Lviv, 1.0f },
    { OblastCode::Luhansk, 0.2f },
    { OblastCode::Kirovohrad, 1.0f },
    { OblastCode::Kiev, 1.0f },
    { OblastCode::Chmielnicki, 1.0f },
    { OblastCode::Kherson, 1.0f },
    { OblastCode::Kharkiv, 1.0f },
    { OblastCode::IvanoFrankivsk, 1.0f },
    { OblastCode::Donetsk, 0.3f },
    { OblastCode::Dnipropetrovsk, 1.0f },
    { OblastCode::Crimea, 0.0f },
    { OblastCode::Chernihiv, 1.0f },
    { OblastCode::Chernivtsi, 1.0f },
    { OblastCode::Cherkasy, 1.0f },
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

    void UpdateColorBasedOnControl();

    Oblast(MeshInBuffer mesh, OblastCode code, const char* name, f32 ukrainian_control = 0.0f)
    {
        this->mesh = mesh;
        this->code = code;
        this->name = name;
        this->ukrainian_control = ukrainian_control;

        UpdateColorBasedOnControl();
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
    CountryCode code;
    const char* name;
    f32 support;
	u8 index_in_atlas;

    Country() { }
    Country(CountryCode code, const char* name, f32 support, u8 index_in_atlas)
    {
        this->code = code;
        this->name = name;
        this->support = support;
		this->index_in_atlas = index_in_atlas;
    }
};

#endif
