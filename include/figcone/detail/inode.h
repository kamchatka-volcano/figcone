#ifndef FIGCONE_INODE_H
#define FIGCONE_INODE_H

#include "iconfigentity.h"
#include <figcone_tree/tree.h>
#include <memory>
#include <string>

namespace figcone::detail {

class INode : public IConfigEntity {
public:
    virtual void load(const figcone::TreeNode& node) = 0;
    virtual bool hasValue() const = 0;
};

} //namespace figcone::detail

#endif //FIGCONE_INODE_H