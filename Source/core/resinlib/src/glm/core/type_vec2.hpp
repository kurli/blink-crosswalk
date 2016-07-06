///////////////////////////////////////////////////////////////////////////////////
/// OpenGL Mathematics (glm.g-truc.net)
///
/// Copyright (c) 2005 - 2013 G-Truc Creation (www.g-truc.net)
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
/// 
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
/// 
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
/// THE SOFTWARE.
///
/// @ref core
/// @file glm/core/type_vec2.hpp
/// @date 2008-08-18 / 2013-08-27
/// @author Christophe Riccio
///////////////////////////////////////////////////////////////////////////////////

#ifndef glm_core_type_gentype2
#define glm_core_type_gentype2

#include "../fwd.hpp"
#include "type_vec.hpp"
#include "_swizzle.hpp"

namespace glm{
namespace detail
{
	template <typename T, precision P>
	struct tvec2
	{
		enum ctor{_null};

		typedef tvec2<T, P> type;
		typedef tvec2<bool, P> bool_type;

		GLM_FUNC_DECL GLM_CONSTEXPR int length() const;

		//////////////////////////////////////
		// Data

#		if(GLM_HAS_ANONYMOUS_UNION && defined(GLM_SWIZZLE))
			union
			{
				struct{ T x, y; };
				struct{ T r, g; };
				struct{ T s, t; };

				_GLM_SWIZZLE2_2_MEMBERS(T, P, tvec2, x, y)
				_GLM_SWIZZLE2_2_MEMBERS(T, P, tvec2, r, g)
				_GLM_SWIZZLE2_2_MEMBERS(T, P, tvec2, s, t)
				_GLM_SWIZZLE2_3_MEMBERS(T, P, tvec3, x, y)
				_GLM_SWIZZLE2_3_MEMBERS(T, P, tvec3, r, g)
				_GLM_SWIZZLE2_3_MEMBERS(T, P, tvec3, s, t)
				_GLM_SWIZZLE2_4_MEMBERS(T, P, tvec4, x, y)
				_GLM_SWIZZLE2_4_MEMBERS(T, P, tvec4, r, g)
				_GLM_SWIZZLE2_4_MEMBERS(T, P, tvec4, s, t)
			};
#		else
			union {T x, r, s;};
			union {T y, g, t;};

#			if(defined(GLM_SWIZZLE))
				//GLM_SWIZZLE_GEN_REF_FROM_VEC2(T, P, detail::tvec2, detail::tref2)
				GLM_SWIZZLE_GEN_VEC_FROM_VEC2(T, P, detail::tvec2, detail::tvec2, detail::tvec3, detail::tvec4)
#			endif//(defined(GLM_SWIZZLE))
#		endif//GLM_LANG

		//////////////////////////////////////
		// Accesses

		GLM_FUNC_DECL T & operator[](int i);
		GLM_FUNC_DECL T const & operator[](int i) const;

		//////////////////////////////////////
		// Implicit basic constructors

		GLM_FUNC_DECL tvec2();
		GLM_FUNC_DECL tvec2(tvec2<T, P> const & v);
		template <precision Q>
		GLM_FUNC_DECL tvec2(tvec2<T, Q> const & v);

		//////////////////////////////////////
		// Explicit basic constructors

		GLM_FUNC_DECL explicit tvec2(
			ctor);
		GLM_FUNC_DECL explicit tvec2(
			T const & s);
		GLM_FUNC_DECL explicit tvec2(
			T const & s1,
			T const & s2);

		//////////////////////////////////////
		// Swizzle constructors

		GLM_FUNC_DECL tvec2(tref2<T, P> const & r);

		template <int E0, int E1>
		GLM_FUNC_DECL tvec2(_swizzle<2,T, P, tvec2<T, P>, E0, E1,-1,-2> const & that)
		{
			*this = that();
		}

		//////////////////////////////////////
		// Conversion constructors

		//! Explicit converions (From section 5.4.1 Conversion and scalar constructors of GLSL 1.30.08 specification)
		template <typename U>
		GLM_FUNC_DECL explicit tvec2(
			U const & x);
		//! Explicit converions (From section 5.4.1 Conversion and scalar constructors of GLSL 1.30.08 specification)
		template <typename U, typename V>
		GLM_FUNC_DECL explicit tvec2(
			U const & x,
			V const & y);

		//////////////////////////////////////
		// Conversion vector constructors

		//! Explicit conversions (From section 5.4.1 Conversion and scalar constructors of GLSL 1.30.08 specification)
		template <typename U, precision Q>
		GLM_FUNC_DECL explicit tvec2(tvec2<U, Q> const & v);
		//! Explicit conversions (From section 5.4.1 Conversion and scalar constructors of GLSL 1.30.08 specification)
		template <typename U, precision Q>
		GLM_FUNC_DECL explicit tvec2(tvec3<U, Q> const & v);
		//! Explicit conversions (From section 5.4.1 Conversion and scalar constructors of GLSL 1.30.08 specification)
		template <typename U, precision Q>
		GLM_FUNC_DECL explicit tvec2(tvec4<U, Q> const & v);

