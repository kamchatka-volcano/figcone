#pragma once
#include "nodecreator.h"
#include "nodelistcreator.h"
#include "paramcreator.h"
#include "paramlistcreator.h"
#include "dictcreator.h"

#define FIGCONE_PARAM(name, type) type name = figcone::detail::makeParamCreator<type>(cfgReader(), #name, [this]()->type&{return name;})
#define FIGCONE_NODE(name, type) type name = figcone::detail::makeNodeCreator<type>(cfgReader(), #name, [this]()->type&{return name;})
#define FIGCONE_COPY_NODELIST(name, listType) listType name = figcone::detail::makeNodeListCreator<listType>(cfgReader(), #name, [this]()->listType&{return name;}, figcone::detail::NodeListType::Copy)
#define FIGCONE_NODELIST(name, listType) listType name = figcone::detail::makeNodeListCreator<listType>(cfgReader(), #name, [this]()->listType&{return name;})
#define FIGCONE_PARAMLIST(name, listType) listType name = figcone::detail::makeParamListCreator<listType>(cfgReader(), #name, [this]()->listType&{return name;})
#define FIGCONE_DICT(name, mapType) mapType name = figcone::detail::makeDictCreator<mapType>(cfgReader(), #name, [this]()->mapType&{return name;})