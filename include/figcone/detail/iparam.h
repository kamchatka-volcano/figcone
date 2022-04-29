#pragma once
#include "iconfigentity.h"
#include <figcone_tree/tree.h>

namespace figcone::detail{

class IParam : public IConfigEntity{
public:
    virtual void load(const figcone::TreeParam& node) = 0;
    virtual bool hasValue() const = 0;
};

}