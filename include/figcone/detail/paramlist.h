#pragma once
#include "iparam.h"
#include <figcone_tree/tree.h>
#include <figcone/errors.h>
#include <figcone_tree/stringconverter.h>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

namespace figcone::detail {

template<typename T>
class ParamList : public IParam{
public:
    ParamList(std::string name, std::vector<T>& paramValue)
        : name_{std::move(name)}
        , paramListValue_{paramValue}
    {
    }

    void markValueIsSet()
    {
        hasValue_ = true;
    }

private:
    void load(const TreeParam& paramList) override
    {
        position_ = paramList.position();
        hasValue_ = true;
        if (!paramList.isList())
            throw ConfigError{"Parameter list '" + name_ + "': config parameter must be a list.", paramList.position()};
        for (const auto& paramValueStr : paramList.valueList()) {
            auto paramValue = convertFromString<T>(paramValueStr);
            if (!paramValue)
                throw ConfigError{
                        "Couldn't set parameter list element'" + name_ + "' value from '" + paramValueStr + "'",
                        paramList.position()};
            paramListValue_.emplace_back(std::move(*paramValue));
        }
    }

    bool hasValue() const override
    {
        return hasValue_;
    }

    StreamPosition position() override
    {
        return position_;
    }

    std::string description() override
    {
        return "Parameter list '" + name_ + "'";
    }


private:
    std::string name_;
    std::vector<T>& paramListValue_;
    bool hasValue_ = false;
    StreamPosition position_;
};

}