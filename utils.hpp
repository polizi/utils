//
// Created by polizi on 20-4-23.
//

#ifndef UTILS_UTILS_HPP
#define UTILS_UTILS_HPP

namespace polizi {

    template<typename _Tp0, typename _Tp1>
    _Tp0* byte_offset(_Tp1* __ptr, int offset) {
        return reinterpret_cast<_Tp0*>(reinterpret_cast<char*>(__ptr) + offset);
    };

}

#endif //UTILS_UTILS_HPP
