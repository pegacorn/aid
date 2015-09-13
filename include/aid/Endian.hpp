// -*- tab-width: 4 -*-
/*!
   @file Endian.hpp

   Copyright 2015 pegacorn

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
#ifndef aid_Endian_hpp
#define aid_Endian_hpp

#include <cstddef>
#include <cstring>
#include <stdexcept>
#include <type_traits>
#include "aid/Config.hpp"


namespace aid {

enum class EndianType: unsigned char
{
	unknown
	, little
	, big
};

namespace Endian_impl {

/*!
  @brief		get native endian type
 */
constexpr EndianType native_type() noexcept
{
#if		AID_ENDIAN == AID_ENDIAN_LITTLE
	return EndianType::little;
#elif	AID_ENDIAN == AID_ENDIAN_BIG
	return EndianType::big;
#else
	return EndianType::unknown;
#endif
}

/*!
  @tparam		t_external_type	external endian type
  @tparam		t_native_type	native endian type
*/
template<EndianType t_external_type, EndianType t_native_type>
struct Converter
{
	template<typename Integer>
	static void to_external(Integer native, unsigned char *external, std::size_t size);

	template<typename Integer>
	static void from_external(const unsigned char *external, std::size_t size, Integer &native);
}; // struct Converter

template<>
struct Converter<EndianType::big, EndianType::little>
{
	/*!
	  @brief		a native(little) endian value to a big endian value
	  @details		0x00FEDCBAu => {0xFE,0xDC,0xBA}
	  @details		0xFFFEDCBA  => {0xFE,0xDC,0xBA}
	  @pre			external != nullptr
	  @pre			0 < size <= sizeof(native)
	  @attention	This function does not check the pre-conditions.
	 */
	template<typename Integer>
	static void to_external(Integer native, unsigned char *external, std::size_t size) {
		const unsigned char * const msbyte = reinterpret_cast<const unsigned char *>(&native) + size - 1;
		for ( std::size_t i{0}; i < size; ++i ) {
			*(external + i) = *(msbyte - i);
		}
	}

	/*!
	  @brief		convert a big endian value to a native(little) endian value
	  @details		{0xFE,0xDC,0xBA} => 0x00FEDCBAu
	  @details		{0xFE,0xDC,0xBA} => 0xFFFEDCBA
	  @pre			external != nullptr
	  @pre			0 < size <= sizeof(native)
	  @attention	This function does not check the pre-conditions.
	 */
	template<typename Integer>
	static void from_external(const unsigned char *external, std::size_t size, Integer &native) {
		native = 0;
		unsigned char * const msbyte = reinterpret_cast<unsigned char *>(&native) + size - 1;
		for ( std::size_t i{0}; i < size; ++i ) {
			*(msbyte - i) = *(external + i);
		}

		if ( std::is_signed<Integer>::value
			 && *reinterpret_cast<const signed char *>(external) < 0 )
		{
			const std::size_t offset{sizeof(native) - size};
			if ( offset > 0 ) {
				// big   : MSByte -> LSByte
				// native: LSByte -> MSByte Sign
				std::memset(msbyte + 1, ~0, offset);
			}
		}
	}
}; // struct Converter<EndianType::big, EndianType::little>

template<>
struct Converter<EndianType::little, EndianType::little>
{
	/*!
	  @brief		a native(little) endian value to a little endian value
	  @details		0x00FEDCBAu => {0xBA,0xDC,0xFE}
	  @details		0xFFFEDCBA  => {0xBA,0xDC,0xFE}
	  @pre			external != nullptr
	  @pre			0 < size <= sizeof(native)
	  @attention	This function does not check the pre-conditions.
	 */
	template<typename Integer>
	static void to_external(Integer native, unsigned char *external, std::size_t size) {
		std::memcpy(external, &native, size);
	}

	/*!
	  @brief		a little endian value to a native(little) endian value
	  @details		{0xBA,0xDC,0xFE} => 0x00FEDCBAu
	  @details		{0xBA,0xDC,0xFE} => 0xFFFEDCBA
	  @pre			external != nullptr
	  @pre			0 < size <= sizeof(native)
	  @attention	This function does not check the pre-conditions.
	 */
	template<typename Integer>
	static void from_external(const unsigned char *external, std::size_t size, Integer &native) {
		native = 0;
		std::memcpy(&native, external, size);

		if ( std::is_signed<Integer>::value
			 && *reinterpret_cast<const signed char *>(external + size - 1) < 0 )
		{
			const std::size_t offset{sizeof(native) - size};
			if ( offset > 0 ) {
				// little: LSByte -> MSByte
				// native: LSByte -> MSByte Sign
				unsigned char * const msbyte = reinterpret_cast<unsigned char *>(&native) + size - 1;
				std::memset(msbyte + 1, ~0, offset);
			}
		}
	}
}; // struct Converter<EndianType::little, EndianType::little>

