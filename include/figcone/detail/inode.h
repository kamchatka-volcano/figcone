#pragma once
#include "iconfigentity.h"
#include <figcone_tree/tree.h>
#include <memory>
#include <string>

namespace figcone::detail {

class IConfigNode : public IConfigEntity {
public:
    virtual void load(const figcone::TreeNode& node) = 0;

    virtual bool hasValue() const = 0;
};

}