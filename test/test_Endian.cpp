// -*- tab-width: 4 -*-
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Endian
#include <boost/mpl/list.hpp>
#include <boost/test/test_case_template.hpp>
#include <boost/test/unit_test.hpp>

#include "aid/Endian.hpp"

#include <climits>
#include <cstring>
#include <initializer_list>
#include <iterator>
#include <stdexcept>
#include <type_traits>

#define CHECK_EQUAL_COLLECTIONS(x, y) BOOST_CHECK_EQUAL_COLLECTIONS(begin(x), end(x), begin(y), end(y))


using namespace std;
using test_type_list = boost::mpl::list<
	char, wchar_t, char16_t, char32_t
	, unsigned char,		signed char
	, unsigned short,		short
	, unsigned int,			int
	, unsigned long,		long
	, unsigned long long,	long long
	>;

template<typename UInteger>
UInteger make_integer_impl(UInteger native, initializer_list<unsigned char> bytes)
{
	decltype(bytes)::const_iterator i = bytes.begin();
	if ( bytes.size() > sizeof(native) ) {
		i += bytes.size() - sizeof(native);
	}
	if ( sizeof(native) > 1 ) {
		for ( ; i != bytes.end(); ++i ) {
			native <<= CHAR_BIT;
			native |= *i;
		}
	}
	else {
		native = *i;
	}
	return native;
}

template<typename Integer>
Integer make_integer(initializer_list<unsigned char> bytes)
{
	using UInteger = typename make_unsigned<Integer>::type;
	UInteger native{0};
	if ( is_signed<Integer>::value ) native = ~native;
	native = make_integer_impl(native, bytes);
	return static_cast<Integer>(native);
}


BOOST_AUTO_TEST_CASE_TEMPLATE(native_to_big_1, test_type, test_type_list)
{
	using converter = aid::EndianConverter<aid::EndianType::big>;

	char big[3];
	if ( sizeof(test_type) >= sizeof(big) ) {
		const auto integer = make_integer<test_type>({0xFE,0xDC,0xBA});
		converter::to_external(integer, big);

		const unsigned char uexpected[]{0xFE,0xDC,0xBA};
		char expected[sizeof(uexpected)];
		memcpy(expected, uexpected, sizeof(expected));
		CHECK_EQUAL_COLLECTIONS(expected, big);
	}
	else {
		constexpr test_type integer{0};
		BOOST_CHECK_THROW(converter::to_external(integer, big), invalid_argument);
	}
}

BOOST_AUTO_TEST_CASE_TEMPLATE(native_to_big_2, test_type, test_type_list)
{
	using converter = aid::EndianConverter<aid::EndianType::big>;

	char big[4];
	if ( sizeof(test_type) >= sizeof(big) ) {
		const auto integer = make_integer<test_type>({0xFE,0xDC,0xBA,0x98});
		converter::to_external(integer, big);

		const unsigned char uexpected[]{0xFE,0xDC,0xBA,0x98};
		char expected[sizeof(uexpected)];
		memcpy(expected, uexpected, sizeof(expected));
		CHECK_EQUAL_COLLECTIONS(expected, big);
	}
	else {
		constexpr test_type integer{0};
		BOOST_CHECK_THROW(converter::to_external(integer, big), invalid_argument);
	}
}

BOOST_AUTO_TEST_CASE_TEMPLATE(native_to_big_e1, test_type, test_type_list)
{
	using converter = aid::EndianConverter<aid::EndianType::big>;

	constexpr test_type integer{0};
	char big[2];
	BOOST_CHECK_THROW(converter::to_external(integer, (char *)nullptr, sizeof(big)), invalid_argument);
	BOOST_CHECK_THROW(converter::to_external(integer, big, 0), length_error);
	if ( sizeof(big) > sizeof(integer) ) {
		BOOST_CHECK_THROW(converter::to_external(integer, big, sizeof(big)), invalid_argument);
	}
	else {
		BOOST_CHECK_NO_THROW(converter::to_external(integer, big, sizeof(big)));
	}
}

BOOST_AUTO_TEST_CASE_TEMPLATE(big_to_native_1, test_type, test_type_list)
{
	using converter = aid::EndianConverter<aid::EndianType::big>;

	const unsigned char ubig[]{0xFE,0xDC,0xBA};
	char big[sizeof(ubig)];
	memcpy(big, ubig, sizeof(big));
	test_type integer;
	if ( sizeof(test_type) >= sizeof(big) ) {
		converter::from_external(big, integer);

		const auto expected = make_integer<test_type>({0xFE,0xDC,0xBA});
		BOOST_CHECK_EQUAL(expected, integer);
	}
	else {
		BOOST_CHECK_THROW(converter::from_external(big, integer), invalid_argument);
	}
}

BOOST_AUTO_TEST_CASE_TEMPLATE(big_to_native_2, test_type, test_type_list)
{
	using converter = aid::EndianConverter<aid::EndianType::big>;

	const unsigned char ubig[]{0xFE,0xDC,0xBA,0x98};
	char big[sizeof(ubig)];
	memcpy(big, ubig, sizeof(big));
	test_type integer;
	if ( sizeof(test_type) >= sizeof(big) ) {
		converter::from_external(big, integer);

		const auto expected = make_integer<test_type>({0xFE,0xDC,0xBA,0x98});
		BOOST_CHECK_EQUAL(expected, integer);
	}
	else {
		BOOST_CHECK_THROW(converter::from_external(big, integer), invalid_argument);
	}
}

