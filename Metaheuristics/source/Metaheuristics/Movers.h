#pragma once

#include "Solution.h"

class Mover {
public:
    virtual void MoveEx(Solution&) = 0;
    virtual void Reset() = 0;

    void Move(Solution&);
};
