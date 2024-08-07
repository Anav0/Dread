#ifndef Entities_H
#define Entities_H

#pragma once

#include "Engine/GLM.h"
#include "Misc/Constants.h"

#include "Engine/Base.h"
#include "Engine/Mesh.h"
#include "Engine/bimap.h"

#include <map>

// 25 or 27
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

static const BiMap<OblastCode, const std::string> OBLASTS = {
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

enum class FactorToShow {
    Air,
    Combat,
    Damage,
    Infrastructure,
    GroundPower,
};

class Oblast {
public:
    const char* name = "";
    v3 position;
    MeshInBuffer mesh;
    OblastCode code = OblastCode::Kiev;
    f32 ukrainian_control = 0.0f;

    Oblast() { }

    void UpdateColorBasedOnControl();
    void UpdateColorBasedOnFactor(FactorToShow factor);

    u32 GetUkrainianFactor(FactorToShow factor);
    u32 GetRussianFactor(FactorToShow factor);

    Oblast(MeshInBuffer mesh, OblastCode code, v3 position, const char* name, f32 ukrainian_control = 0.0f)
    {
        this->position = position;
        this->mesh = mesh;
        this->code = code;
        this->name = name;
        this->ukrainian_control = ukrainian_control;

        UpdateColorBasedOnControl();
    }

    Oblast(OblastCode code, v3 position, const char* name, f32 ukrainian_control = 0.0f)
    {
        this->position = position;
        this->code = code;
        this->name = name;
        this->ukrainian_control = ukrainian_control;
    }

    void SetMesh(MeshInBuffer mesh)
    {
        this->mesh = mesh;
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
