#pragma once

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <array>

namespace cpp_helper {

template<typename T1, typename T2, typename... Ts>
struct overloads : public overloads<T1, overloads<T2, Ts...>>{
    constexpr overloads() = default;
    constexpr overloads(T1 t1, T2 t2, Ts... ts)
        : overloads<T1, overloads<T2, Ts...>>{t1, {t2, ts...}}
    {}

    using overloads<T1, overloads<T2, Ts...>>::operator();
};

template<typename T1, typename T2>
class overloads<T1, T2> : public T1, public T2{
public:
    constexpr overloads() = default;
	constexpr overloads(T1 t1, T2 t2)
		:
		T1{t1},
		T2{t2}
	{}
    using T1::operator();
    using T2::operator();
};

template<int Index, typename T0, typename T1>
struct select {
};
template<typename T0, typename T1>
struct select<0, T0, T1> {
	using type = T0;
};
template<typename T0, typename T1>
struct select<1, T0, T1> {
	using type = T1;
};
template<int Index, typename T0, typename T1>
using select_t = typename select<Index, T0, T1>::type;

template<typename T, typename... U>
concept same_as_one_of = (std::same_as<T,U> || ...);

template<size_t N>
struct bitset{
    constexpr bitset() = default;
    constexpr bitset(uint32_t i) : bit32{i} {}
    constexpr bitset(int32_t i) : bit32{static_cast<uint32_t>(i)} {}
    struct reference {
        bitset& bits;
        size_t index;

        constexpr reference& operator=(bool x) {
            uint32_t bit32 = bits.bit32[index/32];
            uint32_t mask = ~(1u << (index%32));
            bits.bit32[index/32] = (bit32 & mask) | (x << (index%32));
            return *this;
        }
        constexpr operator bool() const {
            return (bits.bit32[index/32] >> (index%32)) & 1;
        }
        constexpr reference& flip() {
            bits.bit32[index/32] ^= (1u << (index%32));
            return *this;
        }
    };
    constexpr operator uint32_t() const {
        static_assert(N <= 32);
        return bit32[0];
    }

    std::array<uint32_t,(N+31)/32> bit32;

};
template<size_t N1, size_t N2>
requires (N1 + N2 > 32)
constexpr bitset<N1+N2> operator+(const bitset<N1> lhs, const bitset<N2> rhs) {
    bitset<N1+N2> res{};
    static_assert(N1+N2 < 32);
}
template<size_t N1, size_t N2>
requires (N1 + N2 <= 32 && N1+N2 > 0)
constexpr bitset<N1+N2> operator+(const bitset<N1> lhs, const bitset<N2> rhs) {
    bitset<N1+N2> res{};
    res.bit32[0] = (lhs.bit32[0] << N2) | rhs.bit32[0];
    return res;
}
template<size_t N1, size_t N2>
requires (N1+N2 == 0)
constexpr bitset<N1+N2> operator+(const bitset<N1> lhs, const bitset<N2> rhs) {
    bitset<N1+N2> res{};
    return res;
}

}
