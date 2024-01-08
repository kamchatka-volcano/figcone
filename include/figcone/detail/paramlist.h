#ifndef FIGCONE_PARAMLIST_H
#define FIGCONE_PARAMLIST_H

#include "iparam.h"
#include "stringconverter.h"
#include "utils.h"
#include "external/sfun/type_traits.h"
#include <figcone/errors.h>
#include <figcone_tree/stringconverter.h>
#include <figcone_tree/tree.h>
#include <algorithm>
#include <sstream>
#include <string>
#include <vector>

namespace figcone::detail {

template<typename TParamList>
class ParamList : public IParam {
    static_assert(
            sfun::is_dynamic_sequence_container_v<sfun::remove_optional_t<TParamList>>,
            "Param list field must be a sequence container or a sequence container placed in std::optional");

public:
    ParamList(std::string name, TParamList& paramValue)
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
        if constexpr (sfun::is_optional_v<TParamList>)
            paramListValue_.emplace();

        if (!paramList.isList())
            throw ConfigError{"Parameter list '" + name_ + "': config parameter must be a list.", paramList.position()};
        for (const auto& paramValueStr : paramList.valueList()) {
            using Param = typename sfun::remove_optional_t<TParamList>::value_type;
            auto paramReadResult = convertFromString<Param>(paramValueStr);
            auto readResultVisitor = sfun::overloaded{
                    [&](const Param& param)
                    {
                        maybeOptValue(paramListValue_).emplace_back(param);
                    },
                    [&](const StringConversionError& error)
                    {
                        throw ConfigError{
                                "Couldn't set parameter list element'" + name_ + "' value from '" + paramValueStr +
                                        "'" + (!error.message.empty() ? ": " + error.message : ""),
                                paramList.position()};
                    }};

            std::visit(readResultVisitor, paramReadResult);
        }
    }

    bool hasValue() const override
    {
        if constexpr (sfun::is_optional_v<TParamList>)
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
        return "Parameter list '" + name_ + "'";
    }

private:
    std::string name_;
    TParamList& paramListValue_;
    bool hasValue_ = false;
    StreamPosition position_;
};

} //namespace figcone::detail

#endif //FIGCONE_PARAMLIST_H