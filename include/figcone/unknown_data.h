#pragma once

#include <functional>
#include "figcone_tree/streamposition.h"

namespace figcone {

typedef std::function<void(std::string node,
                           std::string param,
                           StreamPosition position)> UnknownDataHandler;

static inline UnknownDataHandler defaultUnknownDataHandler()
{
    return [](std::string nodeName, std::string paramName, StreamPosition position) {
        if(!nodeName.empty())
            throw ConfigError{"Unknown node '" + nodeName + "'", position};
        throw ConfigError{"Unknown param '" + paramName + "'", position};
    };
}    
                            
} //namespace figcone
