// -*- tab-width: 4 -*-
/*!
   @file Factory.hpp

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
#ifndef aid_Factory_hpp
#define aid_Factory_hpp

#include <exception>
#include <map>
#include <memory>
#include <utility>


namespace aid {

template<typename IdentifierType, class AbstractProductPtr>
class DefaultFactoryError
{
  public:
	struct Exception: public std::exception
	{
		const char *what() const noexcept { return "Unknown Type"; }
	};

  protected:
	static AbstractProductPtr on_unknown_type(IdentifierType) {
		throw Exception();
	}

  protected:
	~DefaultFactoryError() = default;
}; // struct DefaultFactoryError


template
<
	class AbstractProductPtr,
	typename IdentifierType,
	typename ProductCreator = AbstractProductPtr (*)(),
	template<typename, class>
	class FactoryErrorPolicy = DefaultFactoryError
>
class Factory
	: public FactoryErrorPolicy<IdentifierType, AbstractProductPtr>
{
  private:
	using IdToProductMap	= std::map<IdentifierType, ProductCreator>;

  public:
	using error_policy_type	= FactoryErrorPolicy<IdentifierType, AbstractProductPtr>;

  private:
	IdToProductMap	m_associations;

  public:
	bool register_creator(const IdentifierType &id, ProductCreator creator) {
		return m_associations.insert(std::make_pair(id, creator)).second;
	}

	bool unregister_creator(const IdentifierType &id) {
		return m_associations.erase(id) != 0;
	}

	template<typename... Args>
	AbstractProductPtr create(const IdentifierType &id, Args &&... args) {
		typename IdToProductMap::const_iterator i = m_associations.find(id);
		if ( i == m_associations.end() ) {
			return this->on_unknown_type(id);
		}
		return (i->second)(std::forward<Args>(args)...);
	}
}; // class Factory

} // namespace aid


#endif // aid_Factory_hpp
