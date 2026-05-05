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

#pragma once

#include "Vector2.hpp"
#include <algorithm>


namespace mce::core {
	////////////////////////////////////////////////////////////
	/// \brief Utility class for manipulating 2D axis aligned rectangles
	///
	////////////////////////////////////////////////////////////
	template <typename T>
	class Rect {
	public:

		////////////////////////////////////////////////////////////
		/// \brief Default constructor
		///
		/// Creates an empty rectangle (it is equivalent to calling
		/// Rect(0, 0, 0, 0)).
		///
		////////////////////////////////////////////////////////////
		Rect();

		////////////////////////////////////////////////////////////
		/// \brief Construct the rectangle from its coordinates
		///
		/// Be careful, the last two parameters are the width
		/// and height, not the right and bottom coordinates!
		///
		/// \param rectLeft   Left coordinate of the rectangle
		/// \param rectTop    Top coordinate of the rectangle
		/// \param rectWidth  Width of the rectangle
		/// \param rectHeight Height of the rectangle
		///
		////////////////////////////////////////////////////////////
		Rect(T rectLeft, T rectTop, T rectWidth, T rectHeight);

		////////////////////////////////////////////////////////////
		/// \brief Construct the rectangle from position and size
		///
		/// Be careful, the last parameter is the size,
		/// not the bottom-right corner!
		///
		/// \param position Position of the top-left corner of the rectangle
		/// \param size     Size of the rectangle
		///
		////////////////////////////////////////////////////////////
		Rect(const sf::Vector2<T>& position, const sf::Vector2<T>& size);

		////////////////////////////////////////////////////////////
		/// \brief Construct the rectangle from another type of rectangle
		///
		/// This constructor doesn't replace the copy constructor,
		/// it's called only when U != T.
		/// A call to this constructor will fail to compile if U
		/// is not convertible to T.
		///
		/// \param rectangle Rectangle to convert
		///
		////////////////////////////////////////////////////////////
		template <typename U>
		explicit Rect(const Rect<U>& rectangle);

		////////////////////////////////////////////////////////////
		/// \brief Check if a point is inside the rectangle's area
		///
		/// This check is non-inclusive. If the point lies on the
		/// edge of the rectangle, this function will return false.
		///
		/// \param x X coordinate of the point to test
		/// \param y Y coordinate of the point to test
		///
		/// \return True if the point is inside, false otherwise
		///
		/// \see intersects
		///
		////////////////////////////////////////////////////////////
		bool contains(T x, T y) const;

		////////////////////////////////////////////////////////////
		/// \brief Check if a point is inside the rectangle's area
		///
		/// This check is non-inclusive. If the point lies on the
		/// edge of the rectangle, this function will return false.
		///
		/// \param point Point to test
		///
		/// \return True if the point is inside, false otherwise
		///
		/// \see intersects
		///
		////////////////////////////////////////////////////////////
		bool contains(const sf::Vector2<T>& point) const;

		////////////////////////////////////////////////////////////
		/// \brief Check the intersection between two rectangles
		///
		/// \param rectangle Rectangle to test
		///
		/// \return True if rectangles overlap, false otherwise
		///
		/// \see contains
		///
		////////////////////////////////////////////////////////////
		bool intersects(const Rect<T>& rectangle) const;

		////////////////////////////////////////////////////////////
		/// \brief Check the intersection between two rectangles
		///
		/// This overload returns the overlapped rectangle in the
		/// \a intersection parameter.
		///
		/// \param rectangle    Rectangle to test
		/// \param intersection Rectangle to be filled with the intersection
		///
		/// \return True if rectangles overlap, false otherwise
		///
		/// \see contains
		///
		////////////////////////////////////////////////////////////
		bool intersects(const Rect<T>& rectangle, Rect<T>& intersection) const;

		////////////////////////////////////////////////////////////
		/// \brief Get the position of the rectangle's top-left corner
		///
		/// \return Position of rectangle
		///
		/// \see getSize
		///
		////////////////////////////////////////////////////////////
		sf::Vector2<T> getPosition() const;

