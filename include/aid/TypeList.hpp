// -*- tab-width: 4 -*-
/*!
   @file TypeList.hpp

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
#ifndef aid_TypeList_hpp
#define aid_TypeList_hpp


namespace aid {

template<typename... TPack>
struct TypeList;

template<>
struct TypeList<>
{
	static constexpr std::size_t size{0};

  protected:
	~TypeList() = default;
};

template<typename THead, typename... TTail>
struct TypeList<THead, TTail...>: TypeList<TTail...>
{
	using head = THead;
	using tail = TypeList<TTail...>;

	static constexpr std::size_t size{sizeof...(TTail) + 1};

  protected:
	~TypeList() = default;
};

} // namespace aid


#endif // aid_TypeList_hpp
