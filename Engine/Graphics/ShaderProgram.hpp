#pragma once
#include <EASTL/vector.h>
#include <EASTL/unordered_map.h>
#include <EASTL/shared_ptr.h>
#include "../libs/bgfx/bgfx.h"
#include <string>
#include <unordered_map>



namespace dms::gfx {
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

		void setUniform(const std::string& name, float f1, float f2, float f3, float f4);

		void setUniform(const std::string& name, const float* mat4x4);

	private:
		std::unordered_map<std::string, bgfx::UniformHandle> uniform;
		bgfx::ProgramHandle program;
	};
}
