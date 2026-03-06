#pragma once
#include "EASTL/initializer_list.h"
#include "EASTL/string.h"
#include "EASTL/any.h"
#include "EASTL/unordered_map.h"
#include "EASTL/functional.h"
#include "..\IO\Logger.hpp"
#include <source_location>

namespace mce {
	/**
	 * @brief Container for storing and invoking named functions.
	 *
	 * This class allows storing callables (wrapped as eastl::any) associated
	 * with string keys. Functions can be retrieved, cast to the expected
	 * signature, and invoked. The API supports adding functions, calling
	 * them by name, checking for existence, and retrieving a stored function
	 * as an eastl::function object.
	 * 
	 * 
	 * A usage example:
	 * FunctionContainer fc = {
	 *		{
	 *			"foo", eastl::function<int(int)>([](int value) -> int{
	 *				return value + 1;
	 *			})
	 *		}
	 * };
	 * if(fc.findFunction("foo")
	 *		fc.callFunction<int, int>("foo", 1);
	 */
	class FunctionContainer {
	public:
		/**
		 * @brief Default constructor.
		 */
		FunctionContainer() = default;

		/**
		 * @brief Construct the container with an initializer list of name/value pairs.
		 *
		 * The initializer list should contain pairs of function name and
		 * eastl::any containing the callable.
		 *
		 * @param initlist Initializer list of pairs (name, callable-as-eastl::any).
		 */
		FunctionContainer(std::initializer_list<std::pair<eastl::string, eastl::any>> initlist);

		/**
		 * @brief Add a function to the container.
		 *
		 * The callable will be stored in the internal map under the provided
		 * name. The callable is stored as an eastl::any and should be retrievable
		 * later by using the same signature when casting.
		 *
		 * @tparam Functor Type of the callable being stored.
		 * @param name The name under which the function will be stored.
		 * @param functor The callable to store.
		 */
		template<typename Functor>
		void addFunction(const eastl::string& name, Functor functor);

		/**
		 * @brief Call a stored function by name with the provided arguments.
		 *
		 * The template parameters specify the expected return type and argument
		 * types of the stored function. If the named function exists it will be
		 * cast to eastl::function<Return(Arguments...)> and invoked. If the
		 * function is not found an error is logged via MCE_ERROR and behavior is
		 * undefined for the returned value.
		 *
		 * @tparam Return The expected return type of the function.
		 * @tparam Arguments The expected argument types of the function.
		 * @param name The name of the function to call.
		 * @param args Arguments to forward to the function.
		 * @return Return The value returned by the invoked function.
		 */
		template<typename Return, typename... Arguments>
		Return callFunction(const eastl::string& name, Arguments&&... args);

		/**
		 * @brief Check whether a function with the given name exists in the container.
		 *
		 * @param name The name to check for.
		 * @return true if a function with the given name exists, false otherwise.
		 */
		bool findFunction(const eastl::string& name);

		/**
		 * @brief Retrieve a stored function as an eastl::function via operator[].
		 *
		 * The template specifies the expected signature of the function stored
		 * under the given name. If no function exists an error is logged and a
		 * null function is returned.
		 *
		 * @tparam Return The return type of the function.
		 * @tparam Arguments The argument types of the function.
		 * @param name The name of the stored function.
		 * @return eastl::function<Return(Arguments...)> The callable if found;
		 *         a null function otherwise.
		 */
		template<typename Return, typename... Arguments>
		eastl::function<Return(Arguments...)> operator[](const eastl::string& name);

		/**
		 * @brief Retrieve a stored function as an eastl::function.
		 *
		 * Same behavior as operator[] but provided with an explicit named method.
		 *
		 * @tparam Return The return type of the function.
		 * @tparam Arguments The argument types of the function.
		 * @param name The name of the stored function.
		 * @return eastl::function<Return(Arguments...)> The callable if found;
		 *         a null function otherwise.
		 */
		template<typename Return, typename... Arguments>
		eastl::function<Return(Arguments...)> getFunction(const eastl::string& name);
	private:
		/// Internal map of name to stored callable (wrapped in eastl::any)
		eastl::unordered_map<eastl::string, eastl::any> functions;
	};

	// Template implementations
	/**
	 * @brief Add a function implementation.
	 *
	 * See addFunction documentation above.
	 */
	template<typename Functor>
	inline void FunctionContainer::addFunction(const eastl::string& name, Functor functor) {
		FunctionContainer::functions[name] = functor;
	}

	/**
	 * @brief Call a stored function implementation.
	 *
	 * See callFunction documentation above.
	 */
	template<typename Return, typename ...Arguments>
	inline Return FunctionContainer::callFunction(const eastl::string& name, Arguments && ...args) {
		if (FunctionContainer::functions.find(name) != FunctionContainer::functions.end()) {
			auto& func = FunctionContainer::functions[name];
			auto castedFunc = eastl::any_cast<eastl::function<Return(Arguments...)>>(func);
			return castedFunc(eastl::forward<Arguments>(args)...);
		}
		else {
			MCE_ERROR("Function \"{}\" not found! File:", name);
		}
	}

	/**
	 * @brief operator[] implementation to retrieve stored function.
	 *
	 * See operator[] documentation above.
	 */
	template<typename Return, typename ...Arguments>
	inline eastl::function<Return(Arguments...)> FunctionContainer::operator[](const eastl::string& name) {
		if (FunctionContainer::functions.find(name) != FunctionContainer::functions.end()) {
			return eastl::any_cast<eastl::function<Return(Arguments...)>>(FunctionContainer::functions[name]);
		}
		else {
			MCE_ERROR("Function \"{}\" not found! File:", name);
			return nullptr;
		}
	}

	/**
	 * @brief getFunction implementation to retrieve stored function.
	 *
	 * See getFunction documentation above.
	 */
	template<typename Return, typename ...Arguments>
	inline eastl::function<Return(Arguments...)> FunctionContainer::getFunction(const eastl::string& name) {
		if (FunctionContainer::functions.find(name) != FunctionContainer::functions.end()) {
			return eastl::any_cast<eastl::function<Return(Arguments...)>>(FunctionContainer::functions[name]);
		}
		else { 
			MCE_ERROR("Function \"{}\" not found! File:", name);
			return nullptr;
		}
	}
}