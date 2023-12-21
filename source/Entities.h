#ifndef Entities_H
#define Entities_H

#pragma once

#include "Base.h"
#include "Model.h"
#include "Mesh.h"

enum OblastCode {
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
    Capitol,
    Sevastopol,

};

struct Oblast : public Entity {
    Oblast(Mesh* mesh, OblastCode code, const char* name, float ukrainian_control)
    {
        this->mesh = mesh;
        this->code = code;
        this->name = name;
        this->ukrainian_control = ukrainian_control;
    }

    Mesh* mesh;
    OblastCode code;
    const char* name;
    float ukrainian_control;

    // Inherited via Entity
    bool Update() override;
};

//void SetOblastPosition(OblastCode code, Model* ukraine_map, v4 pos)
//{
//    int i = 0;
//    Mesh* oblast = &ukraine_map->meshes.at(code);
//    for (auto& v : oblast->vertices) {
//        v.Color = { 1.0, 0.2, 0.2, 1.0 };
//    }
//    oblast->UpdateBuffer();
//}

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

#endif
