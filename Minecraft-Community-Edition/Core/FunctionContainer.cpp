#include "FunctionContainer.hpp"

namespace mce {
	FunctionContainer::FunctionContainer(std::initializer_list<std::pair<eastl::string, eastl::any>> initList) {
		for (const auto& [name, func] : initList) {
			FunctionContainer::functions[name] = func;
		}
	}

	bool FunctionContainer::findFunction(const eastl::string& name) {
		return static_cast<bool>(this->functions.count(name));
	}
}
