// -*- tab-width: 4 -*-
/*!
   @file DynamicEndianConverter.hpp

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
#ifndef aid_DynamicEndianConverter_hpp
#define aid_DynamicEndianConverter_hpp

#include "aid/Endian.hpp"


namespace aid {

class DynamicEndianConverter
{
	template<EndianType t_external_type>
	using Native = Endian_impl::Converter<t_external_type, Endian_impl::native_type()>;

  private:
	EndianType	m_external_type;

  public:
	DynamicEndianConverter() noexcept
		: m_external_type{EndianType::unknown}
	{}

	/*!
	  @brief		construct the object and set the endian type of a external data
	  @param[in]	external_type	external endian type
	 */
	explicit DynamicEndianConverter(EndianType external_type) {
		set_external_type(external_type);
	}

  public:
	/*!
	  @brief		set the endian type of a external data
	  @param[in]	external_type	external endian type
	 */
	void set_external_type(EndianType external_type) {
		m_external_type = external_type;
	}

	/*!
	  @brief		get the endian type of a external data
	  @return		external endian type
	 */
	EndianType get_external_type() const noexcept {
		return m_external_type;
	}

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
	void to_external(Integer native, unsigned char *external, std::size_t size) const {
		if ( external == nullptr ) throw std::invalid_argument("external == nullptr");
		if ( size == 0 ) throw std::length_error("size == 0");
		if ( size > sizeof(native) ) throw std::invalid_argument("size > sizeof(native)");

		switch ( m_external_type ) {
		case EndianType::little:
			return Native<EndianType::little>::to_external(native, external, size);
		case EndianType::big:
			return Native<EndianType::big>::to_external(native, external, size);
		default:
			throw std::logic_error("m_external_type is unknown");
		}
	}

	/*!
	  @copydoc		to_external(Integer,unsigned char *,std::size_t)
	 */
	template<typename Integer>
	void to_external(Integer native, char *external, std::size_t size) const {
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
	void to_external(Integer native, unsigned char (&external)[t_size]) const {
		to_external(native, external, t_size);
	}

	/*!
	  @copydoc		to_external(Integer,unsigned char (&)[])
	 */
	template<typename Integer, std::size_t t_size>
	void to_external(Integer native, char (&external)[t_size]) const {
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
	void from_external(const unsigned char *external, std::size_t size, Integer &native) const {
		if ( external == nullptr ) throw std::invalid_argument("external == nullptr");
		if ( size == 0 ) throw std::length_error("size == 0");
		if ( size > sizeof(native) ) throw std::invalid_argument("size > sizeof(native)");

		switch ( m_external_type ) {
		case EndianType::little:
			return Native<EndianType::little>::from_external(external, size, native);
		case EndianType::big:
			return Native<EndianType::big>::from_external(external, size, native);
		default:
			throw std::logic_error("m_external_type is unknown");
		}
	}

	/*!
	  @copydoc		from_external(const unsigned char *,std::size_t,Integer &)
	 */
	template<typename Integer>
	void from_external(const char *external, std::size_t size, Integer &native) const {
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
	void from_external(const unsigned char (&external)[t_size], Integer &native) const {
		from_external(external, t_size, native);
	}

	/*!
	  @copydoc		from_external(const unsigned char (&)[],Integer &)
	*/
	template<std::size_t t_size, typename Integer>
	void from_external(const char (&external)[t_size], Integer &native) const {
		from_external(external, t_size, native);
	}
}; // class DynamicEndianConverter

#define aid_DEFINE_DynamicEndianConverter_external(DModifier, DInteger) \
	DModifier template void DynamicEndianConverter::to_external(DInteger, unsigned char *, std::size_t) const; \
	DModifier template void DynamicEndianConverter::from_external(const unsigned char *, std::size_t, DInteger &) const
#define aid_DEFINE_DynamicEndianConverter(DModifier) \
	aid_DEFINE_DynamicEndianConverter_external(DModifier, char); \
	aid_DEFINE_DynamicEndianConverter_external(DModifier, wchar_t); \
	aid_DEFINE_DynamicEndianConverter_external(DModifier, char16_t); \
	aid_DEFINE_DynamicEndianConverter_external(DModifier, char32_t); \
	aid_DEFINE_DynamicEndianConverter_external(DModifier, unsigned char); \
	aid_DEFINE_DynamicEndianConverter_external(DModifier, signed char); \
	aid_DEFINE_DynamicEndianConverter_external(DModifier, unsigned short); \
	aid_DEFINE_DynamicEndianConverter_external(DModifier, short); \
	aid_DEFINE_DynamicEndianConverter_external(DModifier, unsigned int); \
	aid_DEFINE_DynamicEndianConverter_external(DModifier, int); \
	aid_DEFINE_DynamicEndianConverter_external(DModifier, unsigned long); \
	aid_DEFINE_DynamicEndianConverter_external(DModifier, long); \
	aid_DEFINE_DynamicEndianConverter_external(DModifier, unsigned long long); \
	aid_DEFINE_DynamicEndianConverter_external(DModifier, long long)

aid_DEFINE_DynamicEndianConverter(extern);

//#undef aid_DEFINE_DynamicEndianConverter
//#undef aid_DEFINE_DynamicEndianConverter_external

} // namespace aid


#endif // aid_DynamicEndianConverter_hpp
