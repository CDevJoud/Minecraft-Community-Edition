#pragma once
#include <cstdint>

namespace mce::gfx::flags {

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

	struct Clear {
		enum Enum : uint16_t {
			None			 = 0x0000, //!< No clear Flags.
			Color			 = 0x0001, //!< Clear color.
			Depth			 = 0x0002, //!< Clear depth.
			Stencil			 = 0x0004, //!< Clear stencil.
			DiscardColor0	 = 0x0008, //!< Discard frame buffer attachment 0.
			DiscardColor1	 = 0x0010, //!< Discard frame buffer attachment 1.
			DiscardColor2	 = 0x0020, //!< Discard frame buffer attachment 2.
			DiscardColor3	 = 0x0040, //!< Discard frame buffer attachment 3.
			DiscardColor4	 = 0x0080, //!< Discard frame buffer attachment 4.
			DiscardColor5	 = 0x0100, //!< Discard frame buffer attachment 5.
			DiscardColor6	 = 0x0200, //!< Discard frame buffer attachment 6.
			DiscardColor7	 = 0x0400, //!< Discard frame buffer attachment 7.
			DiscardDepth	 = 0x0800, //!< Discard frame buffer depth attachment.
			DiscardStencil	 = 0x1000, //!< Discard frame buffer stencil attachment.

			DiscardColorMask = 0 | DiscardColor0    | DiscardColor1 | DiscardColor2 | DiscardColor3 | DiscardColor4 | DiscardColor5 | DiscardColor6 | DiscardColor7,
			DiscardMask		 = 0 | DiscardColorMask | DiscardDepth  | DiscardStencil
		};
		Clear& addFlag(Clear::Enum cFlag) {
			this->cFlag |= cFlag;
			return *this;
		}
		Clear& removeFlag(Clear::Enum cFlag) {
			this->cFlag &= ~cFlag;
			return *this;
		}
		Clear& toggleFlag(Clear::Enum cFlag) {
			this->cFlag ^= cFlag;
			return *this;
		}

		constexpr bool hasFlag(Clear::Enum cFlag) const {
			return (toUnderlying(this->cFlag) & toUnderlying(cFlag)) != 0;
		}
		Enum cFlag = Enum::None; // clear flag
	};

	struct Buffer {
		enum Enum {
			ComputeFormat_8X1   = 0x0001, //!< 1 x 8-bit value
			ComputeFormat_8X2   = 0x0002, //!< 2 x 8-bit values
			ComputeFormat_8X4   = 0x0003, //!< 4 x 8-bit values
			ComputeFormat_16X1  = 0x0004, //!< 1 x 16-bit value
			ComputeFormat_16X2  = 0x0005, //!< 2 x 16-bit values
			ComputeFormat_16X4  = 0x0006, //!< 4 x 16-bit values
			ComputeFormat_32X1  = 0x0007, //!< 1 x 32-bit value
			ComputeFormat_32X2  = 0x0008, //!< 2 x 32-bit values
			ComputeFormat_32X4  = 0x0009, //!< 4 x 32-bit values
			ComputeFormat_SHIFT = 0x0000,
			ComputeFormat_MASK  = 0x000F,
							    
			ComputeType_Int     = 0x0010, //!< Type `int`.
			ComputeType_Uint    = 0x0020, //!< Type `uint`.
			ComputeType_Float   = 0x0030, //!< Type `float`.
			ComputeType_Shift   = 0x0000,
			ComputeType_Mask    = 0x0030,

			None			    = 0x0000,
			ComputeRead		    = 0x0100, //!< Buffer will be read by shader.
			ComputeWrite	    = 0x0200, //!< Buffer will be used for writing.
			DrawIndirect	    = 0x0400, //!< Buffer will be used for storing draw indirect commands.
			AllowResize		    = 0x0800, //!< Allow dynamic index/vertex buffer resize during update.
			Index32			    = 0x1000, //!< Index buffer contains 32-bit indices.

			ComputeReadWrite    = (0 | ComputeRead | ComputeWrite)

		};

		Buffer& addFlag(Buffer::Enum bFlag) {
			this->bFlag |= bFlag;
			return *this;
		}
		Buffer& removeFlag(Buffer::Enum bFlag) {
			this->bFlag &= ~bFlag;
			return *this;
		}
		Buffer& toggleFlag(Buffer::Enum bFlag) {
			this->bFlag ^= bFlag;
			return *this;
		}

		constexpr bool hasFlag(Buffer::Enum bFlag) const {
			return (toUnderlying(this->bFlag) & toUnderlying(bFlag)) != 0;
		}
		Enum bFlag = Enum::None; // buffer flag
	};
	
}
