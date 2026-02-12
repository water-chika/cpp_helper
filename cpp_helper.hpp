#pragma once

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <array>
#include <limits>
#include <cassert>
#include <string>
#include <cuchar>

namespace cpp_helper {

template<typename T1, typename T2, typename... Ts>
struct overloads : public overloads<T1, overloads<T2, Ts...>>{
    constexpr overloads() = default;
    constexpr overloads(T1 t1, T2 t2, Ts... ts)
        : overloads<T1, overloads<T2, Ts...>>{t1, {t2, ts...}}
    {}
};

template<typename T1, typename T2>
class overloads<T1, T2> {
public:
    constexpr overloads() = default;
	constexpr overloads(T1 t1, T2 t2)
		:
		m_t1{t1},
		m_t2{t2}
	{}
	template<typename... Args>
		requires std::invocable<T1, Args...>
	constexpr auto operator()(Args... args) && -> std::invoke_result_t<T1, Args...>{
		return m_t1(args...);
	}
	
	template<typename... Args>
		requires std::invocable<T1, Args...>
	constexpr auto operator()(Args... args) & -> std::invoke_result_t<T1, Args...> {
		return m_t1(args...);
	}

	template<typename... Args>
		requires std::invocable<T2, Args...>
	constexpr auto operator()(Args... args) && -> std::invoke_result_t<T2, Args...> {
		return m_t2(args...);
	}

	template<typename... Args>
		requires std::invocable<T2, Args...>
	constexpr auto operator()(Args... args) & -> std::invoke_result_t<T2, Args...> {
		return m_t2(args...);
	}

private:
	T1 m_t1;
	T2 m_t2;
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

template<typename T>
class iterator{
public:
    iterator(T& ref, size_t i) : m_ref{ref}, m_i{i} {}
    iterator(const iterator&) = default;

    iterator& operator++() {
        m_i++;
        return *this;
    }
    iterator operator++(int) {
        m_i++;
        return *this;
    }

    auto& operator*() {
        return m_ref[m_i];
    }
private:
    T& m_ref;
    size_t m_i;
};

template<typename T, size_t N>
class array {
public:
    array() = default;
    __device__ __host__
    auto& operator[](size_t i) const {
        return m_elements[i];
    }
    __device__ __host__
    auto& operator[](size_t i) {
        return m_elements[i];
    }

    __device__ __host__
    auto begin() {
        return iterator{*this, 0};
    }
    __device__ __host__
    auto end() {
        return iterator{*this, N};
    }
private:
    T m_elements[N];
};

__device__ __host__
void swap(auto&& lhs, auto&& rhs) {
    auto t = lhs;
    lhs = rhs;
    rhs = t;
}
#undef max
template<typename T>
auto checked_cast(auto v) {
    assert(v < std::numeric_limits<T>::max());
    return static_cast<T>(v);
}

template<typename T, typename SizeType, SizeType MaxSize>
class small_vector {
public:
    __device__ __host__
    small_vector(SizeType n) : m_size{n} {}
    __device__ __host__
    small_vector(std::initializer_list<T> ts) : m_elements{},
        m_size{
            checked_cast<SizeType>(ts.size())
        }
    {
        size_t i = 0;
        for (auto ite = ts.begin(); ite != ts.end(); ++ite, ++i) {
            m_elements[i] = *ite;
        }
    }
    __device__ __host__
    auto size() const {
        return m_size;
    }
    __device__ __host__
    void resize(SizeType s) {
        m_size = s;
    }
    __device__ __host__
    auto begin() {
        return iterator{*this, 0};
    }
    __device__ __host__
    auto end() {
        return iterator{*this, size()};
    }
    __device__ __host__
    auto& operator[](size_t i) {
	    return m_elements[i];
    }
    __device__ __host__
    auto& operator[](size_t i) const {
	    return m_elements[i];
    }
    template<typename... Args>
    void emplace_back(Args... args) {
        m_elements[m_size++] = T{args...};
    }
private:
    T m_elements[MaxSize];
    SizeType m_size;
};

template<typename T>
class vector {
public:
    static constexpr size_t MAX_SIZE = 8;
    __device__ __host__
    vector(size_t n) : m_size{n} {}
    __device__ __host__
    vector(std::initializer_list<T> ts) : m_elements{}, m_size{ts.size()} {
        size_t i = 0;
        for (auto ite = ts.begin(); ite != ts.end(); ++ite, ++i) {
            m_elements[i] = *ite;
        }
    }
    __device__ __host__
    auto size() const {
        return m_size;
    }
    __device__ __host__
    void resize(size_t s) {
        m_size = s;
    }
    __device__ __host__
    auto begin() {
        return iterator{*this, 0};
    }
    __device__ __host__
    auto end() {
        return iterator{*this, size()};
    }
    __device__ __host__
    auto& operator[](size_t i) {
	    return m_elements[i];
    }
    __device__ __host__
    auto& operator[](size_t i) const {
	    return m_elements[i];
    }
private:
    T m_elements[MAX_SIZE];
    size_t m_size;
};

struct empty_type {};

template<bool Valid, typename T>
using valid_if_t = select_t<Valid, empty_type,T>;

template<typename T, size_t N1, size_t N2>
constexpr auto merge(std::array<T,N1> lhs, std::array<T,N2> rhs) {
    std::array<T,N1+N2> res{};
    std::move(lhs.begin(), lhs.end(), res.begin());
    std::move(rhs.begin(), rhs.end(), res.begin()+lhs.size());
    return res;
}

template<typename T, size_t... Sizes>
class multiple_array {
public:

private:
    static constexpr size_t offset(size_t i) {
        size_t res = 0;
        size_t s[] = {Sizes...};
        for (size_t j = 0; j < sizeof...(Sizes); ++j) {
            res += s[j];
        }
        return res;
    }
    static constexpr auto offsets = std::to_array<size_t>({offset(Sizes)...});
    std::array<T, (Sizes + ...)> m_values;
};

std::string to_mb(char16_t* u16_str) {
    std::string res{};

    std::mbstate_t state{};
    for (auto p = u16_str; *p != 0; p++) {
        auto str = std::array<char, MB_LEN_MAX>();
        auto rc = std::c16rtomb(str.data(), *p, &state);
        if (rc != (std::size_t)-1){
            res += std::string_view{str.data(), rc};
        }
    }
    return res;
}

}
