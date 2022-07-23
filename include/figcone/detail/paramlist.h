#pragma once
#include "iparam.h"
#include "utils.h"
#include <figcone_tree/tree.h>
#include <figcone/errors.h>
#include <figcone_tree/stringconverter.h>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

namespace figcone::detail {

template<typename TParamList>
class ParamList : public IParam{
public:
    ParamList(std::string name, TParamList& paramValue)
        : name_{std::move(name)}
        , paramListValue_{paramValue}
    {
        static_assert(is_sequence_container_v<remove_optional_t<TParamList>>,
                      "Param list field must be a sequence container or a sequence container placed in std::optional");
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
        if constexpr(is_optional<TParamList>::value)
            paramListValue_.emplace();

        if (!paramList.isList())
            throw ConfigError{"Parameter list '" + name_ + "': config parameter must be a list.", paramList.position()};
        for (const auto& paramValueStr : paramList.valueList()) {
            auto paramValue = convertFromString<typename remove_optional_t<TParamList>::value_type>(paramValueStr);
            if (!paramValue)
                throw ConfigError{
                        "Couldn't set parameter list element'" + name_ + "' value from '" + paramValueStr + "'",
                        paramList.position()};
            maybeOptValue(paramListValue_).emplace_back(std::move(*paramValue));
        }
    }

    bool hasValue() const override
    {
         if constexpr (is_optional<TParamList>::value)
            return true;
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
    TParamList& paramListValue_;
    bool hasValue_ = false;
    StreamPosition position_;
};

}