#ifndef FIGCONE_LOADINGERROR_H
#define FIGCONE_LOADINGERROR_H

#include <figcone/errors.h>

namespace figcone::detail {

class LoadingError : public Error {
    using Error::Error;
};

} //namespace figcone::detail

#endif //FIGCONE_LOADINGERROR_H