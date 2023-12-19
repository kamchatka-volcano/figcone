#pragma once

namespace figcone {

template<typename T>
struct PostProcessor {
    void operator()(T&){};
};

} //namespace figcone