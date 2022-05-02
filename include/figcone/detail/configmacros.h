#pragma once
#include "nodecreator.h"
#include "nodelistcreator.h"
#include "paramcreator.h"
#include "paramlistcreator.h"
#include "dictcreator.h"
#include <vector>
#include <string>
#include <map>

#define FIGCONE_PARAM(name, type) type name = figcone::detail::makeParamCreator<type>(*this, #name, [this]()->type&{return name;})
#define FIGCONE_NODE(name, type) type name = figcone::detail::makeNodeCreator<type>(*this, #name, [this]()->type&{return name;})
#define FIGCONE_COPY_NODELIST(name, type) type name = figcone::detail::makeNodeListCreator<type>(*this, #name, [this]()->type&{return name;}, figcone::detail::NodeListType::Copy)
#define FIGCONE_NODELIST(name, type) type name = figcone::detail::makeNodeListCreator<type>(*this, #name, [this]()->type&{return name;})
#define FIGCONE_PARAMLIST(name, type) type name = figcone::detail::makeParamListCreator<type>(*this, #name, [this]()->type&{return name;})
#define FIGCONE_DICT(name) std::map<std::string, std::string> name = figcone::detail::makeDictCreator(*this, #name, [this]()->std::map<std::string, std::string>&{return name;})