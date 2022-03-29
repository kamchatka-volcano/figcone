#pragma once
#include <figcone_tree/streamposition.h>
#include <functional>
#include <string>

namespace figcone::detail {

class IConfigEntity{
public:
    virtual ~IConfigEntity() = default;
    virtual StreamPosition position() = 0;
    virtual std::string description() = 0;
};

}