		//////////////////////////////////////
		// Unary arithmetic operators

		GLM_FUNC_DECL tvec2<T, P> & operator= (tvec2<T, P> const & v);
		template <typename U> 
		GLM_FUNC_DECL tvec2<T, P> & operator= (tvec2<U, P> const & v);

		template <typename U> 
		GLM_FUNC_DECL tvec2<T, P> & operator+=(U const & s);
		template <typename U> 
		GLM_FUNC_DECL tvec2<T, P> & operator+=(tvec2<U, P> const & v);
		template <typename U> 
		GLM_FUNC_DECL tvec2<T, P> & operator-=(U const & s);
		template <typename U> 
		GLM_FUNC_DECL tvec2<T, P> & operator-=(tvec2<U, P> const & v);
		template <typename U> 
		GLM_FUNC_DECL tvec2<T, P> & operator*=(U const & s);
		template <typename U> 
		GLM_FUNC_DECL tvec2<T, P> & operator*=(tvec2<U, P> const & v);
		template <typename U> 
		GLM_FUNC_DECL tvec2<T, P> & operator/=(U const & s);
		template <typename U> 
		GLM_FUNC_DECL tvec2<T, P> & operator/=(tvec2<U, P> const & v);

		//////////////////////////////////////
		// Increment and decrement operators

		GLM_FUNC_DECL tvec2<T, P> & operator++();
		GLM_FUNC_DECL tvec2<T, P> & operator--();
		GLM_FUNC_DECL tvec2<T, P> operator++(int);
		GLM_FUNC_DECL tvec2<T, P> operator--(int);

		//////////////////////////////////////
		// Unary bit operators

		template <typename U> 
		GLM_FUNC_DECL tvec2<T, P> & operator%= (U const & s);
		template <typename U> 
		GLM_FUNC_DECL tvec2<T, P> & operator%= (tvec2<U, P> const & v);
		template <typename U> 
		GLM_FUNC_DECL tvec2<T, P> & operator&= (U const & s);
		template <typename U> 
		GLM_FUNC_DECL tvec2<T, P> & operator&= (tvec2<U, P> const & v);
		template <typename U> 
		GLM_FUNC_DECL tvec2<T, P> & operator|= (U const & s);
		template <typename U> 
		GLM_FUNC_DECL tvec2<T, P> & operator|= (tvec2<U, P> const & v);
		template <typename U> 
		GLM_FUNC_DECL tvec2<T, P> & operator^= (U const & s);
		template <typename U> 
		GLM_FUNC_DECL tvec2<T, P> & operator^= (tvec2<U, P> const & v);
		template <typename U> 
		GLM_FUNC_DECL tvec2<T, P> & operator<<=(U const & s);
		template <typename U> 
		GLM_FUNC_DECL tvec2<T, P> & operator<<=(tvec2<U, P> const & v);
		template <typename U> 
		GLM_FUNC_DECL tvec2<T, P> & operator>>=(U const & s);
		template <typename U> 
		GLM_FUNC_DECL tvec2<T, P> & operator>>=(tvec2<U, P> const & v);

		//////////////////////////////////////
		// Swizzle operators

		GLM_FUNC_DECL T swizzle(comp X) const;
		GLM_FUNC_DECL tvec2<T, P> swizzle(comp X, comp Y) const;
		GLM_FUNC_DECL tvec3<T, P> swizzle(comp X, comp Y, comp Z) const;
		GLM_FUNC_DECL tvec4<T, P> swizzle(comp X, comp Y, comp Z, comp W) const;
		GLM_FUNC_DECL tref2<T, P> swizzle(comp X, comp Y);
	};

	template <typename T, precision P>
	struct tref2
	{
		GLM_FUNC_DECL tref2(T & x, T & y);
		GLM_FUNC_DECL tref2(tref2<T, P> const & r);
		GLM_FUNC_DECL explicit tref2(tvec2<T, P> const & v);
		GLM_FUNC_DECL tref2<T, P> & operator= (tref2<T, P> const & r);
		GLM_FUNC_DECL tref2<T, P> & operator= (tvec2<T, P> const & v);
		GLM_FUNC_DECL tvec2<T, P> operator() ();

		T & x;
		T & y;
	};

	GLM_DETAIL_IS_VECTOR(tvec2);

	template <typename T, precision P>
	GLM_FUNC_DECL tvec2<T, P> operator+(tvec2<T, P> const & v, T const & s);

	template <typename T, precision P>
	GLM_FUNC_DECL tvec2<T, P> operator+(T const & s, tvec2<T, P> const & v);

