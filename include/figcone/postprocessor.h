#ifndef FIGCONE_POST_PROCESSOR_H
#define FIGCONE_POST_PROCESSOR_H

namespace figcone {

template<typename T>
struct PostProcessor {
    void operator()(T&){};
};

} //namespace figcone

#endif //FIGCONE_POST_PROCESSOR_H