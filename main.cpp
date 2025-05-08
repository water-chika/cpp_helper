#include <cpp_helper.hpp>
#include <utility>
#include <cassert>

int main() {
    auto fun = cpp_helper::overloads{
        [](auto x){
            return x;
        },
        [](auto x, auto y) {
            return std::pair{x, y};
        }
    };

    auto [x, y] = fun(1, 3.0f);
    assert(x == 1 && y == 3.0f);
    return 0;
}