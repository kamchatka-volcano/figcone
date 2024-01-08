#ifndef FIGCONE_UNREGISTEREDFIELDHANDLER_H
#define FIGCONE_UNREGISTEREDFIELDHANDLER_H

#include "errors.h"
#include <figcone_tree/streamposition.h>
#include <string>

namespace figcone {

enum class FieldType {
    Node,
    Param
};

template<typename TCfg>
struct UnregisteredFieldHandler;

} //namespace figcone

#endif //FIGCONE_UNREGISTEREDFIELDHANDLER_H