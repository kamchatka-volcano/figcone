#pragma once
#include "external/sfun/interface.h"
#include <figcone_tree/streamposition.h>
#include <functional>
#include <string>

namespace figcone::detail {

class IConfigEntity : private sfun::Interface<IConfigEntity> {
public:
    virtual StreamPosition position() = 0;
    virtual std::string description() = 0;
};

} //namespace figcone::detail