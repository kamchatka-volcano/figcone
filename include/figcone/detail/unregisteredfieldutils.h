#pragma once
#include "external/sfun/type_traits.h"
#include <figcone/unregisteredfieldhandler.h>

namespace figcone::detail {
template<typename TCfg>
void handleUnregisteredField(FieldType fieldType, const std::string& fieldName, const StreamPosition& position)
{
    if constexpr (sfun::is_complete_type_v<UnregisteredFieldHandler<TCfg>>)
        UnregisteredFieldHandler<TCfg>{}(fieldType, fieldName, position);
    else {
        if (fieldType == FieldType::Node)
            throw ConfigError{"Unknown node '" + fieldName + "'", position};
        else
            throw ConfigError{"Unknown param '" + fieldName + "'", position};
    }
}

} //namespace figcone::detail