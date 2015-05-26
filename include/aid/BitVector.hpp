// -*- tab-width: 4 -*-
/*!
   @file BitVector.hpp

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
#ifndef aid_BitVector_hpp
#define aid_BitVector_hpp

#include <cstdint>
#include <climits>
#include <stdexcept>
#include <type_traits>


namespace aid {

template<typename DataType>
class BitVector
{
	static_assert(std::is_integral<DataType>::value
				  && std::is_unsigned<DataType>::value,
				  "DataType is not unsigned integral type");

  public:
	using data_type		= DataType;
	using offset_type	= std::uint_fast8_t;

	struct Section
	{
		using mask_type	= DataType;

		mask_type			mask;
		std::uint_least8_t	offset;

		constexpr Section(mask_type imask = 0, std::uint_least8_t ioffset = 0)
			: mask{imask}, offset{ioffset}
		{}
	};

  private:
	using mask_type	= typename Section::mask_type;

  private:
	static constexpr mask_type create_mask(offset_type first, offset_type last) {
		return (mask_type{1u} << first)
			| (first < last ? create_mask(first + 1, last) : 0);
	}

  public:
	static constexpr Section create_section(offset_type first, offset_type last) {
		return first > last
			? throw std::invalid_argument("first > last")
			: last >= sizeof(data_type) * CHAR_BIT
			? throw std::out_of_range("last >= sizeof(data_type) * CHAR_BIT")
			: Section(create_mask(first, last), first);
	}

	static constexpr Section create_section(offset_type first) {
		return create_section(first, first);
	}

  private:
	data_type	m_data;

  public:
	constexpr BitVector() noexcept
		: m_data{0}
	{}

	constexpr BitVector(data_type data)
		: m_data{data}
	{}

	constexpr data_type get() const noexcept {
		return m_data;
	}

	constexpr data_type get(const Section &section) const {
		return (m_data & section.mask) >> section.offset;
	}

	void set(const Section &section, data_type value) {
		data_type shifted_value = value << section.offset;
#ifdef NDEBUG
		shifted_value &= section.mask;
#else
		if ( value != (value & (section.mask >> section.offset)) ) {
			throw std::overflow_error("data_type value");
		}
#endif
		m_data &= ~section.mask;
		m_data |= shifted_value;
	}
}; // class BitVector

} // namespace aid


#endif // aid_BitVector_hpp
