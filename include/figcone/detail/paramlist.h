#pragma once
#include "iparam.h"
#include <figcone_tree/tree.h>
#include <figcone/errors.h>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

namespace figcone::detail {

template<typename T>
class ConfigParamList : public IConfigParam{
public:
    ConfigParamList(std::string name, std::vector<T>& paramValue)
        : name_{std::move(name)}
        , paramListValue_{paramValue}
    {
    }

    void markValueIsSet()
    {
        hasValue_ = true;
    }

private:
    void load(const figcone::TreeParam& paramList) override
    {
        position_ = paramList.position();
        hasValue_ = true;
        for (const auto& paramValueStr : paramList.valueList()) {
            auto value = T{};
            if (!paramValueFromString(paramValueStr, value))
                throw ConfigError{paramList.position(),
                                  "Couldn't set parameter list element'" + name_ + "' value from '" + paramValueStr + "'"};
            paramListValue_.emplace_back(std::move(value));
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