////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2023 Laurent Gomila (laurent@sfml-dev.org)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "Image.hpp"
#include "Platform.hpp"
#ifdef MCE_PLATFORM_WINDOWS
#pragma warning(disable:4996)
#endif
#define STB_IMAGE_IMPLEMENTATION
#include "libs/stb/stb_image.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "libs/stb/stb_image_write.hpp"
//#ifdef SFML_SYSTEM_ANDROID
//#include <SFML/System/Android/ResourceStream.hpp>
//#endif
#include <algorithm>
#include <cstring>
#include <iterator>

// Convert a string to lower case
std::string toLower(std::string str) {
	for (std::string::iterator i = str.begin(); i != str.end(); ++i)
		*i = static_cast<char>(std::tolower(*i));
	return str;
}



// stb_image callback for constructing a buffer
void bufferFromCallback(void* context, void* data, int size) {
	uint8_t* source = static_cast<uint8_t*>(data);
	std::vector<uint8_t>* dest = static_cast<std::vector<uint8_t>*>(context);
	std::copy(source, source + size, std::back_inserter(*dest));
}

namespace mce::gfx {
	////////////////////////////////////////////////////////////
	Image::Image() :
		m_size(0, 0) {

	}


	////////////////////////////////////////////////////////////
	Image::~Image() {

	}


	////////////////////////////////////////////////////////////
	void Image::create(unsigned int width, unsigned int height, const Color& color) {
		if (width && height) {
			// Create a new pixel buffer first for exception safety's sake
			std::vector<uint8_t> newPixels(width * height * 4);

			// Fill it with the specified color
			uint8_t* ptr = &newPixels[0];
			uint8_t* end = ptr + newPixels.size();
			while (ptr < end) {
				*ptr++ = color.r;
				*ptr++ = color.g;
				*ptr++ = color.b;
				*ptr++ = color.a;
			}

			// Commit the new pixel buffer
			m_pixels.swap(newPixels);

			// Assign the new size
			m_size.x = width;
			m_size.y = height;
		}
		else {
			// Dump the pixel buffer
			std::vector<uint8_t>().swap(m_pixels);

			// Assign the new size
			m_size.x = 0;
			m_size.y = 0;
		}
	}


	////////////////////////////////////////////////////////////
	void Image::create(unsigned int width, unsigned int height, const uint8_t* pixels) {
		if (pixels && width && height) {
			// Create a new pixel buffer first for exception safety's sake
			std::vector<uint8_t> newPixels(pixels, pixels + width * height * 4);

			// Commit the new pixel buffer
			m_pixels.swap(newPixels);

			// Assign the new size
			m_size.x = width;
			m_size.y = height;
		}
		else {
			// Dump the pixel buffer
			std::vector<uint8_t>().swap(m_pixels);

			// Assign the new size
			m_size.x = 0;
			m_size.y = 0;
		}
	}

//
//	////////////////////////////////////////////////////////////
//	bool Image::loadFromFile(const std::string& filename) {
//#ifndef SFML_SYSTEM_ANDROID
//
//		return priv::ImageLoader::getInstance().loadImageFromFile(filename, m_pixels, m_size);
//
//#else
//
//		priv::ResourceStream stream(filename);
//		return loadFromStream(stream);
//
//#endif
//	}

	bool loadImageFromMemory(const void* data, std::size_t dataSize, std::vector<uint8_t>& pixels, sf::Vector2u& size) {
		// Check input parameters
		if (data && dataSize) {
			// Clear the array (just in case)
			pixels.clear();

			// Load the image and get a pointer to the pixels in memory
			int width = 0;
			int height = 0;
			int channels = 0;
			const unsigned char* buffer = static_cast<const unsigned char*>(data);
			unsigned char* ptr = stbi_load_from_memory(buffer, static_cast<int>(dataSize), &width, &height, &channels, STBI_rgb_alpha);

			if (ptr) {
				// Assign the image properties
				size.x = static_cast<unsigned int>(width);
				size.y = static_cast<unsigned int>(height);

				if (width > 0 && height > 0) {
					// Copy the loaded pixels to the pixel buffer
					pixels.resize(static_cast<std::size_t>(width * height * 4));
					memcpy(&pixels[0], ptr, pixels.size());
				}

				// Free the loaded pixels (they are now in our own pixel buffer)
				stbi_image_free(ptr);

				return true;
			}
			else {
				// Error, failed to load the image
				// err() << "Failed to load image from memory. Reason: " << stbi_failure_reason() << std::endl;

				return false;
			}
		}
		else {
			// err() << "Failed to load image from memory, no data provided" << std::endl;
			return false;
		}
	}