		////////////////////////////////////////////////////////////
		/// \brief Get the size of the rectangle
		///
		/// \return Size of rectangle
		///
		/// \see getPosition
		///
		////////////////////////////////////////////////////////////
		sf::Vector2<T> getSize() const;

		////////////////////////////////////////////////////////////
		// Member data
		////////////////////////////////////////////////////////////
		T left;   //!< Left coordinate of the rectangle
		T top;    //!< Top coordinate of the rectangle
		T width;  //!< Width of the rectangle
		T height; //!< Height of the rectangle
	};

	////////////////////////////////////////////////////////////
	/// \relates Rect
	/// \brief Overload of binary operator ==
	///
	/// This operator compares strict equality between two rectangles.
	///
	/// \param left  Left operand (a rectangle)
	/// \param right Right operand (a rectangle)
	///
	/// \return True if \a left is equal to \a right
	///
	////////////////////////////////////////////////////////////
	template <typename T>
	bool operator ==(const Rect<T>& left, const Rect<T>& right);

	////////////////////////////////////////////////////////////
	/// \relates Rect
	/// \brief Overload of binary operator !=
	///
	/// This operator compares strict difference between two rectangles.
	///
	/// \param left  Left operand (a rectangle)
	/// \param right Right operand (a rectangle)
	///
	/// \return True if \a left is not equal to \a right
	///
	////////////////////////////////////////////////////////////
	template <typename T>
	bool operator!=(const Rect<T>& left, const Rect<T>& right);

	////////////////////////////////////////////////////////////
	template <typename T>
	Rect<T>::Rect() :
		left(0),
		top(0),
		width(0),
		height(0) {

	}


	////////////////////////////////////////////////////////////
	template <typename T>
	Rect<T>::Rect(T rectLeft, T rectTop, T rectWidth, T rectHeight) :
		left(rectLeft),
		top(rectTop),
		width(rectWidth),
		height(rectHeight) {

	}


	////////////////////////////////////////////////////////////
	template <typename T>
	Rect<T>::Rect(const sf::Vector2<T>& position, const sf::Vector2<T>& size) :
		left(position.x),
		top(position.y),
		width(size.x),
		height(size.y) {

	}


	////////////////////////////////////////////////////////////
	template <typename T>
	template <typename U>
	Rect<T>::Rect(const Rect<U>& rectangle) :
		left(static_cast<T>(rectangle.left)),
		top(static_cast<T>(rectangle.top)),
		width(static_cast<T>(rectangle.width)),
		height(static_cast<T>(rectangle.height)) {}


	////////////////////////////////////////////////////////////
	template <typename T>
	bool Rect<T>::contains(T x, T y) const {
		// Rectangles with negative dimensions are allowed, so we must handle them correctly

		// Compute the real min and max of the rectangle on both axes
		T minX = std::min(left, static_cast<T>(left + width));
		T maxX = std::max(left, static_cast<T>(left + width));
		T minY = std::min(top, static_cast<T>(top + height));
		T maxY = std::max(top, static_cast<T>(top + height));

		return (x >= minX) && (x < maxX) && (y >= minY) && (y < maxY);
	}


	////////////////////////////////////////////////////////////
	template <typename T>
	bool Rect<T>::contains(const sf::Vector2<T>& point) const {
		return contains(point.x, point.y);
	}


	////////////////////////////////////////////////////////////
	template <typename T>
	bool Rect<T>::intersects(const Rect<T>& rectangle) const {
		Rect<T> intersection;
		return intersects(rectangle, intersection);
	}