template<>
struct Converter<EndianType::big, EndianType::big>
{
	/*!
	  @brief		a native(big) endian value to a big endian value
	  @details		0x00FEDCBAu => {0xFE,0xDC,0xBA}
	  @details		0xFFFEDCBA  => {0xFE,0xDC,0xBA}
	  @pre			external != nullptr
	  @pre			0 < size <= sizeof(native)
	  @attention	This function does not check the pre-conditions.
	 */
	template<typename Integer>
	static void to_external(Integer native, unsigned char *external, std::size_t size) {
		const std::size_t offset{sizeof(native) - size};
		const unsigned char * const msbyte = reinterpret_cast<const unsigned char *>(&native) + offset;
		std::memcpy(external, msbyte, size);
	}

	/*!
	  @brief		convert a big endian value to a native(big) endian value
	  @details		{0xFE,0xDC,0xBA} => 0x00FEDCBAu
	  @details		{0xFE,0xDC,0xBA} => 0xFFFEDCBA
	  @pre			external != nullptr
	  @pre			0 < size <= sizeof(native)
	  @attention	This function does not check the pre-conditions.
	 */
	template<typename Integer>
	static void from_external(const unsigned char *external, std::size_t size, Integer &native) {
		native = 0;
		const std::size_t offset{sizeof(native) - size};
		unsigned char * const msbyte = reinterpret_cast<unsigned char *>(&native) + offset;
		std::memcpy(msbyte, external, size);

		if ( std::is_signed<Integer>::value
			 && offset > 0 && *reinterpret_cast<const signed char *>(external) < 0 )
		{
			// big   :      MSByte -> LSByte
			// native: Sign MSByte -> LSByte
			std::memset(&native, ~0, offset);
		}
	}
}; // struct Converter<EndianType::big, EndianType::big>

template<>
struct Converter<EndianType::little, EndianType::big>
{
	/*!
	  @brief		a native(big) endian value to a little endian value
	  @details		0x00FEDCBAu => {0xBA,0xDC,0xFE}
	  @details		0xFFFEDCBA  => {0xBA,0xDC,0xFE}
	  @pre			external != nullptr
	  @pre			0 < size <= sizeof(native)
	  @attention	This function does not check the pre-conditions.
	 */
	template<typename Integer>
	static void to_external(Integer native, unsigned char *external, std::size_t size) {
		const unsigned char * const lsbyte = reinterpret_cast<unsigned char *>(&native) + sizeof(native) - 1;
		for ( std::size_t i{0}; i < size; ++i ) {
			*(external + i) = *(lsbyte - i);
		}
	}

	/*!
	  @brief		a little endian value to a native(big) endian value
	  @details		{0xBA,0xDC,0xFE} => 0x00FEDCBAu
	  @details		{0xBA,0xDC,0xFE} => 0xFFFEDCBA
	  @pre			external != nullptr
	  @pre			0 < size <= sizeof(native)
	  @attention	This function does not check the pre-conditions.
	 */
	template<typename Integer>
	static void from_external(const unsigned char *external, std::size_t size, Integer &native) {
		native = 0;
		unsigned char * const lsbyte = reinterpret_cast<unsigned char *>(&native) + size - 1;
		for ( std::size_t i{0}; i < size; ++i ) {
			*(lsbyte - i) = *(external + i);
		}

		if ( std::is_signed<Integer>::value
			 && *reinterpret_cast<const signed char *>(external + size - 1) < 0 )
		{
			const std::size_t offset{sizeof(native) - size};
			if ( offset > 0 ) { 
				// little:      LSByte -> MSByte
				// native: Sign MSByte -> LSByte
				std::memset(&native, ~0, offset);
			}
		}
	}
}; // struct Converter<EndianType::little, EndianType::big>

} // namespace Endian_impl

/*!
  @tparam		t_external_type	external endian type
*/
template<EndianType t_external_type>
class EndianConverter
{
	using Native = Endian_impl::Converter<t_external_type, Endian_impl::native_type()>;

  public:
	/*!
	  @brief		convert a native endian value to a external endian value
	  @tparam		Integer	integer type
	  @param[in]	native		native endian value
	  @param[out]	external	pointer to the beginning of the external endian value
	  @param[in]	size		size of the external endian value
	  @exception	std::invalid_argument	external == nullptr
	  @exception	std::length_error		size == 0
	  @exception	std::invalid_argument	size > sizeof(native)
	 */
	template<typename Integer>
	static void to_external(Integer native, unsigned char *external, std::size_t size) {
		if ( external == nullptr ) throw std::invalid_argument("external == nullptr");
		if ( size == 0 ) throw std::length_error("size == 0");
		if ( size > sizeof(native) ) throw std::invalid_argument("size > sizeof(native)");

		Native::to_external(native, external, size);
	}