	////////////////////////////////////////////////////////////
	bool Image::loadFromMemory(const void* data, std::size_t size) {
		return loadImageFromMemory(data, size, m_pixels, m_size);
	}


	//////////////////////////////////////////////////////////////
	//bool Image::loadFromStream(InputStream& stream) {
	//	return priv::ImageLoader::getInstance().loadImageFromStream(stream, m_pixels, m_size);
	//}


	//////////////////////////////////////////////////////////////
	//bool Image::saveToFile(const std::string& filename) const {
	//	return priv::ImageLoader::getInstance().saveImageToFile(filename, m_pixels, m_size);
	//}

	////////////////////////////////////////////////////////////
	bool saveImageToMemory(const std::string& format, std::vector<uint8_t>& output, const std::vector<uint8_t>& pixels, const sf::Vector2u& size) {
		// Make sure the image is not empty
		if (!pixels.empty() && (size.x > 0) && (size.y > 0)) {
			// Choose function based on format

			std::string specified = toLower(format);
			const sf::Vector2i convertedSize = sf::Vector2i(size);

			if (specified == "bmp") {
				// BMP format
				if (stbi_write_bmp_to_func(&bufferFromCallback, &output, convertedSize.x, convertedSize.y, 4, &pixels[0]))
					return true;
			}
			else if (specified == "tga") {
				// TGA format
				if (stbi_write_tga_to_func(&bufferFromCallback, &output, convertedSize.x, convertedSize.y, 4, &pixels[0]))
					return true;
			}
			else if (specified == "png") {
				// PNG format
				if (stbi_write_png_to_func(&bufferFromCallback, &output, convertedSize.x, convertedSize.y, 4, &pixels[0], 0))
					return true;
			}
			else if (specified == "jpg" || specified == "jpeg") {
				// JPG format
				if (stbi_write_jpg_to_func(&bufferFromCallback, &output, convertedSize.x, convertedSize.y, 4, &pixels[0], 90))
					return true;
			}
		}

		// err() << "Failed to save image with format \"" << format << "\"" << std::endl;
		return false;
	}

	////////////////////////////////////////////////////////////
	bool Image::saveToMemory(std::vector<uint8_t>& output, const std::string& format) const {
		return saveImageToMemory(format, output, m_pixels, m_size);
	}


	////////////////////////////////////////////////////////////
	sf::Vector2u Image::getSize() const {
		return m_size;
	}


	////////////////////////////////////////////////////////////
	void Image::createMaskFromColor(const Color& color, uint8_t alpha) {
		// Make sure that the image is not empty
		if (!m_pixels.empty()) {
			// Replace the alpha of the pixels that match the transparent color
			uint8_t* ptr = &m_pixels[0];
			uint8_t* end = ptr + m_pixels.size();
			while (ptr < end) {
				if ((ptr[0] == color.r) && (ptr[1] == color.g) && (ptr[2] == color.b) && (ptr[3] == color.a))
					ptr[3] = alpha;
				ptr += 4;
			}
		}
	}


