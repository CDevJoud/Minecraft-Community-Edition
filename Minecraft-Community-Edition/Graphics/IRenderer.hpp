#pragma once
#include <cstdint>
#include <type_traits>

namespace bgfx {
	struct VertexBufferHandle;
}

namespace mce::gfx {

	using bgfx::VertexBufferHandle;

	class IRenderer {
	public:
		enum class Clear : uint16_t {
			None		   = 0x0000, //!< No clear Flags.
			Color		   = 0x0001, //!< Clear color.
			Depth          = 0x0002, //!< Clear depth.
			Stencil		   = 0x0004, //!< Clear stencil.
			DiscardColor0  = 0x0008, //!< Discard frame buffer attachment 0.
			DiscardColor1  = 0x0010, //!< Discard frame buffer attachment 1.
			DiscardColor2  = 0x0020, //!< Discard frame buffer attachment 2.
			DiscardColor3  = 0x0040, //!< Discard frame buffer attachment 3.
			DiscardColor4  = 0x0080, //!< Discard frame buffer attachment 4.
			DiscardColor5  = 0x0100, //!< Discard frame buffer attachment 5.
			DiscardColor6  = 0x0200, //!< Discard frame buffer attachment 6.
			DiscardColor7  = 0x0400, //!< Discard frame buffer attachment 7.
			DiscardDepth   = 0x0800, //!< Discard frame buffer depth attachment.
			DiscardStencil = 0x1000, //!< Discard frame buffer stencil attachment.

			DiscardColorMask = 0 | DiscardColor0 | DiscardColor1 | DiscardColor2 | DiscardColor3 | DiscardColor4 | DiscardColor5 | DiscardColor6 | DiscardColor7,
			DiscardMask      = 0 | DiscardColorMask | DiscardDepth | DiscardStencil
		};
		
		virtual void setClearColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0xFF) = 0;
		virtual void setViewSpace(uint16_t x, uint16_t y, uint16_t width, uint16_t height) = 0;
		
		virtual void setVertexBuffer(VertexBufferHandle handle) = 0;
		//virtual bgfx::VertexBufferHandle createVertexBuffer() = 0; 

		virtual void addFlag(Clear cFlag);
		virtual void removeFlag(Clear cFlag);
		virtual void toggleFlag(Clear cFlag);

		//void re

		constexpr bool hasFlag(IRenderer::Clear flag) const;

	private:

	protected:
		Clear clearFlags = Clear::None;
	};

	template<typename T>
	constexpr auto toUnderlying(T value) {
		return static_cast<std::underlying_type_t<T>>(value);
	}

	template<typename T>
	inline T operator|(T a, T b) {
		return static_cast<T>(
			toUnderlying(a) | toUnderlying(b)
			);
	}

	template<typename T>
	inline T& operator|=(T& a, T b) {
		a = a | b;
		return a;
	}

	template<typename T>
	constexpr T operator&(T a, T b) {
		return static_cast<T>(
			toUnderlying(a) & toUnderlying(b)
			);
	}

	template<typename T>
	inline T& operator &=(T& a, T b) {
		a = static_cast<T>(
			toUnderlying(a) & ~toUnderlying(b)
			);
		return a;
	}

	template<typename T>
	constexpr T operator^(T a, T b) {
		return static_cast<T>(
			toUnderlying(a) ^ toUnderlying(b)
			);
	}

	template<typename T>
	inline T& operator^=(T& a, T b) {
		a = a ^ b;
		return a;
	}

	template<typename T>
	constexpr T operator~(T a) {
		return static_cast<T>(~toUnderlying(a));
	}
	
}