	template <typename T, precision P>
	GLM_FUNC_DECL tvec2<T, P> operator+(tvec2<T, P> const & v1, tvec2<T, P> const & v2);

	template <typename T, precision P>
	GLM_FUNC_DECL tvec2<T, P> operator-(tvec2<T, P> const & v, T const & s);

	template <typename T, precision P>
	GLM_FUNC_DECL tvec2<T, P> operator-(T const & s, tvec2<T, P> const & v);

	template <typename T, precision P>
	GLM_FUNC_DECL tvec2<T, P> operator-	(tvec2<T, P> const & v1, tvec2<T, P> const & v2);

	template <typename T, precision P>
	GLM_FUNC_DECL tvec2<T, P> operator*(tvec2<T, P> const & v, T const & s);

	template <typename T, precision P>
	GLM_FUNC_DECL tvec2<T, P> operator*(T const & s, tvec2<T, P> const & v);

	template <typename T, precision P>
	GLM_FUNC_DECL tvec2<T, P> operator*(tvec2<T, P> const & v1, tvec2<T, P> const & v2);

	template <typename T, precision P>
	GLM_FUNC_DECL tvec2<T, P> operator/(tvec2<T, P> const & v, T const & s);

	template <typename T, precision P>
	GLM_FUNC_DECL tvec2<T, P> operator/(T const & s, tvec2<T, P> const & v);

	template <typename T, precision P>
	GLM_FUNC_DECL tvec2<T, P> operator/(tvec2<T, P> const & v1, tvec2<T, P> const & v2);

	template <typename T, precision P>
	GLM_FUNC_DECL tvec2<T, P> operator-(tvec2<T, P> const & v);

	template <typename T, precision P>
	GLM_FUNC_DECL tvec2<T, P> operator%(tvec2<T, P> const & v, T const & s);

	template <typename T, precision P>
	GLM_FUNC_DECL tvec2<T, P> operator%(T const & s, tvec2<T, P> const & v);

	template <typename T, precision P>
	GLM_FUNC_DECL tvec2<T, P> operator%(tvec2<T, P> const & v1, tvec2<T, P> const & v2);

	template <typename T, precision P>
	GLM_FUNC_DECL tvec2<T, P> operator&(tvec2<T, P> const & v, T const & s);

	template <typename T, precision P>
	GLM_FUNC_DECL tvec2<T, P> operator&(T const & s, tvec2<T, P> const & v);

	template <typename T, precision P>
	GLM_FUNC_DECL tvec2<T, P> operator&(tvec2<T, P> const & v1, tvec2<T, P> const & v2);

	template <typename T, precision P>
	GLM_FUNC_DECL tvec2<T, P> operator|(tvec2<T, P> const & v, T const & s);

	template <typename T, precision P>
	GLM_FUNC_DECL tvec2<T, P> operator|(T const & s, tvec2<T, P> const & v);

	template <typename T, precision P>
	GLM_FUNC_DECL tvec2<T, P> operator|(tvec2<T, P> const & v1, tvec2<T, P> const & v2);

	template <typename T, precision P>
	GLM_FUNC_DECL tvec2<T, P> operator^(tvec2<T, P> const & v, T const & s);

	template <typename T, precision P>
	GLM_FUNC_DECL tvec2<T, P> operator^(T const & s, tvec2<T, P> const & v);

	template <typename T, precision P>
	GLM_FUNC_DECL tvec2<T, P> operator^(tvec2<T, P> const & v1, tvec2<T, P> const & v2);

	template <typename T, precision P>
	GLM_FUNC_DECL tvec2<T, P> operator<<(tvec2<T, P> const & v, T const & s);

	template <typename T, precision P>
	GLM_FUNC_DECL tvec2<T, P> operator<<(T const & s, tvec2<T, P> const & v);

	template <typename T, precision P>
	GLM_FUNC_DECL tvec2<T, P> operator<<(tvec2<T, P> const & v1, tvec2<T, P> const & v2);

	template <typename T, precision P>
	GLM_FUNC_DECL tvec2<T, P> operator>>(tvec2<T, P> const & v, T const & s);

	template <typename T, precision P>
	GLM_FUNC_DECL tvec2<T, P> operator>>(T const & s, tvec2<T, P> const & v);

	template <typename T, precision P>
	GLM_FUNC_DECL tvec2<T, P> operator>>(tvec2<T, P> const & v1, tvec2<T, P> const & v2);

	template <typename T, precision P>
	GLM_FUNC_DECL tvec2<T, P> operator~(tvec2<T, P> const & v);

}//namespace detail
}//namespace glm

#ifndef GLM_EXTERNAL_TEMPLATE
#include "type_vec2.inl"
#endif//GLM_EXTERNAL_TEMPLATE

#endif//glm_core_type_gentype2
