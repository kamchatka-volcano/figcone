#pragma once

namespace figcone::detail {

class IValidator{
public:
    virtual ~IValidator() = default;
    virtual void validate() = 0;
};

}