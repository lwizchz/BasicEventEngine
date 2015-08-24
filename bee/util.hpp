/*
* Copyright (c) 2015 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_UTIL_H
#define _BEE_UTIL_H 1

#include <map>
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>

// General utility
std::pair<int,int> coord_approach(int, int, int, int, int);

// Real number functions
#define PI 3.14159265
unsigned int random_internal(unsigned int, unsigned int, unsigned int);
unsigned int random(int);
unsigned int random_range(unsigned int, unsigned int);
unsigned random_get_seed();
unsigned random_set_seed(unsigned int);
unsigned int random_reset_seed();
unsigned int randomize();
template <typename T>
int sign(T);
template <typename T>
T sqr(T);
double logn(double, double);
double degtorad(double);
double radtodeg(double);
double direction_of(double, double, double, double);
double distance(double, double, double, double);
template <typename T>
T dot_product(T, T, T, T);
template <typename T>
T dot_product(std::pair<T,T>, std::pair<T,T>);

// String handling functions
std::string chr(int);
int ord(char);
int ord(std::string);
std::string chra(Uint8*);
Uint8* orda(std::string);
std::string string_lower(std::string);
std::string string_upper(std::string);
std::string string_letters(std::string);
std::string string_digits(std::string);
std::string string_lettersdigits(std::string);
std::map<int,std::string> handle_newlines(std::string);
std::string ltrim(std::string);
std::string rtrim(std::string);
std::string trim(std::string);
bool clipboard_has_text();
std::string clipboard_get_text();
int clipboard_set_text(std::string);

// Date and time functions
time_t date_date_of(time_t);
time_t date_time_of(time_t);
time_t date_current_datetime();
time_t date_current_date();
time_t date_current_time();
time_t date_create_datetime(int, int, int, int, int, int);
time_t date_create_date(int, int, int);
time_t date_create_time(int, int, int);
time_t date_inc_year(time_t, int);
time_t date_inc_month(time_t, int);
time_t date_inc_week(time_t, int);
time_t date_inc_day(time_t, int);
time_t date_inc_hour(time_t, int);
time_t date_inc_minute(time_t, int);
time_t date_inc_second(time_t, int);
int date_get_year(time_t);
int date_get_month(time_t);
int date_get_week(time_t);
int date_get_day(time_t);
int date_get_hour(time_t);
int date_get_minute(time_t);
int date_get_second(time_t);
int date_get_weekday(time_t);
int date_get_day_of_year(time_t);
int date_get_hour_of_year(time_t);
int date_get_minute_of_year(time_t);
int date_get_second_of_year(time_t);
double date_year_span(time_t, time_t);
double date_month_span(time_t, time_t);
double date_week_span(time_t, time_t);
double date_day_span(time_t, time_t);
double date_hour_span(time_t, time_t);
double date_minute_span(time_t, time_t);
double date_second_span(time_t, time_t);
int date_compare_datetime(time_t, time_t);
int date_compare_date(time_t, time_t);
int date_compare_time(time_t, time_t);
std::string date_datetime_string(time_t);
std::string date_date_string(time_t);
std::string date_time_string(time_t);
bool date_is_leap_year(time_t);
bool date_is_leap_year();
bool date_is_today(time_t);
int date_days_in_month(time_t);
int date_days_in_year(time_t);

// Collision checking functions
class Line;
bool check_collision(SDL_Rect*, SDL_Rect*);
bool check_collision_circle(double, double, double, double, double, double);
bool check_collision_line(Line, Line);
double angle_hbounce(double);
double angle_vbounce(double);

// Sound effect functions
class se_chorus_data;
void sound_effect_chorus(int, void*, int, void*);
void sound_effect_chorus_cleanup(int, void*);
class se_echo_data;
void sound_effect_echo(int, void*, int, void*);
void sound_effect_echo_cleanup(int, void*);
class se_flanger_data;
void sound_effect_flanger(int, void*, int, void*);
void sound_effect_flanger_cleanup(int, void*);
class se_gargle_data;
void sound_effect_gargle(int, void*, int, void*);
void sound_effect_gargle_cleanup(int, void*);
class se_reverb_data;
void sound_effect_reverb(int, void*, int, void*);
void sound_effect_reverb_cleanup(int, void*);
class se_compressor_data;
void sound_effect_compressor(int, void*, int, void*);
void sound_effect_compressor_cleanup(int, void*);
class se_equalizer_data;
void sound_effect_equalizer(int, void*, int, void*);
void sound_effect_equalizer_cleanup(int, void*);

// Message box functions
int show_message(std::string);
int show_warning(std::string);
int show_error(std::string, bool);
int show_error(std::string);
int show_message(std::string, std::string, std::string, std::string);
bool show_question(std::string);

// File handling functions
bool file_exists(std::string);
int file_delete(std::string);
int file_rename(std::string, std::string);
int file_copy(std::string, std::string);
std::string file_get_contents(std::string);
bool directory_exists(std::string);
int directory_create(std::string);
std::string directory_get_temp();

#ifdef _BEE_SCRIPTING

// Scripting functions
template <typename T>
T execute_string(std::string);
template <typename T>
T execute_file(std::string);
template <typename T>
int save_map(std::string, std::map<std::string,T>);
template <typename T>
std::map<std::string,T> load_map(std::string);

#endif // _BEE_SCRIPTING

// Networking functions
int network_init();
int network_quit();
IPaddress* network_resolve_host(std::string, int);
std::string network_get_address(Uint32);
TCPsocket network_tcp_open(std::string, int);
int network_tcp_close(TCPsocket);
TCPsocket network_tcp_accept(TCPsocket);
IPaddress* network_get_peer_address(TCPsocket);
int network_tcp_send(TCPsocket, const void*, int);
int network_tcp_send(TCPsocket, std::string);
int network_tcp_recv(TCPsocket, void*, int);
std::string network_tcp_recv(TCPsocket, int);
UDPsocket network_udp_open(int);
int network_udp_close(UDPsocket);
int network_udp_bind(UDPsocket, int, IPaddress*);
int network_udp_bind(UDPsocket, int, std::string, int);
int network_udp_unbind(UDPsocket, int);
IPaddress* network_get_peer_address(UDPsocket, int);
int network_udp_send(UDPsocket, int, UDPpacket*);
int network_udp_recv(UDPsocket, UDPpacket*);
int network_udp_sendv(UDPsocket, UDPpacket**, int);
int network_udp_recvv(UDPsocket, UDPpacket**);
int network_udp_send(UDPsocket, int, Uint8*);
int network_udp_send(UDPsocket, int, Uint8, Uint8, Uint8);
UDPpacket* network_packet_alloc(int);
UDPpacket* network_packet_resize(UDPpacket*, int);
int network_packet_free(UDPpacket*);
int network_packet_realloc(UDPpacket*, int);
UDPpacket** network_packet_allocv(int, int);
int network_packet_freev(UDPpacket**);

#include "util/template.hpp"

#endif // _BEE_UTIL_H
