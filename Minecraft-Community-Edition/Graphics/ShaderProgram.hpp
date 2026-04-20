#pragma once
#include <EASTL/vector.h>
#include <EASTL/unordered_map.h>
#include <EASTL/shared_ptr.h>
#include <bgfx/bgfx.h>
#include <string>
#include <unordered_map>



namespace mce::gfx {
	class RenderFactory;
	class Texture;

	class ShaderProgram {
		friend class RenderFactory;

		template<typename... Args>
		static eastl::shared_ptr<ShaderProgram> createInstance(Args&&... args) {
			struct EnableMakeShared : public ShaderProgram {
				EnableMakeShared(Args&&... a) : ShaderProgram(eastl::forward<Args>(a)...) {}
			};

			return eastl::make_shared<EnableMakeShared>(eastl::forward<Args>(args)...);
		}

		ShaderProgram(const eastl::vector<uint8_t>& vertexShader, const eastl::vector<uint8_t>& fragmentShader, bool& success, bool destroyShaderBin = false, const std::string& dbName = "");
		~ShaderProgram();
	public:
		bgfx::ProgramHandle getProgramHandle() const;

		void setUniform(const std::string& name, const eastl::shared_ptr<Texture>& texture);

	private:
		std::unordered_map<std::string, bgfx::UniformHandle> uniform;
		bgfx::ProgramHandle program;
	};
}
