#include "GenericFunction.hpp"

namespace Hazel
{
	template<typename R, typename ...Args>
	GenericFunction<R, Args...>::GenericFunction()
	{
		f = std::function<void()>();
	}

	template<typename R, typename ...Args>
	void GenericFunction<R, Args...>::Set(std::function<R(Args...)> f)
	{
		f = f;
	}

	template<typename R, typename ...Args>
	void GenericFunction<R, Args...>::Call(...)
	{
		f(...);
	}
}