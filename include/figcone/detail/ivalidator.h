#pragma once
#include "external/sfun/interface.h"

namespace figcone::detail {

class IValidator : private sfun::Interface<IValidator>{
public:
    virtual void validate() = 0;
};

}