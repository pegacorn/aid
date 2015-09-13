// -*- tab-width: 4 -*-
/*!
  @file Singleton.hpp

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
#ifndef aid_Singleton_hpp
#define aid_Singleton_hpp

#include <cassert>
#include <cstdlib>
#include <mutex>
#include <stdexcept>
#include "aid/Memory.hpp"


namespace aid {

using AtExitFunction	= void (*)();

template<class Object>
struct DefaultLifetime
{
	static void schedule_destruction(Object *, AtExitFunction function) {
		std::atexit(function);
	}

	static void on_dead_reference() {
		throw std::logic_error("Dead Reference Detected");
	}
}; // struct DefaultLifetime


template<
	typename Object
	, class Allocator = PrivateAllocator<Object>
	, template<class> class LifetimePolicy = DefaultLifetime
	>
class Singleton
{
  private:
	using object_ptr_type		= Object * volatile;
	using creator_type			= Creator<Object, Allocator>;

  public:
	using object_type			= Object;
	using allocator_type		= Allocator;
	using lifetime_policy_type	= LifetimePolicy<Object>;

  private:
	static creator_type			s_creator;
	static std::mutex			s_mutex;
	static object_ptr_type		s_instance;
	static bool					s_destroyed;

  private:
	static void make_instance() {
		std::lock_guard<decltype(s_mutex)> guard(s_mutex);
		if ( s_instance ) {
			return;
		}

		if ( s_destroyed ) {
			s_destroyed = false;
			lifetime_policy_type::on_dead_reference();
		}

		s_instance = s_creator.create();
		lifetime_policy_type::schedule_destruction(
		  s_instance, &destroy_singleton);
	}

	static void destroy_singleton() {
		assert(!s_destroyed);
		s_creator.destroy(s_instance);
		s_instance = nullptr;
		s_destroyed = true;
	}

  public:
	static object_type &instance() {
		if ( !s_instance ) {
			make_instance();
		}
		return *s_instance;
	}

  private:
	Singleton() = delete;
}; // class Singleton

template<
	typename Object
	, class Allocator
	, template<class> class LifetimePolicy
	>
typename Singleton<Object, Allocator, LifetimePolicy>::creator_type
Singleton<Object, Allocator, LifetimePolicy>::s_creator;

template<
	typename Object
	, class Allocator
	, template<class> class LifetimePolicy
	>
std::mutex Singleton<Object, Allocator, LifetimePolicy>::s_mutex;

template<
	typename Object
	, class Allocator
	, template<class> class LifetimePolicy
	>
typename Singleton<Object, Allocator, LifetimePolicy>::object_ptr_type
Singleton<Object, Allocator, LifetimePolicy>::s_instance = nullptr;
	
template<
	typename Object
	, class Allocator
	, template<class> class LifetimePolicy
	>
bool Singleton<Object, Allocator, LifetimePolicy>::s_destroyed = false;

} // namespace aid


#endif // aid_Singleton_hpp
