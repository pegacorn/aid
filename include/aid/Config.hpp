// -*- mode: C++; tab-width: 4 -*-
/*!
   @file Config.hpp

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
#ifndef aid_Config_hpp
#define aid_Config_hpp


// native endian type
#define AID_ENDIAN_UNKNOWN	0
#define AID_ENDIAN_LITTLE	4321
#define AID_ENDIAN_BIG		1234

// TODO: auto detect
#define AID_ENDIAN AID_ENDIAN_LITTLE


#endif // aid_Config_hpp