	////////////////////////////////////////////////////////////
	template <typename T>
	bool Rect<T>::intersects(const Rect<T>& rectangle, Rect<T>& intersection) const {
		// Rectangles with negative dimensions are allowed, so we must handle them correctly

		// Compute the min and max of the first rectangle on both axes
		T r1MinX = std::min(left, static_cast<T>(left + width));
		T r1MaxX = std::max(left, static_cast<T>(left + width));
		T r1MinY = std::min(top, static_cast<T>(top + height));
		T r1MaxY = std::max(top, static_cast<T>(top + height));

		// Compute the min and max of the second rectangle on both axes
		T r2MinX = std::min(rectangle.left, static_cast<T>(rectangle.left + rectangle.width));
		T r2MaxX = std::max(rectangle.left, static_cast<T>(rectangle.left + rectangle.width));
		T r2MinY = std::min(rectangle.top, static_cast<T>(rectangle.top + rectangle.height));
		T r2MaxY = std::max(rectangle.top, static_cast<T>(rectangle.top + rectangle.height));

		// Compute the intersection boundaries
		T interLeft = std::max(r1MinX, r2MinX);
		T interTop = std::max(r1MinY, r2MinY);
		T interRight = std::min(r1MaxX, r2MaxX);
		T interBottom = std::min(r1MaxY, r2MaxY);

		// If the intersection is valid (positive non zero area), then there is an intersection
		if ((interLeft < interRight) && (interTop < interBottom)) {
			intersection = Rect<T>(interLeft, interTop, interRight - interLeft, interBottom - interTop);
			return true;
		}
		else {
			intersection = Rect<T>(0, 0, 0, 0);
			return false;
		}
	}

	template <typename T>
	sf::Vector2<T> Rect<T>::getPosition() const {
		return sf::Vector2<T>(left, top);
	}

	template <typename T>
	sf::Vector2<T> Rect<T>::getSize() const {
		return sf::Vector2<T>(width, height);
	}


	////////////////////////////////////////////////////////////
	template <typename T>
	inline bool operator ==(const Rect<T>& left, const Rect<T>& right) {
		return (left.left == right.left) && (left.width == right.width) &&
			(left.top == right.top) && (left.height == right.height);
	}


	////////////////////////////////////////////////////////////
	template <typename T>
	inline bool operator !=(const Rect<T>& left, const Rect<T>& right) {
		return !(left == right);
	}

	// Create typedefs for the most common types
	typedef Rect<int>   IntRect;
	typedef Rect<float> FloatRect;

} // namespace mce::core

////////////////////////////////////////////////////////////
/// \class sf::Rect
/// \ingroup graphics
///
/// A rectangle is defined by its top-left corner and its size.
/// It is a very simple class defined for convenience, so
/// its member variables (left, top, width and height) are public
/// and can be accessed directly, just like the vector classes
/// (Vector2 and Vector3).
///
/// To keep things simple, sf::Rect doesn't define
/// functions to emulate the properties that are not directly
/// members (such as right, bottom, center, etc.), it rather
/// only provides intersection functions.
///
/// sf::Rect uses the usual rules for its boundaries:
/// \li The left and top edges are included in the rectangle's area
/// \li The right (left + width) and bottom (top + height) edges are excluded from the rectangle's area
///
/// This means that sf::IntRect(0, 0, 1, 1) and sf::IntRect(1, 1, 1, 1)
/// don't intersect.
///
/// sf::Rect is a template and may be used with any numeric type, but
/// for simplicity the instantiations used by SFML are typedef'd:
/// \li sf::Rect<int> is sf::IntRect
/// \li sf::Rect<float> is sf::FloatRect
///
/// So that you don't have to care about the template syntax.
///
/// Usage example:
/// \code
/// // Define a rectangle, located at (0, 0) with a size of 20x5
/// sf::IntRect r1(0, 0, 20, 5);
///
/// // Define another rectangle, located at (4, 2) with a size of 18x10
/// sf::Vector2i position(4, 2);
/// sf::Vector2i size(18, 10);
/// sf::IntRect r2(position, size);
///
/// // Test intersections with the point (3, 1)
/// bool b1 = r1.contains(3, 1); // true
/// bool b2 = r2.contains(3, 1); // false
///
/// // Test the intersection between r1 and r2
/// sf::IntRect result;
/// bool b3 = r1.intersects(r2, result); // true
/// // result == (4, 2, 16, 3)
/// \endcode
///
////////////////////////////////////////////////////////////