	////////////////////////////////////////////////////////////
	void Image::copy(const Image& source, unsigned int destX, unsigned int destY, const core::IntRect& sourceRect, bool applyAlpha) {
		// Make sure that both images are valid
		if ((source.m_size.x == 0) || (source.m_size.y == 0) || (m_size.x == 0) || (m_size.y == 0))
			return;

		// Adjust the source rectangle
		core::IntRect srcRect = sourceRect;
		if (srcRect.width == 0 || (srcRect.height == 0)) {
			srcRect.left = 0;
			srcRect.top = 0;
			srcRect.width = static_cast<int>(source.m_size.x);
			srcRect.height = static_cast<int>(source.m_size.y);
		}
		else {
			if (srcRect.left < 0) srcRect.left = 0;
			if (srcRect.top < 0) srcRect.top = 0;
			if (srcRect.width > static_cast<int>(source.m_size.x)) srcRect.width = static_cast<int>(source.m_size.x);
			if (srcRect.height > static_cast<int>(source.m_size.y)) srcRect.height = static_cast<int>(source.m_size.y);
		}

		// Then find the valid bounds of the destination rectangle
		unsigned int width = static_cast<unsigned int>(srcRect.width);
		unsigned int height = static_cast<unsigned int>(srcRect.height);
		if (destX + width > m_size.x) width = m_size.x - destX;
		if (destY + height > m_size.y) height = m_size.y - destY;

		// Make sure the destination area is valid
		if ((width <= 0) || (height <= 0))
			return;

		// Precompute as much as possible
		std::size_t  pitch = static_cast<std::size_t>(width) * 4;
		unsigned int rows = height;
		int          srcStride = static_cast<int>(source.m_size.x) * 4;
		int          dstStride = static_cast<int>(m_size.x) * 4;
		const uint8_t* srcPixels = &source.m_pixels[0] + (static_cast<unsigned int>(srcRect.left) + static_cast<unsigned int>(srcRect.top) * source.m_size.x) * 4;
		uint8_t* dstPixels = &m_pixels[0] + (destX + destY * m_size.x) * 4;

		// Copy the pixels
		if (applyAlpha) {
			// Interpolation using alpha values, pixel by pixel (slower)
			for (unsigned int i = 0; i < rows; ++i) {
				for (unsigned int j = 0; j < width; ++j) {
					// Get a direct pointer to the components of the current pixel
					const uint8_t* src = srcPixels + j * 4;
					uint8_t* dst = dstPixels + j * 4;

					// Interpolate RGBA components using the alpha values of the destination and source pixels
					uint8_t src_alpha = src[3];
					uint8_t dst_alpha = dst[3];
					uint8_t out_alpha = static_cast<uint8_t>(src_alpha + dst_alpha - src_alpha * dst_alpha / 255);

					dst[3] = out_alpha;

					if (out_alpha)
						for (int k = 0; k < 3; k++)
							dst[k] = static_cast<uint8_t>((src[k] * src_alpha + dst[k] * (out_alpha - src_alpha)) / out_alpha);
					else
						for (int k = 0; k < 3; k++)
							dst[k] = src[k];
				}

				srcPixels += srcStride;
				dstPixels += dstStride;
			}
		}
		else {
			// Optimized copy ignoring alpha values, row by row (faster)
			for (unsigned int i = 0; i < rows; ++i) {
				std::memcpy(dstPixels, srcPixels, pitch);
				srcPixels += srcStride;
				dstPixels += dstStride;
			}
		}
	}


	////////////////////////////////////////////////////////////
	void Image::setPixel(unsigned int x, unsigned int y, const Color& color) {
		uint8_t* pixel = &m_pixels[(x + y * m_size.x) * 4];
		*pixel++ = color.r;
		*pixel++ = color.g;
		*pixel++ = color.b;
		*pixel++ = color.a;
	}


	////////////////////////////////////////////////////////////
	Color Image::getPixel(unsigned int x, unsigned int y) const {
		const uint8_t* pixel = &m_pixels[(x + y * m_size.x) * 4];
		return Color(pixel[0], pixel[1], pixel[2], pixel[3]);
	}


	////////////////////////////////////////////////////////////
	const uint8_t* Image::getPixelsPtr() const {
		if (!m_pixels.empty()) {
			return &m_pixels[0];
		}
		else {
			//err() << "Trying to access the pixels of an empty image" << std::endl;
			return NULL;
		}
	}


	////////////////////////////////////////////////////////////
	void Image::flipHorizontally() {
		if (!m_pixels.empty()) {
			std::size_t rowSize = m_size.x * 4;

			for (std::size_t y = 0; y < m_size.y; ++y) {
				std::vector<uint8_t>::iterator left = m_pixels.begin() + static_cast<std::vector<uint8_t>::iterator::difference_type>(y * rowSize);
				std::vector<uint8_t>::iterator right = m_pixels.begin() + static_cast<std::vector<uint8_t>::iterator::difference_type>((y + 1) * rowSize - 4);

				for (std::size_t x = 0; x < m_size.x / 2; ++x) {
					std::swap_ranges(left, left + 4, right);

					left += 4;
					right -= 4;
				}
			}
		}
	}


	////////////////////////////////////////////////////////////
	void Image::flipVertically() {
		if (!m_pixels.empty()) {
			std::vector<uint8_t>::iterator::difference_type rowSize = static_cast<std::vector<uint8_t>::iterator::difference_type>(m_size.x * 4);

			std::vector<uint8_t>::iterator top = m_pixels.begin();
			std::vector<uint8_t>::iterator bottom = m_pixels.end() - rowSize;

			for (std::size_t y = 0; y < m_size.y / 2; ++y) {
				std::swap_ranges(top, top + rowSize, bottom);

				top += rowSize;
				bottom -= rowSize;
			}
		}
	}

} // namespace sf