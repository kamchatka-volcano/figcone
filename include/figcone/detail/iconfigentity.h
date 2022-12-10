#pragma once
#include <figcone_tree/streamposition.h>
#include "external/sfun/interface.h"
#include <functional>
#include <string>

namespace figcone::detail {

class IConfigEntity : private sfun::Interface<IConfigEntity>{
public:
    virtual StreamPosition position() = 0;
    virtual std::string description() = 0;
};

}