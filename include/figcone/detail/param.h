#pragma once
#include "iparam.h"
#include "iconfigentity.h"
#include "paramvaluefromstring.h"
#include <figcone_tree/tree.h>
#include <figcone/errors.h>
#include <string>
#include <sstream>
#include <algorithm>

namespace figcone::detail {

template<typename T>
class ConfigParam : public IConfigParam{
public:
    ConfigParam(std::string name, T& paramValue)
        : name_{std::move(name)}
        , paramValue_{paramValue}
    {
    }

    void markValueIsSet()
    {
        hasValue_ = true;
    }

private:
    void load(const figcone::TreeParam& param) override
    {
        const auto& paramVal = param.value();
        if (!paramValueFromString(paramVal, paramValue_))
            throw ConfigError{"Couldn't set parameter '" + name_ + "' value from '" + paramVal + "'", param.position()};
        hasValue_ = true;
        position_ = param.position();
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
        return "Parameter '" + name_ + "'";
    }

private:
    std::string name_;
    T& paramValue_;
    bool hasValue_ = false;
    StreamPosition position_;
};

}