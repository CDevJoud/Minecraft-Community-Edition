#include "ShaderProgram.hpp"

namespace mce::gfx {
	ShaderProgram::ShaderProgram(const eastl::vector<uint8_t>& vertexShader, const eastl::vector<uint8_t>& fragmentShader, bool& success, bool destroyShaderBin, const std::string& dbgName) :
		program(BGFX_INVALID_HANDLE) {
		const bgfx::Memory* vsmem = bgfx::copy(vertexShader.data(), vertexShader.size());
		const bgfx::Memory* fsmem = bgfx::copy(fragmentShader.data(), fragmentShader.size());
		bgfx::ShaderHandle vs = bgfx::createShader(vsmem);
		if (!bgfx::isValid(vs)) {
			success = false;
		} else {
			bgfx::setName(vs, dbgName.c_str());
			bgfx::ShaderHandle fs = bgfx::createShader(fsmem);
			if (!bgfx::isValid(fs)) {
				success = false;
			}
			else {
				bgfx::setName(fs, dbgName.c_str());
				ShaderProgram::program = bgfx::createProgram(vs, fs, destroyShaderBin);
				success = bgfx::isValid(ShaderProgram::program);
				bgfx::destroy(vs);
				bgfx::destroy(fs);
				//we flush the command buffer and dispatch the commands to the GPU
				//bgfx::frame(BGFX_FRAME_FLUSH);
			}
		}

	}
	ShaderProgram::~ShaderProgram() {
		if (bgfx::isValid(ShaderProgram::program)) {
			bgfx::destroy(ShaderProgram::program);
			ShaderProgram::program = BGFX_INVALID_HANDLE;
		}
	}
	bgfx::ProgramHandle ShaderProgram::getProgramHandle() const {
		return ShaderProgram::program;
	}
}
