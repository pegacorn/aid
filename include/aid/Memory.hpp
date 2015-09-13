// -*- tab-width: 4 -*-
/*!
  @file Memory.hpp

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
#ifndef aid_Memory_hpp
#define aid_Memory_hpp

#include <cstddef>
#include <memory>
#include <utility>


namespace aid {

template<class Object>
class PrivateAllocator
{
  public:
	using pointer			= Object *;
	using value_type		= Object;

	pointer allocate(std::size_t n, std::allocator<void>::const_pointer = 0) {
		return static_cast<pointer>(::operator new(n * sizeof(Object)));
	}

	void deallocate(pointer p, std::size_t n) {
		::operator delete(static_cast<void *>(p));
	}

	template<class U, class... Args>
	void construct(U *p, Args &&... args) {
		::new(static_cast<void *>(p)) U(std::forward<Args>(args)...);
	}

	template <class U>
	void destroy(U *p) {
		p->~U();
	}
}; // class PrivateAllocator


template<
	class Object
	, class Allocator = std::allocator<Object>
	>
class Creator
{
  public:
	using pointer			= typename Allocator::pointer;
	using value_type		= typename Allocator::value_type;

  private:
	Allocator	m_allocator;

  public:
	pointer create(std::size_t n = 1) {
		pointer obj = m_allocator.allocate(n);
		std::size_t i{0};
		try {
			for ( ; i < n; ++i ) {
				m_allocator.construct(obj + i);
			}
			return obj;
		}
		catch (...) {
			for ( ; i > 0; --i ) {
				m_allocator.destroy(obj + i - 1);
			}
			m_allocator.deallocate(obj, n);
			throw;
		}
	}

	void destroy(pointer obj, std::size_t n = 1) {
		for ( std::size_t i{0}; i < n; ++i ) {
			m_allocator.destroy(obj + i);
		}
		m_allocator.deallocate(obj, n);
	}
}; // class Creator

} // namespace aid


#endif // aid_Memory_hpp
