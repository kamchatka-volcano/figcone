#pragma once
#include "nodecreator.h"
#include "nodelistcreator.h"
#include "paramcreator.h"
#include "paramlistcreator.h"
#include "dictcreator.h"

#define FIGCONE_PARAM(name, type) type name = param<&std::remove_pointer_t<decltype(this)>::name>(#name)
#define FIGCONE_NODE(name, type) type name = node<&std::remove_pointer_t<decltype(this)>::name>(#name)
#define FIGCONE_COPY_NODELIST(name, listType) listType name = copyNodeList<&std::remove_pointer_t<decltype(this)>::name>(#name)
#define FIGCONE_NODELIST(name, listType) listType name = nodeList<&std::remove_pointer_t<decltype(this)>::name>(#name)
#define FIGCONE_PARAMLIST(name, listType) listType name = paramList<&std::remove_pointer_t<decltype(this)>::name>(#name)
#define FIGCONE_DICT(name, mapType) mapType name = dict<&std::remove_pointer_t<decltype(this)>::name>(#name)