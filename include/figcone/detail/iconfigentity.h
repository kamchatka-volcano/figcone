#ifndef FIGCONE_ICONFIGIDENTITY_H
#define FIGCONE_ICONFIGIDENTITY_H

#include "external/sfun/interface.h"
#include <figcone_tree/streamposition.h>
#include <functional>
#include <string>

namespace figcone::detail {

class IConfigEntity : private sfun::interface<IConfigEntity> {
public:
    virtual StreamPosition position() = 0;
    virtual std::string description() = 0;
};

} //namespace figcone::detail

#endif //FIGCONE_ICONFIGIDENTITY_H