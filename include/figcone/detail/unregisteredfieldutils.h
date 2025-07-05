#ifndef FIGCONE_UNREGISTEREDFIELDUTILS_H
#define FIGCONE_UNREGISTEREDFIELDUTILS_H

#include "external/eel/type_traits.h"
#include <figcone/unregisteredfieldhandler.h>

namespace figcone::detail {
template<typename TCfg>
void handleUnregisteredField(FieldType fieldType, const std::string& fieldName, const StreamPosition& position)
{
    if constexpr (eel::is_complete_type_v<UnregisteredFieldHandler<TCfg>>)
        UnregisteredFieldHandler<TCfg>{}(fieldType, fieldName, position);
    else {
        if (fieldType == FieldType::Node)
            throw ConfigError{"Unknown node '" + fieldName + "'", position};
        else
            throw ConfigError{"Unknown param '" + fieldName + "'", position};
    }
}

} //namespace figcone::detail

#endif //FIGCONE_UNREGISTEREDFIELDUTILS_H