#pragma once
#include <figcone/errors.h>

namespace figcone::detail {

class LoadingError : public Error{
    using Error::Error;
};

}