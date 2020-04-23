//
// Created by untouch on 20-4-23.
//

#ifndef UTILS_UTILS_HPP
#define UTILS_UTILS_HPP

namespace polizi {

    template<typename _Tp1, typename _Tp2>
    _Tp1 byte_offset(_Tp2 __ptr, int offset) {
        return reinterpret_cast<_Tp1>(reinterpret_cast<char*>(__ptr) + offset);
    };

}

#endif //UTILS_UTILS_HPP
