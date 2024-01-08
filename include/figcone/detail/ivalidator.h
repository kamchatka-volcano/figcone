#ifndef FIGCONE_IVALIDATOR_H
#define FIGCONE_IVALIDATOR_H

#include "external/sfun/interface.h"

namespace figcone::detail {

class IValidator : private sfun::interface<IValidator> {
public:
    virtual void validate() = 0;
};

} //namespace figcone::detail

#endif //FIGCONE_IVALIDATOR_H