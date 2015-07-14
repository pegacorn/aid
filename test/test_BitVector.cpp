// -*- tab-width: 4 -*-
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE BitVector
#include <boost/mpl/list.hpp>
#include <boost/test/test_case_template.hpp>
#include <boost/test/unit_test.hpp>

#include "aid/BitVector.hpp"

#include <climits>
#include <cstdint>
#include <stdexcept>
#include <type_traits>
#include <unordered_set>

using namespace std;
using test_type_list = boost::mpl::list<unsigned char, unsigned short, unsigned int, unsigned long, unsigned long long, uintmax_t>;


BOOST_AUTO_TEST_CASE_TEMPLATE(create_section_1, test_type, test_type_list)
{
	using BVec = aid::BitVector<test_type>;

	auto sec = BVec::create_section(0);
	BOOST_CHECK_EQUAL(0x0001u, sec.mask);
	BOOST_CHECK_EQUAL(0, sec.offset);

	sec = BVec::create_section(0, 1);
	BOOST_CHECK_EQUAL(0x0003u, sec.mask);
	BOOST_CHECK_EQUAL(0, sec.offset);

	sec = BVec::create_section(1);
	BOOST_CHECK_EQUAL(0x0002u, sec.mask);
	BOOST_CHECK_EQUAL(1, sec.offset);

	sec = BVec::create_section(1, 2);
	BOOST_CHECK_EQUAL(0x0006u, sec.mask);
	BOOST_CHECK_EQUAL(1, sec.offset);

	sec = BVec::create_section(1, 3);
	BOOST_CHECK_EQUAL(0x000Eu, sec.mask);
	BOOST_CHECK_EQUAL(1, sec.offset);

	constexpr test_type msb{sizeof(test_type) * CHAR_BIT - 1};
	sec = BVec::create_section(msb);
	BOOST_CHECK_EQUAL(test_type{1u} << msb, sec.mask);
	BOOST_CHECK_EQUAL(msb, sec.offset);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(create_section_e1, test_type, test_type_list)
{
	using BVec = aid::BitVector<test_type>;

	BOOST_CHECK_THROW(BVec::create_section(1, 0), invalid_argument);
	BOOST_CHECK_THROW(BVec::create_section(2, 1), invalid_argument);

	constexpr test_type msb{sizeof(test_type) * CHAR_BIT - 1};
	BOOST_CHECK_NO_THROW(BVec::create_section(0, msb));
	BOOST_CHECK_THROW(BVec::create_section(0, msb + 1), out_of_range);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(default_constructor_1, test_type, test_type_list)
{
	aid::BitVector<test_type> bvec;
	BOOST_CHECK_EQUAL(0, bvec.get());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(constructor_1, test_type, test_type_list)
{
	using BVec = aid::BitVector<test_type>;

	{
		BVec bvec{0};
		BOOST_CHECK_EQUAL(0, bvec.get());
	}
	{
		BVec bvec{1};
		BOOST_CHECK_EQUAL(1, bvec.get());
	}
	{
		constexpr typename make_signed<test_type>::type m1{-1};
		constexpr test_type ff{static_cast<test_type>(m1)};
		BVec bvec{ff};
		BOOST_CHECK_EQUAL(ff, bvec.get());
	}
}

BOOST_AUTO_TEST_CASE_TEMPLATE(set_1, test_type, test_type_list)
{
	using BVec = aid::BitVector<test_type>;
	constexpr test_type msb{sizeof(test_type) * CHAR_BIT - 1};

	{
		{
			constexpr auto sec = BVec::create_section(0);
			BVec bvec;
			bvec.set(sec, 1);
			BOOST_CHECK_EQUAL(1, bvec.get());
		}
		{
			constexpr auto sec = BVec::create_section(1);
			BVec bvec;
			bvec.set(sec, 1);
			BOOST_CHECK_EQUAL(1u << 1, bvec.get());
		}
		{
			constexpr auto sec = BVec::create_section(msb);
			BVec bvec;
			bvec.set(sec, 1);
			BOOST_CHECK_EQUAL(test_type{1u} << msb, bvec.get());
		}
	}
	{
		{
			constexpr auto sec = BVec::create_section(0, 1);
			BVec bvec;
			bvec.set(sec, 3);
			BOOST_CHECK_EQUAL(3, bvec.get());
		}
		{
			constexpr auto sec = BVec::create_section(1, 2);
			BVec bvec;
			bvec.set(sec, 3);
			BOOST_CHECK_EQUAL(test_type{3u} << 1, bvec.get());
		}
		{
			constexpr auto sec = BVec::create_section(msb - 1, msb);
			BVec bvec;
			bvec.set(sec, 3);
			BOOST_CHECK_EQUAL(test_type{3u} << (msb - 1), bvec.get());
		}
	}
	{
		{
			constexpr auto sec = BVec::create_section(0, 1);
			BVec bvec;
			bvec.set(sec, 1);
			BOOST_CHECK_EQUAL(1, bvec.get());
		}
		{
			constexpr auto sec = BVec::create_section(1, 2);
			BVec bvec;
			bvec.set(sec, 2);
			BOOST_CHECK_EQUAL(4, bvec.get());
		}
	}
}

BOOST_AUTO_TEST_CASE_TEMPLATE(set_2, test_type, test_type_list)
{
	using BVec = aid::BitVector<test_type>;
	constexpr test_type msb{sizeof(test_type) * CHAR_BIT - 1};
	constexpr typename make_signed<test_type>::type m1{-1};
	constexpr test_type ff{static_cast<test_type>(m1)};

	{
		{
			constexpr auto sec = BVec::create_section(0, 1);
			BVec bvec{ff};
			bvec.set(sec, 1);
			BOOST_CHECK_EQUAL(1, bvec.get(sec));
		}
		{
			constexpr auto sec = BVec::create_section(1, 2);
			BVec bvec{ff};
			bvec.set(sec, 1);
			BOOST_CHECK_EQUAL(1, bvec.get(sec));
		}
		{
			constexpr auto sec = BVec::create_section(msb - 1, msb);
			BVec bvec{ff};
			bvec.set(sec, 1);
			BOOST_CHECK_EQUAL(1, bvec.get(sec));
		}
	}
	{
		{
			constexpr auto sec = BVec::create_section(0, 1);
			BVec bvec{ff};
			bvec.set(sec, 2);
			BOOST_CHECK_EQUAL(2, bvec.get(sec));
		}
		{
			constexpr auto sec = BVec::create_section(1, 2);
			BVec bvec{ff};
			bvec.set(sec, 2);
			BOOST_CHECK_EQUAL(2, bvec.get(sec));
		}
		{
			constexpr auto sec = BVec::create_section(msb - 1, msb);
			BVec bvec{ff};
			bvec.set(sec, 2);
			BOOST_CHECK_EQUAL(2, bvec.get(sec));
		}
	}
}


BOOST_AUTO_TEST_CASE_TEMPLATE(set_e1, test_type, test_type_list)
{
	using BVec = aid::BitVector<test_type>;
	constexpr test_type msb{sizeof(test_type) * CHAR_BIT - 1};

	{
		constexpr auto sec = BVec::create_section(0);
		BVec bvec;
#ifdef NDEBUG
		bvec.set(sec, 2);
		BOOST_CHECK_EQUAL(0, bvec.get());
#else
		BOOST_CHECK_THROW(bvec.set(sec, 2), overflow_error);
#endif
	}
	{
		constexpr auto sec = BVec::create_section(1);
		BVec bvec;
#ifdef NDEBUG
		bvec.set(sec, 2);
		BOOST_CHECK_EQUAL(0, bvec.get());
#else
		BOOST_CHECK_THROW(bvec.set(sec, 2), overflow_error);
#endif
	}
	{
		constexpr auto sec = BVec::create_section(msb);
		BVec bvec;
#ifdef NDEBUG
		bvec.set(sec, 2);
		BOOST_CHECK_EQUAL(0, bvec.get());
#else
		BOOST_CHECK_THROW(bvec.set(sec, 2), overflow_error);
#endif
	}
}

BOOST_AUTO_TEST_CASE_TEMPLATE(get_1, test_type, test_type_list)
{
	using BVec = aid::BitVector<test_type>;
	constexpr test_type msb{sizeof(test_type) * CHAR_BIT - 1};
	constexpr typename make_signed<test_type>::type m1{-1};
	constexpr test_type ff{static_cast<test_type>(m1)};

	{
		{
			constexpr auto sec = BVec::create_section(0);
			constexpr BVec bvec{ff};
			BOOST_CHECK_EQUAL(1, bvec.get(sec));
		}
		{
			constexpr auto sec = BVec::create_section(1);
			constexpr BVec bvec{ff};
			BOOST_CHECK_EQUAL(1, bvec.get(sec));
		}
		{
			constexpr auto sec = BVec::create_section(msb);
			constexpr BVec bvec{ff};
			BOOST_CHECK_EQUAL(1, bvec.get(sec));
		}
	}
	{
		{
			constexpr auto sec = BVec::create_section(0, 1);
			constexpr BVec bvec{ff};
			BOOST_CHECK_EQUAL(3, bvec.get(sec));
		}
		{
			constexpr auto sec = BVec::create_section(1, 2);
			constexpr BVec bvec{ff};
			BOOST_CHECK_EQUAL(3, bvec.get(sec));
		}
		{
			constexpr auto sec = BVec::create_section(msb - 1, msb);
			constexpr BVec bvec{ff};
			BOOST_CHECK_EQUAL(3, bvec.get(sec));
		}
	}
	{
		{
			constexpr auto sec = BVec::create_section(0, 1);
			constexpr BVec bvec{0x05};
			BOOST_CHECK_EQUAL(1, bvec.get(sec));
		}
		{
			constexpr auto sec = BVec::create_section(1, 2);
			constexpr BVec bvec{0x05};
			BOOST_CHECK_EQUAL(2, bvec.get(sec));
		}
	}
}

BOOST_AUTO_TEST_CASE_TEMPLATE(compare_1, test_type, test_type_list)
{
	using BVec = aid::BitVector<test_type>;

	{
		BOOST_CHECK(  BVec{12} == BVec{12});
		BOOST_CHECK(!(BVec{12} == BVec{13}));
	}
	{
		BOOST_CHECK(!(BVec{12} != BVec{12}));
		BOOST_CHECK(  BVec{12} != BVec{13});
	}
	{
		BOOST_CHECK(!(BVec{12} < BVec{12}));
		BOOST_CHECK(  BVec{12} < BVec{13});
	}
	{
		BOOST_CHECK(!(BVec{12} <= BVec{11}));
		BOOST_CHECK(  BVec{12} <= BVec{12});
		BOOST_CHECK(  BVec{12} <= BVec{13});
	}
	{
		BOOST_CHECK(  BVec{12} > BVec{11});
		BOOST_CHECK(!(BVec{12} > BVec{12}));
	}
	{
		BOOST_CHECK(  BVec{12} >= BVec{11});
		BOOST_CHECK(  BVec{12} >= BVec{12});
		BOOST_CHECK(!(BVec{12} >= BVec{13}));
	}
}

BOOST_AUTO_TEST_CASE_TEMPLATE(hash_1, test_type, test_type_list)
{
	using BVec = aid::BitVector<test_type>;
	using Set = unordered_set<BVec>;

	Set s;
	for ( test_type i = 0; i < 10; ++i ) {
		s.insert(BVec{i});
		BOOST_CHECK_EQUAL(s.size(), i + 1);
	}
}
