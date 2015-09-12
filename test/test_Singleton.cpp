// -*- tab-width: 4 -*-
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Factory
#include <boost/test/test_case_template.hpp>
#include <boost/test/unit_test.hpp>

#include "aid/Factory.hpp"
#include "aid/NonCopyable.hpp"
#include "aid/NonMovable.hpp"
#include "aid/Singleton.hpp"

#include <functional>
#include <memory>
#include <string>
#include <typeinfo>

using namespace std;


struct MyAbstractProduct
{
	virtual ~MyAbstractProduct() = default;
};

using MyAbstractProductPtr = unique_ptr<MyAbstractProduct>;

struct MyProductA: MyAbstractProduct {};

MyAbstractProductPtr create_a() {
	return MyAbstractProductPtr(new MyProductA);
}

struct MyProductB: MyAbstractProduct {};

MyAbstractProductPtr create_b() {
	return MyAbstractProductPtr(new MyProductB);
}

class MyFactory
	: public aid::Factory<MyAbstractProductPtr, std::string>
	, private aid::NonCopyable
	, private aid::NonMovable
{
  public:
	using holder_type = aid::Singleton<MyFactory>;
	friend holder_type::creation_policy_type;
	friend holder_type::lifetime_policy_type;

  private:
	MyFactory() = default;
	~MyFactory() = default;
};
using MyFactoryHolder = MyFactory::holder_type;
extern template MyFactory &MyFactoryHolder::instance();
template MyFactory &MyFactoryHolder::instance();

struct Fixture {
	~Fixture() {
		MyFactoryHolder::instance().clear_creator();
	}
};

BOOST_FIXTURE_TEST_SUITE(normal, Fixture)

BOOST_AUTO_TEST_CASE(instance_1)
{
	{
		auto &factory = MyFactoryHolder::instance();
		const bool registered_a = factory.register_creator("MyProductA", create_a);
		BOOST_CHECK(registered_a);
	}
	{
		auto &factory = MyFactoryHolder::instance();
		const bool registered_a = factory.register_creator("MyProductA", create_a);
		BOOST_CHECK(!registered_a);
	}
	{
		auto &factory = MyFactoryHolder::instance();
		const bool registered_b = factory.register_creator("MyProductB", create_b);
		BOOST_CHECK(registered_b);
	}
	{
		auto &factory = MyFactoryHolder::instance();
		const bool registered_b = factory.register_creator("MyProductB", create_b);
		BOOST_CHECK(!registered_b);
	}

	{
		auto &factory = MyFactoryHolder::instance();
		const bool unregistered_a = factory.unregister_creator("MyProductA");
		BOOST_CHECK(unregistered_a);
	}
	{
		auto &factory = MyFactoryHolder::instance();
		const bool unregistered_a = factory.unregister_creator("MyProductA");
		BOOST_CHECK(!unregistered_a);
	}
	{
		auto &factory = MyFactoryHolder::instance();
		const bool unregistered_b = factory.unregister_creator("MyProductB");
		BOOST_CHECK(unregistered_b);
	}
	{
		auto &factory = MyFactoryHolder::instance();
		const bool unregistered_b = factory.unregister_creator("MyProductB");
		BOOST_CHECK(!unregistered_b);
	}
}

BOOST_AUTO_TEST_SUITE_END()
