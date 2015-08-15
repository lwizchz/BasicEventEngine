/*
* Copyright (c) 2015 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_UTIL_TEMPLATE_H
#define _BEE_UTIL_TEMPLATE_H 1

#include <vector>
#include <algorithm>

// Template requiring functions

// Real number functions
template <typename T, typename... Args>
T choose(Args&&... args) {
        auto t = std::make_tuple(std::forward<Args>(args)...);
        unsigned int size = std::tuple_size<decltype(t)>::value;
        std::vector<T> arguments (size);
        arguments = {std::forward<Args>(args)...};
        return arguments[random(size)];
}

template <typename T, typename... Args>
T min(Args&&... args) {
        auto t = std::make_tuple(std::forward<Args>(args)...);
        unsigned int size = std::tuple_size<decltype(t)>::value;
        std::vector<T> arguments (size);
        arguments = {std::forward<Args>(args)...};
        std::sort(arguments.begin(), arguments.end());
        return arguments[0];
}
template <typename T, typename... Args>
T max(Args&&... args) {
        auto t = std::make_tuple(std::forward<Args>(args)...);
        unsigned int size = std::tuple_size<decltype(t)>::value;
        std::vector<T> arguments (size);
        arguments = {std::forward<Args>(args)...};
        std::sort(arguments.begin(), arguments.end());
        return arguments[size-1];
}
template <typename T, typename... Args>
T mean(Args&&... args) {
        auto t = std::make_tuple(std::forward<Args>(args)...);
        unsigned int size = std::tuple_size<decltype(t)>::value;
        std::vector<T> arguments (size);
        arguments = {std::forward<Args>(args)...};
        T sum = 0;
        for (auto a : arguments) {
                sum += a;
        }
        return sum/size;
}
template <typename T, typename... Args>
T median(Args&&... args) {
        auto t = std::make_tuple(std::forward<Args>(args)...);
        unsigned int size = std::tuple_size<decltype(t)>::value;
        std::vector<T> arguments (size);
        arguments = {std::forward<Args>(args)...};
        std::sort(arguments.begin(), arguments.end());
        if (size % 2 == 1) {
                return arguments[(int)size/2+1];
        } else {
                return (arguments[size/2-1] + arguments[size/2])/2;
        }
}

#endif // _BEE_UTIL_TEMPLATE_H