	/*!
	  @copydoc		to_external(Integer,unsigned char *,std::size_t)
	 */
	template<typename Integer>
	static void to_external(Integer native, char *external, std::size_t size) {
		to_external(native, reinterpret_cast<unsigned char *>(external), size);
	}

	/*!
	  @brief		convert a native endian value to a external endian value
	  @tparam		Integer		integer type
	  @param[in]	native		native endian value
	  @param[out]	external	external endian value (byte array)
	  @exception	std::length_error		t_size == 0
	  @exception	std::invalid_argument	t_size > sizeof(native)
	 */
	template<typename Integer, std::size_t t_size>
	static void to_external(Integer native, unsigned char (&external)[t_size]) {
		to_external(native, external, t_size);
	}

	/*!
	  @copydoc		to_external(Integer,unsigned char (&)[])
	 */
	template<typename Integer, std::size_t t_size>
	static void to_external(Integer native, char (&external)[t_size]) {
		to_external(native, external, t_size);
	}

	/*!
	  @brief		convert a external endian value to a native endian value
	  @tparam		Integer		integer type
	  @param[in]	external	pointer to the beginning of the external endian value
	  @param[in]	size		size of the external endian value
	  @param[out]	native		native endian value
	  @exception	std::invalid_argument	external == nullptr
	  @exception	std::length_error		size == 0
	  @exception	std::invalid_argument	size > sizeof(native)
	 */
	template<typename Integer>
	static void from_external(const unsigned char *external, std::size_t size, Integer &native) {
		if ( external == nullptr ) throw std::invalid_argument("external == nullptr");
		if ( size == 0 ) throw std::length_error("size == 0");
		if ( size > sizeof(native) ) throw std::invalid_argument("size > sizeof(native)");

		Native::from_external(external, size, native);
	}

	/*!
	  @copydoc		from_external(const unsigned char *,std::size_t,Integer &)
	 */
	template<typename Integer>
	static void from_external(const char *external, std::size_t size, Integer &native) {
		from_external(reinterpret_cast<const unsigned char *>(external), size, native);
	}

	/*!
	  @brief		convert a external endian value to a native endian value
	  @tparam		Integer		integer type
	  @param[in]	external	external endian value (byte array)
	  @param[out]	native		native endian value
	  @exception	std::length_error		t_size == 0
	  @exception	std::invalid_argument	t_size > sizeof(native)
	 */
	template<std::size_t t_size, typename Integer>
	static void from_external(const unsigned char (&external)[t_size], Integer &native) {
		from_external(external, t_size, native);
	}

	/*!
	  @copydoc		from_external(const unsigned char (&)[],Integer &)
	*/
	template<std::size_t t_size, typename Integer>
	static void from_external(const char (&external)[t_size], Integer &native) {
		from_external(external, t_size, native);
	}
}; // class EndianConverter

#define aid_DEFINE_EndianConverter_external(DModifier, d_external_type, DInteger) \
	DModifier template void EndianConverter<d_external_type>::to_external(DInteger, unsigned char *, std::size_t); \
	DModifier template void EndianConverter<d_external_type>::from_external(const unsigned char *, std::size_t, DInteger &)
#define aid_DEFINE_EndianConverter(DModifier, d_external_type) \
	aid_DEFINE_EndianConverter_external(DModifier, d_external_type, char); \
	aid_DEFINE_EndianConverter_external(DModifier, d_external_type, wchar_t); \
	aid_DEFINE_EndianConverter_external(DModifier, d_external_type, char16_t); \
	aid_DEFINE_EndianConverter_external(DModifier, d_external_type, char32_t); \
	aid_DEFINE_EndianConverter_external(DModifier, d_external_type, unsigned char); \
	aid_DEFINE_EndianConverter_external(DModifier, d_external_type, signed char); \
	aid_DEFINE_EndianConverter_external(DModifier, d_external_type, unsigned short); \
	aid_DEFINE_EndianConverter_external(DModifier, d_external_type, short); \
	aid_DEFINE_EndianConverter_external(DModifier, d_external_type, unsigned int); \
	aid_DEFINE_EndianConverter_external(DModifier, d_external_type, int); \
	aid_DEFINE_EndianConverter_external(DModifier, d_external_type, unsigned long); \
	aid_DEFINE_EndianConverter_external(DModifier, d_external_type, long); \
	aid_DEFINE_EndianConverter_external(DModifier, d_external_type, unsigned long long); \
	aid_DEFINE_EndianConverter_external(DModifier, d_external_type, long long)

aid_DEFINE_EndianConverter(extern, EndianType::little);
aid_DEFINE_EndianConverter(extern, EndianType::big);

//#undef aid_DEFINE_EndianConverter
//#undef aid_DEFINE_EndianConverter_external

} // namespace aid


#endif // aid_Endian_hpp
