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
#include <vector>


namespace aid {

template<typename Identifier, class AbstractProductPtr>
class DefaultFactoryError
{
  public:
	struct Exception: public std::exception
	{
		const char *what() const noexcept { return "Unknown Type"; }
	};

  protected:
	static AbstractProductPtr on_unknown_type(Identifier) {
		throw Exception();
	}

  protected:
	~DefaultFactoryError() = default;
}; // struct DefaultFactoryError


template<
	class AbstractProductPtr
	, typename Identifier
	, typename ProductCreator = AbstractProductPtr (*)()
	, template<typename, class> class FactoryErrorPolicy = DefaultFactoryError
	>
class Factory
	: public FactoryErrorPolicy<Identifier, AbstractProductPtr>
{
  private:
	using IdToProductMap		= std::map<Identifier, ProductCreator>;

  public:
	using abstract_product_ptr_type	= AbstractProductPtr;
	using identifier_type			= Identifier;
	using product_creator_type		= ProductCreator;
	using error_policy_type			= FactoryErrorPolicy<Identifier, AbstractProductPtr>;

  private:
	IdToProductMap	m_associations;

  public:
	bool register_creator(const identifier_type &id, product_creator_type creator) {
		return m_associations.insert(std::make_pair(id, creator)).second;
	}

	bool unregister_creator(const identifier_type &id) {
		return m_associations.erase(id) != 0;
	}

	std::vector<identifier_type> registered_ids() const {
		std::vector<identifier_type> ids;
		ids.reserve(m_associations.size());
		for ( const auto &association : m_associations ) {
			ids.push_back(association.first);
		}
		return ids;
	}

	void clear_creator() {
		m_associations.clear();
	}

	template<typename... Args>
	abstract_product_ptr_type create(const identifier_type &id, Args &&... args) const {
		typename IdToProductMap::const_iterator i = m_associations.find(id);
		if ( i == m_associations.end() ) {
			return this->on_unknown_type(id);
		}
		return (i->second)(std::forward<Args>(args)...);
	}
}; // class Factory

} // namespace aid


#endif // aid_Factory_hpp
