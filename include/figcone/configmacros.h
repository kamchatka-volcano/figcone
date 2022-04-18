#pragma once
#include "detail/nodecreator.h"
#include "detail/nodelistcreator.h"
#include "detail/paramcreator.h"
#include "detail/paramlistcreator.h"
#include "detail/dictcreator.h"

#define FIGCONE_PARAM(name, type) type name = figcone::detail::makeParamCreator<type>(*this, #name, [this]()->type&{return name;})
#define FIGCONE_NODE(name, type) type name = figcone::detail::makeNodeCreator<type>(*this, #name, [this]()->type&{return name;})
#define FIGCONE_COPY_NODELIST(name, type) std::vector<type> name = figcone::detail::makeNodeListCreator<type>(*this, #name, [this]()->std::vector<type>&{return name;}, figcone::detail::NodeListType::Copy)
#define FIGCONE_NODELIST(name, type) std::vector<type> name = figcone::detail::makeNodeListCreator<type>(*this, #name, [this]()->std::vector<type>&{return name;})
#define FIGCONE_PARAMLIST(name, type) std::vector<type> name = figcone::detail::makeParamListCreator<type>(*this, #name, [this]()->std::vector<type>&{return name;})
#define FIGCONE_DICT(name) std::map<std::string, std::string> name = figcone::detail::makeDictCreator(*this, #name, [this]()->std::map<std::string, std::string>&{return name;})