#pragma once
#include "external/sfun/interface.h"

namespace figcone::detail {

class IValidator : private sfun::interface<IValidator> {
public:
    virtual void validate() = 0;
};

} //namespace figcone::detail