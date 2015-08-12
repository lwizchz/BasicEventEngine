/*
* Copyright (c) 2015 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_UTIL_SCRIPT_H
#define _BEE_UTIL_SCRIPT_H 1

#ifdef _BEE_SCRIPTING

// Scripting functions

#include <fstream>
#include <map>
#include <chaiscript/chaiscript.hpp>
#include <chaiscript/chaiscript_stdlib.hpp>

template <typename T>
T execute_string(std::string e) {
        chaiscript::ChaiScript c (chaiscript::Std_Lib::library());
        return c.eval<T>(e);
}
template int execute_string<int>(std::string);
template double execute_string<double>(std::string);
template std::string execute_string<std::string>(std::string);

template <typename T>
T execute_file(std::string fname) {
        chaiscript::ChaiScript c (chaiscript::Std_Lib::library());
        return c.eval_file<T>(fname);
}
template int execute_file<int>(std::string);
template double execute_file<double>(std::string);
template std::string execute_file<std::string>(std::string);

template <typename T>
int save_map(std::string fname, std::map<std::string,T> data) {
        std::ofstream savefile (fname, std::ios::trunc);
        if (savefile.good()) {
                for (auto& v : data) {
                        savefile << trim(v.first) << " = " << v.second << "\n";
                }
                savefile.close();
                return 0;
        }
        return 1;
}
template int save_map<int>(std::string, std::map<std::string,int>);
template int save_map<double>(std::string, std::map<std::string,double>);
template int save_map<std::string>(std::string, std::map<std::string,std::string>);

template <typename T>
std::map<std::string,T> load_map(std::string fname) {
        std::map<std::string,T> data;
        std::string datastr = file_get_contents(fname);
        if (datastr != "") {
                std::istringstream data_stream (datastr);

                while (!data_stream.eof()) {
                        std::string tmp;
                        getline(data_stream, tmp);

                        if (tmp.empty()) {
                                continue;
                        }

                        T d = execute_string<T>("var "+tmp+";");

                        std::string v;
                        std::stringstream vs (tmp);
                        getline(vs, v, '=');
                        v = trim(v);

                        data.insert(std::make_pair(v, d));
                }

        }
        return data;
}
template std::map<std::string,int> load_map<int>(std::string);
template std::map<std::string,double> load_map<double>(std::string);
template std::map<std::string,std::string> load_map<std::string>(std::string);

#endif // _BEE_SCRIPTING

#endif // _BEE_UTIL_SCRIPT_H
