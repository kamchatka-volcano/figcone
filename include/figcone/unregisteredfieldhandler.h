#pragma once
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
