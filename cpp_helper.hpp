#pragma once

#include <concepts>

namespace cpp_helper {

template<typename T1, typename T2>
class overloads {
public:
	overloads(T1 t1, T2 t2)
		:
		m_t1{t1},
		m_t2{t2}
	{}
	template<typename... Args>
		requires std::invocable<T1, Args...>
	auto operator()(Args... args) && -> std::invoke_result_t<T1, Args...>{
		return m_t1(args...);
	}
	
	template<typename... Args>
		requires std::invocable<T1, Args...>
	auto operator()(Args... args) & -> std::invoke_result_t<T1, Args...> {
		return m_t1(args...);
	}

	template<typename... Args>
		requires std::invocable<T2, Args...>
	auto operator()(Args... args) && -> std::invoke_result_t<T2, Args...> {
		return m_t2(args...);
	}

	template<typename... Args>
		requires std::invocable<T2, Args...>
	auto operator()(Args... args) & -> std::invoke_result_t<T2, Args...> {
		return m_t2(args...);
	}

private:
	T1 m_t1;
	T2 m_t2;
};
}