BOOST_AUTO_TEST_CASE_TEMPLATE(big_to_native_e1, test_type, test_type_list)
{
	using converter = aid::EndianConverter<aid::EndianType::big>;

	char big[2];
	test_type integer;
	BOOST_CHECK_THROW(converter::from_external((char *)nullptr, sizeof(big), integer), invalid_argument);
	BOOST_CHECK_THROW(converter::from_external(big, 0, integer), length_error);
	if ( sizeof(big) > sizeof(integer) ) {
		BOOST_CHECK_THROW(converter::from_external(big, sizeof(big), integer), invalid_argument);
	}
	else {
		BOOST_CHECK_NO_THROW(converter::from_external(big, sizeof(big), integer));
	}
}

BOOST_AUTO_TEST_CASE_TEMPLATE(native_to_little_1, test_type, test_type_list)
{
	using converter = aid::EndianConverter<aid::EndianType::little>;

	char little[3];
	if ( sizeof(test_type) >= sizeof(little) ) {
		const auto integer = make_integer<test_type>({0xFE,0xDC,0xBA});
		converter::to_external(integer, little);

		const unsigned char uexpected[]{0xBA,0xDC,0xFE};
		char expected[sizeof(uexpected)];
		memcpy(expected, uexpected, sizeof(expected));
		CHECK_EQUAL_COLLECTIONS(expected, little);
	}
	else {
		constexpr test_type integer{0};
		BOOST_CHECK_THROW(converter::to_external(integer, little), invalid_argument);
	}
}

BOOST_AUTO_TEST_CASE_TEMPLATE(native_to_little_2, test_type, test_type_list)
{
	using converter = aid::EndianConverter<aid::EndianType::little>;

	char little[4];
	if ( sizeof(test_type) >= sizeof(little) ) {
		const auto integer = make_integer<test_type>({0xFE,0xDC,0xBA,0x98});
		converter::to_external(integer, little);

		const unsigned char uexpected[]{0x98,0xBA,0xDC,0xFE};
		char expected[sizeof(uexpected)];
		memcpy(expected, uexpected, sizeof(expected));
		CHECK_EQUAL_COLLECTIONS(expected, little);
	}
	else {
		constexpr test_type integer{0};
		BOOST_CHECK_THROW(converter::to_external(integer, little), invalid_argument);
	}
}

BOOST_AUTO_TEST_CASE_TEMPLATE(native_to_little_e1, test_type, test_type_list)
{
	using converter = aid::EndianConverter<aid::EndianType::little>;

	constexpr test_type integer{0};
	char little[2];
	BOOST_CHECK_THROW(converter::to_external(integer, (char *)nullptr, sizeof(little)), invalid_argument);
	BOOST_CHECK_THROW(converter::to_external(integer, little, 0), length_error);
	if ( sizeof(little) > sizeof(integer) ) {
		BOOST_CHECK_THROW(converter::to_external(integer, little, sizeof(little)), invalid_argument);
	}
	else {
		BOOST_CHECK_NO_THROW(converter::to_external(integer, little, sizeof(little)));
	}
}

BOOST_AUTO_TEST_CASE_TEMPLATE(little_to_native_1, test_type, test_type_list)
{
	using converter = aid::EndianConverter<aid::EndianType::little>;

	const unsigned char ulittle[]{0xBA,0xDC,0xFE};
	char little[sizeof(ulittle)];
	memcpy(little, ulittle, sizeof(little));
	test_type integer;
	if ( sizeof(test_type) >= sizeof(little) ) {
		converter::from_external(little, integer);

		const auto expected = make_integer<test_type>({0xFE,0xDC,0xBA});
		BOOST_CHECK_EQUAL(expected, integer);
	}
	else {
		BOOST_CHECK_THROW(converter::from_external(little, integer), invalid_argument);
	}
}

BOOST_AUTO_TEST_CASE_TEMPLATE(little_to_native_2, test_type, test_type_list)
{
	using converter = aid::EndianConverter<aid::EndianType::little>;

	const unsigned char ulittle[]{0x98,0xBA,0xDC,0xFE};
	char little[sizeof(ulittle)];
	memcpy(little, ulittle, sizeof(little));
	test_type integer;
	if ( sizeof(test_type) >= sizeof(little) ) {
		converter::from_external(little, integer);

		const auto expected = make_integer<test_type>({0xFE,0xDC,0xBA,0x98});
		BOOST_CHECK_EQUAL(expected, integer);
	}
	else {
		BOOST_CHECK_THROW(converter::from_external(little, integer), invalid_argument);
	}
}

BOOST_AUTO_TEST_CASE_TEMPLATE(little_to_native_e1, test_type, test_type_list)
{
	using converter = aid::EndianConverter<aid::EndianType::little>;

	char little[2];
	test_type integer;
	BOOST_CHECK_THROW(converter::from_external((char *)nullptr, sizeof(little), integer), invalid_argument);
	BOOST_CHECK_THROW(converter::from_external(little, 0, integer), length_error);
	if ( sizeof(little) > sizeof(integer) ) {
		BOOST_CHECK_THROW(converter::from_external(little, sizeof(little), integer), invalid_argument);
	}
	else {
		BOOST_CHECK_NO_THROW(converter::from_external(little, sizeof(little), integer));
	}
}
