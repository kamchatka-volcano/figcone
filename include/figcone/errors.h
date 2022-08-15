#pragma once
#include <figcone_tree/errors.h>

namespace figcone{

class ValidationError : public Error
{
    using Error::Error;
};

}