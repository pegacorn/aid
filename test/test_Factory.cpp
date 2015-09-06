// -*- tab-width: 4 -*-
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Factory
#include <boost/test/test_case_template.hpp>
#include <boost/test/unit_test.hpp>

#include "aid/Factory.hpp"

#include <functional>
#include <memory>
#include <string>
#include <typeinfo>

using namespace std;


struct MyAbstractProduct
{
	int i;
	double d;
	MyAbstractProduct(int i, double d): i{i}, d{d} {}
	virtual ~MyAbstractProduct() = default;
};

using MyAbstractProductPtr = unique_ptr<MyAbstractProduct>;

struct MyProductA: MyAbstractProduct
{
	MyProductA(int i, double d): MyAbstractProduct(i, d) {}
};

MyAbstractProductPtr create_a(int i, double d) {
	return MyAbstractProductPtr(new MyProductA(i, d));
}

struct MyProductB: MyAbstractProduct
{
	MyProductB(int i, double d): MyAbstractProduct(i, d) {}
};

using MyProductCreator = function<MyAbstractProductPtr(int, double)>;
using MyFactory = aid::Factory<MyAbstractProductPtr, std::string, MyProductCreator>;

BOOST_AUTO_TEST_CASE(register_1)
{
	MyFactory factory;

	bool registered_a = factory.register_creator("MyProductA", create_a);
	BOOST_CHECK(registered_a);

	registered_a = factory.register_creator("MyProductA", create_a);
	BOOST_CHECK(!registered_a);

	bool registered_b = factory.register_creator("MyProductB",
		[](int i, double d) {
			 return MyAbstractProductPtr(new MyProductB(i, d));
		});
	BOOST_CHECK(registered_b);

	registered_b = factory.register_creator("MyProductB",
		[](int i, double d) {
			 return MyAbstractProductPtr(new MyProductB(i, d));
		});
	BOOST_CHECK(!registered_b);


	bool unregistered_a = factory.unregister_creator("MyProductA");
	BOOST_CHECK(unregistered_a);

	unregistered_a = factory.unregister_creator("MyProductA");
	BOOST_CHECK(!unregistered_a);

	bool unregistered_b = factory.unregister_creator("MyProductB");
	BOOST_CHECK(unregistered_b);

	unregistered_b = factory.unregister_creator("MyProductB");
	BOOST_CHECK(!unregistered_b);
}

BOOST_AUTO_TEST_CASE(create_1)
{
	MyFactory factory;
	factory.register_creator("MyProductA", create_a);
	factory.register_creator("MyProductB",
		[](int i, double d) {
			 return MyAbstractProductPtr(new MyProductB(i, d));
		});

	int i = 2;
	double d = 3.4;
	auto product_a = factory.create("MyProductA", i, d);
	BOOST_CHECK(product_a);

	auto product_b = factory.create("MyProductB", 3, 4.5);
	BOOST_CHECK(product_b);
}

BOOST_AUTO_TEST_CASE(create_e1)
{
	MyFactory factory;
	factory.register_creator("MyProductA", create_a);

	BOOST_CHECK_NO_THROW(factory.create("MyProductA", 2, 3.4));

	BOOST_CHECK_THROW(factory.create("MyProductB", 3, 4.5),
					  MyFactory::error_policy_type::Exception);
}
