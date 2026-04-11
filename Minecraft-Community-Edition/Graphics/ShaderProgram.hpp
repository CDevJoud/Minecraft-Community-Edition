#pragma once
#include <EASTL/vector.h>
#include <bgfx/bgfx.h>
#include <string>
namespace mce::gfx {
	class ShaderProgram {
	public:
		ShaderProgram(const eastl::vector<uint8_t>& vertexShader, const eastl::vector<uint8_t>& fragmentShader, bool& success, bool destroyShaderBin = false, const std::string& dbName = "");
		~ShaderProgram();
		bgfx::ProgramHandle getProgramHandle() const;

	private:
		bgfx::ProgramHandle program;
	};
}
