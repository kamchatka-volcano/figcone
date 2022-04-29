#pragma once
#include "iparam.h"
#include "iconfigentity.h"
#include "utils.h"
#include <figcone_tree/tree.h>
#include <figcone/errors.h>
#include <figcone_tree/stringconverter.h>
#include <string>
#include <sstream>
#include <algorithm>

namespace figcone::detail {

template<typename T>
class Param : public IParam{
public:
    Param(std::string name, T& paramValue)
        : name_{std::move(name)}
        , paramValue_{paramValue}
    {
    }

    void markValueIsSet()
    {
        hasValue_ = true;
    }

private:
    void load(const TreeParam& param) override
    {
        hasValue_ = true;
        position_ = param.position();
        if (!param.isItem())
            throw ConfigError{"Parameter '" + name_ + "': config parameter can't be a list.", param.position()};
        auto paramVal = convertFromString<T>(param.value());
        if (!paramVal)
            throw ConfigError{"Couldn't set parameter '" + name_ + "' value from '" + param.value() + "'", param.position()};
        paramValue_ = *paramVal;
    }

    bool hasValue() const override
    {
        if constexpr (is_optional<T>::value)
            return true;
        return hasValue_;
    }

    StreamPosition position() override
    {
        return position_;
    }

    std::string description() override
    {
        return "Parameter '" + name_ + "'";
    }

private:
    std::string name_;
    T& paramValue_;
    bool hasValue_ = false;
    StreamPosition position_;
};

}