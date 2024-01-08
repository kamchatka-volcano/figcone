#ifndef FIGCONE_IPARAM_H
#define FIGCONE_IPARAM_H

#include "iconfigentity.h"
#include <figcone_tree/tree.h>

namespace figcone::detail {

class IParam : public IConfigEntity {
public:
    virtual void load(const figcone::TreeParam& node) = 0;
    virtual bool hasValue() const = 0;
};

} //namespace figcone::detail

#endif //FIGCONE_IPARAM_H