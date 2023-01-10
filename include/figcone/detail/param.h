#pragma once
#include "iconfigentity.h"
#include "iparam.h"
#include "stringconverter.h"
#include "utils.h"
#include "external/sfun/functional.h"
#include "external/sfun/type_traits.h"
#include <figcone/errors.h>
#include <figcone_tree/stringconverter.h>
#include <figcone_tree/tree.h>
#include <algorithm>
#include <sstream>
#include <string>

namespace figcone::detail {

template<typename T>
class Param : public IParam {
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
        auto paramReadResult = convertFromString<T>(param.value());
        auto readResultVisitor = sfun::overloaded{
                [&](const T& param)
                {
                    paramValue_ = param;
                },
                [&](const StringConversionError& error)
                {
                    throw ConfigError{
                            "Couldn't set parameter '" + name_ + "' value from '" + param.value() + "'" +
                                    (!error.message.empty() ? ": " + error.message : ""),
                            param.position()};
                }};

        std::visit(readResultVisitor, paramReadResult);
    }

    bool hasValue() const override
    {
        if constexpr (sfun::is_optional_v<T>)
            return true;
        else
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

} //namespace figcone::detail