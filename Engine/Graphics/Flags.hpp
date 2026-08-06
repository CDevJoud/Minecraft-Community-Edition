#pragma once
#include <cstdint>

namespace dms::gfx::flags {

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

	struct State {
		enum Enum : unsigned long long {

			/**
			* Color RGB/alpha/depth write. When it's not specified write will be disabled.
			*
			*/

			WriteR                        = 0x0000000000000001, //!< Enable R write.
			WriteG                        = 0x0000000000000002, //!< Enable G write.
			WriteB                        = 0x0000000000000004, //!< Enable B write.
			WriteA                        = 0x0000000000000008, //!< Enable alpha write.
			WriteZ                        = 0x0000004000000000, //!< Enable depth write.

			// Enable RGB Write
			WriteRGB					  = (0 | WriteR | WriteG | WriteB),

			// Write all channels mask.
			WriteMask					  = (0 | WriteRGB | WriteA | WriteZ),


			/**
			* Depth test state. When `Depth` is not specified depth test will be disabled.
			*
			*/
			DepthTestLess                 = 0x0000000000000010, //!< Enable depth test, less.
			DepthTestLequal               = 0x0000000000000020, //!< Enable depth test, less or equal.
			DepthTestEqual                = 0x0000000000000030, //!< Enable depth test, equal.
			DepthTestGequal               = 0x0000000000000040, //!< Enable depth test, greater or equal.
			DepthTestGreater              = 0x0000000000000050, //!< Enable depth test, greater.
			DepthTestNotEqual             = 0x0000000000000060, //!< Enable depth test, not equal.
			DepthTestNever                = 0x0000000000000070, //!< Enable depth test, never.
			DepthTestAlways               = 0x0000000000000080, //!< Enable depth test, always.
			DepthTestShift                = 4                 , //!< Depth test state bit shift
			DepthTestMask                 = 0x00000000000000f0, //!< Depth test state bit mask


			/**
 * Use BGFX_STATE_BLEND_FUNC(_src, _dst) or BGFX_STATE_BLEND_FUNC_SEPARATE(_srcRGB, _dstRGB, _srcA, _dstA)
 * helper macros.
 *
 */
			BlendZero                   = 0x0000000000001000, //!< 0, 0, 0, 0
			BlendOne                    = 0x0000000000002000, //!< 1, 1, 1, 1
			BlendSrcColor               = 0x0000000000003000, //!< Rs, Gs, Bs, As
			BlendInvSrcColor            = 0x0000000000004000, //!< 1-Rs, 1-Gs, 1-Bs, 1-As
			BlendSrcAlpha               = 0x0000000000005000, //!< As, As, As, As
			BlendInvSrcAlpha            = 0x0000000000006000, //!< 1-As, 1-As, 1-As, 1-As
			BlendDstAlpha               = 0x0000000000007000, //!< Ad, Ad, Ad, Ad
			BlendInvDstAlpha            = 0x0000000000008000, //!< 1-Ad, 1-Ad, 1-Ad ,1-Ad
			BlendDstColor               = 0x0000000000009000, //!< Rd, Gd, Bd, Ad
			BlendInvDstColor            = 0x000000000000a000, //!< 1-Rd, 1-Gd, 1-Bd, 1-Ad
			BlendSrcAlphaSat            = 0x000000000000b000, //!< f, f, f, 1; f = min(As, 1-Ad)
			BlendFactor                 = 0x000000000000c000, //!< Blend factor
			BlendInvFactor              = 0x000000000000d000, //!< 1-Blend factor
			BlendShift                  = 12                , //!< Blend state bit shift
			BlendMask                   = 0x000000000ffff000, //!< Blend state bit mask


			/**
			 * Use BGFX_STATE_BLEND_EQUATION(_equation) or BGFX_STATE_BLEND_EQUATION_SEPARATE(_equationRGB, _equationA)
			 * helper macros.
			 *
			 */
			BlendEquationAdd             = 0x0000000000000000, //!< Blend add: src + dst.
			BlendEquationSub             = 0x0000000010000000, //!< Blend subtract: src - dst.
			BlendEquationRevSub          = 0x0000000020000000, //!< Blend reverse subtract: dst - src.
			BlendEquationMin             = 0x0000000030000000, //!< Blend min: min(src, dst).
			BlendEquationMax             = 0x0000000040000000, //!< Blend max: max(src, dst).
			BlendEquationShift           = 28				 , //!< Blend equation bit shift
			BlendEquationMask            = 0x00000003f0000000, //!< Blend equation bit mask


			/**
			 * Cull state. When `BGFX_STATE_CULL_*` is not specified culling will be disabled.
			 *
			 */
			CullCW                        = 0x0000001000000000, //!< Cull clockwise triangles.
			CullCCW                       = 0x0000002000000000, //!< Cull counter-clockwise triangles.
			CullShift                     = 36                , //!< Culling mode bit shift
			CullMask                      = 0x0000003000000000, //!< Culling mode bit mask

			/**
			 * Alpha reference value.
			 *
			 */
			AlphaRefShift                 = 40                , //!< Alpha reference bit shift
			AlphaRefMask                  = 0x0000ff0000000000, //!< Alpha reference bit mask
			

			PrimitiveTypeTriStrip         = 0x0001000000000000, //!< Tristrip.
			PrimitiveTypeLines            = 0x0002000000000000, //!< Lines.
			PrimitiveTypeLineStrip        = 0x0003000000000000, //!< Line strip.
			PrimitiveTypePoints           = 0x0004000000000000, //!< Points.
			PrimitiveTypeShift            = 48                , //!< Primitive type bit shift
			PrimitiveTypeMask             = 0x0007000000000000, //!< Primitive type bit mask

			/**
			 * Point size value.
			 *
			 */
			PointSizeShift                = 52                , //!< Point size bit shift
			PointSizeMask                 = 0x00f0000000000000, //!< Point size bit mask


			/**
			* Enable MSAA write when writing into MSAA frame buffer.
			* This flag is ignored when not writing into MSAA frame buffer.
			*
			*/
			MSAA                          = 0x0100000000000000, //!< Enable MSAA rasterization.
			LineAA                        = 0x0200000000000000, //!< Enable line AA rasterization.
			ConservativeRaster            = 0x0400000000000000, //!< Enable conservative rasterization.
			None                          = 0x0000000000000000, //!< No state.
			FrontCCW                      = 0x0000008000000000, //!< Front counter-clockwise (default is clockwise).
			BlendIndependent              = 0x0000000400000000, //!< Enable blend independent.
			BlendAlphaToCoverage          = 0x0000000800000000, //!< Enable alpha to coverage.

			/// Default state is write to RGB, alpha, and depth with depth test less enabled, with clockwise
			/// culling and MSAA (when writing into MSAA frame buffer, otherwise this flag is ignored).
			Default						  = (0 | WriteRGB | WriteA | WriteZ | DepthTestLess | CullCW | MSAA),

			Mask                          = 0xffffffffffffffff  //!< State bit mask
		};
		State& addFlag(State::Enum sFlag) {
			this->sFlag |= sFlag;
			return *this;
		}
		State& removeFlag(State::Enum sFlag) {
			this->sFlag &= ~sFlag;
			return *this;
		}
		State& toggleFlag(State::Enum sFlag) {
			this->sFlag ^= sFlag;
			return *this;
		}

		constexpr bool hasFlag(State::Enum sFlag) const {
			return (toUnderlying(this->sFlag) & toUnderlying(sFlag)) != 0;
		}
		Enum sFlag = Enum::Default; // state flag
	};

}
