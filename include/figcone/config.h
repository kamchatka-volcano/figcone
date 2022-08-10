#pragma once
#include "detail/initializedoptional.h"
#include "detail/configmacros.h"
#include "detail/inode.h"
#include "detail/ivalidator.h"
#include "detail/iconfigreader.h"
#include "detail/dict.h"
#include "detail/paramcreator.h"
#include "detail/nodelistcreator.h"
#include "detail/paramlistcreator.h"
#include "detail/dictcreator.h"
#include "detail/nameconverter.h"
#include "detail/nameof_import.h"
#include "detail/utils.h"
#include <figcone_tree/iparser.h>
#include <figcone_tree/tree.h>
#include <vector>
#include <memory>
#include <map>
#include <string>
#include <optional>
#include <filesystem>
#include <fstream>

namespace figcone {
namespace detail {
class IParam;
}

class Config{

public:
    Config() = default;
    Config(detail::ConfigReaderPtr reader)
        : cfgReader_{reader}
    {}

     Config(Config&& other)
    {
        if (cfgReader() && other.cfgReader())
            cfgReader()->swapContents(other.cfgReader());
    }
    Config& operator=(Config&& other)
    {
        if (cfgReader() && other.cfgReader())
            cfgReader()->swapContents(other.cfgReader());
        return *this;
    }

protected:
    template<auto member>
    auto node(const std::string& memberName)
    {
        auto ptr = decltype(member){};
        return node<member>(ptr, memberName);
    }

    template<auto member>
    auto dict(const std::string& memberName)
    {
        auto ptr = decltype(member){};
        return dict<member>(ptr, memberName);
    }

    template<auto member>
    auto nodeList(const std::string& memberName)
    {
        auto ptr = decltype(member){};
        return nodeList<member>(ptr, memberName);
    }

    template<auto member>
    auto copyNodeList(const std::string& memberName)
    {
        auto ptr = decltype(member){};
        return copyNodeList<member>(ptr, memberName);
    }

    template<auto member>
    auto param(const std::string& memberName)
    {
        auto ptr = decltype(member){};
        return param<member>(ptr, memberName);
    }

    template<auto member>
    auto paramList(const std::string& memberName)
    {
        auto ptr = decltype(member){};
        return paramList<member>(ptr, memberName);
    }

#ifdef FIGCONE_NAMEOF_AVAILABLE
    template<auto member>
    auto node()
    {
        return node<member>(std::string{nameof::nameof_member<member>()});
    }

    template<auto member>
    auto dict()
    {
        return dict<member>(std::string{nameof::nameof_member<member>()});
    }

    template<auto member>
    auto nodeList()
    {
        return nodeList<member>(std::string{nameof::nameof_member<member>()});
    }

    template<auto member>
    auto copyNodeList()
    {
        return copyNodeList<member>(std::string{nameof::nameof_member<member>()});
    }

    template <auto member>
    auto param()
    {
        return param<member>(std::string{nameof::nameof_member<member>()});
    }

    template <auto member>
    auto paramList()
    {
        return paramList<member>(std::string{nameof::nameof_member<member>()});
    }
#endif

    detail::ConfigReaderPtr cfgReader() const
    {
        return cfgReader_;
    }

private:
    template <auto member, typename T, typename TCfg>
    auto node(T TCfg::*, const std::string& memberName)
    {
        auto cfg = static_cast<TCfg*>(this);
        return detail::NodeCreator<T>{cfgReader(), memberName, cfg->*member};
    }

    template <auto member, typename TMap, typename TCfg>
    auto dict(TMap TCfg::*, const std::string& memberName)
    {
        auto cfg = static_cast<TCfg*>(this);
        return detail::DictCreator<TMap>{cfgReader(), memberName, cfg->*member};
    }

    template <auto member, typename TCfgList, typename TCfg>
    auto nodeList(TCfgList TCfg::*, const std::string& memberName)
    {
        auto cfg = static_cast<TCfg*>(this);
        return detail::NodeListCreator<TCfgList>{cfgReader(), memberName, cfg->*member};
    }

    template <auto member, typename TCfgList, typename TCfg>
    auto copyNodeList(TCfgList TCfg::*, const std::string& memberName)
    {
        auto cfg = static_cast<TCfg*>(this);
        return detail::NodeListCreator<TCfgList>{cfgReader(), memberName, cfg->*member, detail::NodeListType::Copy};
    }

    template <auto member, typename T, typename TCfg>
    auto param(T TCfg::*, const std::string& memberName)
    {
        auto cfg = static_cast<TCfg*>(this);
        return detail::ParamCreator<T>{cfgReader(), memberName, cfg->*member};
    }

    template <auto member, typename T, typename TCfg>
    auto paramList(T TCfg::*, const std::string& memberName)
    {
        auto cfg = static_cast<TCfg*>(this);
        return detail::ParamListCreator<T>{cfgReader(), memberName, cfg->*member};
    }

private:
    detail::ConfigReaderPtr cfgReader_;
    friend class detail::IConfigReader;
};

template<class TConfig>
class ConfigT : public Config{
protected:
    using T = TConfig;
};

template<typename T>
using optional = std::conditional_t<std::is_base_of_v<Config, T>, detail::InitializedOptional<T>, std::optional<T>>;

}
