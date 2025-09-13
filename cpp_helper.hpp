#pragma once

#include <concepts>

namespace cpp_helper {

template<typename T1, typename T2>
class overloads {
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

}
