#include "ShaderProgram.hpp"
#include "Texture.hpp"

namespace dms::gfx {
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
    void ShaderProgram::setUniform(const std::string& name, const eastl::shared_ptr<Texture>& texture) {
		auto it = uniform.find(name);
		if (it != uniform.end()) {
			bgfx::setTexture(0, it->second, texture->getTextureHandle());
		}
		else {
			auto u = bgfx::createUniform(name.c_str(), bgfx::UniformType::Sampler);
			uniform[name] = u;
			bgfx::setTexture(0, u, texture->getTextureHandle());
		}
	}
	void ShaderProgram::setUniform(const std::string& name, float f1, float f2, float f3, float f4) {
		auto it = uniform.find(name);
		if (it != uniform.end()) {
			float data[4] = { f1, f2, f3, f4 };
			bgfx::setUniform(it->second, data);
		}
		else {
			auto u = bgfx::createUniform(name.c_str(), bgfx::UniformType::Vec4);
			uniform[name] = u;
			float data[4] = { f1, f2, f3, f4 };
			bgfx::setUniform(u, data);
		}
	}
	void ShaderProgram::setUniform(const std::string& name, const float* mat4x4) {
		auto it = uniform.find(name);
		if (it != uniform.end()) {
			bgfx::setUniform(it->second, mat4x4);
		}
		else {
			auto u = bgfx::createUniform(name.c_str(), bgfx::UniformType::Mat4);
			uniform[name] = u;
			bgfx::setUniform(u, mat4x4);
		}
	}
}
