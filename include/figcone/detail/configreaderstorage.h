#pragma once
#include <figcone_tree/tree.h>
#include "configreaderptr.h"
#include <memory>
#include <string>

namespace figcone::detail{

class ConfigReaderStorage{
    friend class IConfigReader;

public:
    ConfigReaderStorage() = default;
    ConfigReaderStorage(detail::ConfigReaderPtr reader)
        : cfgReader_{reader}
    {}

protected:
    detail::ConfigReaderPtr cfgReader() const
    {
        return cfgReader_;
    }
private:
    detail::ConfigReaderPtr cfgReader_;
};

}