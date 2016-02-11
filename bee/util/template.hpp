/*
* Copyright (c) 2015-16 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_UTIL_TEMPLATE_H
#define _BEE_UTIL_TEMPLATE_H 1

#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <map>

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

// Networking functions
template <typename A, typename B>
Uint8* network_map_encode(std::map<A,B> m) {
        std::stringstream s ("1");
        for (auto& e : m) {
                s << e.first << " = " << e.second << "\n";
        }
        Uint8* d = orda(s.str());
        d[0] = s.str().length();
        return d;
}
template <typename A, typename B>
int network_map_decode(Uint8* data, std::map<A,B> mv) {
        std::map<A,B> m;
        std::string datastr = chra(data+1);
        if (!datastr.empty()) {
                std::istringstream data_stream (datastr);

                while (!data_stream.eof()) {
                        std::string tmp;
                        getline(data_stream, tmp);

                        if (tmp.empty()) {
                                continue;
                        }

                        std::string v;
                        std::stringstream vs (tmp);
                        getline(vs, v, '=');
                        v = trim(v);

                        std::string d = tmp.substr(tmp.find(" = ")+3);
                        d = trim(d);

                        m.insert(std::make_pair(v, d));
                }

        }
        mv = m;
        return 0;
}

#endif // _BEE_UTIL_TEMPLATE_H
