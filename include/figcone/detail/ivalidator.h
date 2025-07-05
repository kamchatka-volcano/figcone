#ifndef FIGCONE_IVALIDATOR_H
#define FIGCONE_IVALIDATOR_H

#include "external/eel/interface.h"

namespace figcone::detail {

class IValidator : private eel::interface<IValidator> {
public:
    virtual void validate() = 0;
};

} //namespace figcone::detail

#endif //FIGCONE_IVALIDATOR_H