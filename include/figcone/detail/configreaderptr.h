#pragma once

namespace figcone::detail {
class IConfigReader;

class ConfigReaderPtr {
    friend class IConfigReader;

private:
    ConfigReaderPtr(IConfigReader* reader)
        : reader_{reader}
    {}

public:
    ConfigReaderPtr() = default;

    const IConfigReader* operator->() const
    {
        return reader_;
    }

    IConfigReader* operator->()
    {
        return reader_;
    }

    const IConfigReader& operator*() const
    {
        return *reader_;
    }

    IConfigReader& operator*()
    {
        return *reader_;
    }

    operator bool() const
    {
        return reader_;
    }

private:
    IConfigReader* reader_ = nullptr;
};

}