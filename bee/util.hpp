/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_UTIL_H
#define _BEE_UTIL_H 1

#include <string> // Include the required library headers
#include <map>
#include <vector>
#include <functional>

#include <glm/glm.hpp> // Include the required headers for vector handling
#include <btBulletDynamicsCommon.h>

#include <SDL2/SDL.h> // Include the required SDL headers
#include <SDL2/SDL_net.h>

#include "debug.hpp"

// General utility functions bee/util.cpp
bool verify_assertions(int, char**);
bool verify_assertions();

// Platform compatibility functions, bee/util/platform.hpp
int bee_get_platform();
std::string bee_get_path();
std::string bee_itos(int);
int bee_stoi(const std::string&);
time_t bee_inc_dst(time_t);
bool bee_has_commandline_input();
int bee_remove(const std::string&);
bool bee_dir_exists(const std::string&);
int bee_mkdir(const char*, mode_t);
std::string bee_mkdtemp(const std::string&);
std::string bee_inet_ntop(const void* src);
int bee_commandline_color(int);
int bee_commandline_color_reset();

// Real number functions, bee/util/real.hpp
#define PI 3.141592653589793238462643383279502884197 // 40 digits of pi, sufficient to calculate the circumference of the observable universe to the width of one atom
unsigned int random_internal(unsigned int, unsigned int, unsigned int);
unsigned int random(int);
unsigned int random_range(unsigned int, unsigned int);
unsigned int random_get_seed();
unsigned int random_set_seed(unsigned int);
unsigned int random_reset_seed();
unsigned int randomize();
template <typename T>
int sign(T);
template <typename T>
T sqr(T);
double logn(double, double);
double degtorad(double);
double radtodeg(double);
double opposite_angle(double);
double absolute_angle(double);
double direction_of(double, double, double, double);
double dist_sqr(double, double, double, double, double, double);
double dist_sqr(double, double, double, double);
double distance(double, double, double, double, double, double);
double distance(double, double, double, double);
double distance(double, double, double);
double distance(double, double);
std::pair<double,double> coord_approach(double, double, double, double, double, double);
glm::vec3 bt_to_glm3(const btVector3&);
btVector3 glm_to_bt3(const glm::vec3&);
template <typename T>
T dot_product(T, T, T, T);
template <typename T>
T dot_product(const std::pair<T,T>&, const std::pair<T,T>&);
template <typename T>
bool is_between(T, T, T);
template <typename T>
bool is_angle_between(T, T, T);
template <typename T>
T fit_bounds(T, T, T);

// String handling functions, bee/util/string.hpp
std::string chr(int);
int ord(char);
int ord(const std::string&);
std::string chra(Uint8*);
Uint8* orda(const std::string&);
std::string string_lower(const std::string&);
std::string string_upper(const std::string&);
std::string string_letters(const std::string&);
std::string string_digits(const std::string&);
std::string string_lettersdigits(const std::string&);
std::map<int,std::string> split(const std::string&, char, bool);
std::map<int,std::string> split(const std::string&, char);
std::map<int,std::string> handle_newlines(const std::string&);
std::string join(const std::map<int,std::string>&, char);
std::string joinv(const std::vector<std::string>&, char);
std::string ltrim(const std::string&);
std::string rtrim(const std::string&);
std::string trim(const std::string&);
bool stringtobool(const std::string&);
std::string booltostring(bool);
std::string string_replace(const std::string&, const std::string&, const std::string&);
std::string string_escape(const std::string&);
std::string string_unescape(const std::string&);
std::string string_repeat(size_t, const std::string&);
std::string string_tabulate(const std::vector<std::vector<std::string>>);
bool clipboard_has_text();
std::string clipboard_get_text();
int clipboard_set_text(const std::string&);

// Date and time functions, bee/util/dates.hpp
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
double stopwatch(std::string, std::function<void()>);

// Collision checking functions, bee/util/collision.hpp
class Line; // Defined below
bool check_collision(const SDL_Rect&, const SDL_Rect&);
bool check_collision_circle(double, double, double, double, double, double);
bool check_collision_line(const Line&, const Line&);
bool check_collision_aligned_line(const Line&, const Line&);
bool check_collision(const SDL_Rect&, const Line&);
double angle_hbounce(double);
double angle_vbounce(double);

// Sound effect functions, bee/util/sound.hpp
class se_chorus_data; // Defined below
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

// Message box functions, bee/util/messagebox.hpp
int show_message(const std::string&, const std::string&, const std::string&, const std::string&);
bool show_question(const std::string&);
int show_message(const std::string&);
int show_warning(const std::string&);
int show_error(const std::string&, bool);
int show_error(const std::string&);

// File handling functions, bee/util/files.hpp
bool file_exists(const std::string&);
int file_delete(const std::string&);
int file_rename(const std::string&, const std::string&);
int file_copy(const std::string&, const std::string&);
std::string file_get_contents(const std::string&);
size_t file_put_contents(const std::string&, const std::string&);
bool directory_exists(const std::string&);
int directory_create(const std::string&);
std::string directory_get_temp();
std::string file_basename(const std::string&);
std::string file_dirname(const std::string&);
std::string file_plainname(const std::string&);
std::string file_extname(const std::string&);

// Networking functions, bee/util/network.hpp
int network_init();
int network_close();
IPaddress* network_resolve_host(const std::string&, int);
std::string network_get_address(Uint32);
int network_get_port(Uint16);
TCPsocket network_tcp_open(const std::string&, int);
int network_tcp_close(TCPsocket*);
TCPsocket network_tcp_accept(TCPsocket*);
IPaddress* network_get_peer_address(TCPsocket*);
int network_tcp_send(TCPsocket*, const void*, int);
int network_tcp_send(TCPsocket*, const std::string&);
int network_tcp_recv(TCPsocket*, void*, int);
std::string network_tcp_recv(TCPsocket*, int);
UDPsocket network_udp_open(int);
int network_udp_close(UDPsocket*);
int network_udp_reopen(UDPsocket*, int);
int network_udp_bind(UDPsocket*, int, IPaddress*);
int network_udp_bind(UDPsocket*, int, const std::string&, int);
int network_udp_bind(UDPsocket*, int, const std::string&);
int network_udp_unbind(UDPsocket*, int);
IPaddress* network_get_peer_address(UDPsocket, int);
int network_udp_send(UDPsocket, int, UDPpacket*);
int network_udp_recv(UDPsocket, UDPpacket*);
int network_udp_send_vector(UDPsocket, UDPpacket**, int);
int network_udp_recv_vector(UDPsocket, UDPpacket**);
int network_udp_send(UDPsocket, int, Uint8*);
int network_udp_send(UDPsocket, int, Uint8, Uint8, Uint8);
UDPpacket* network_packet_alloc(int);
UDPpacket* network_packet_resize(UDPpacket*, int);
int network_packet_free(UDPpacket*);
int network_packet_realloc(UDPpacket*, int);
UDPpacket** network_packet_alloc_vector(int, int);
int network_packet_free_vector(UDPpacket**);

#include "util/template.hpp" // Include functions which require templates

// Define a struct for line data
class Line {
	public:
		double x1, y1, x2, y2;
};

// Define structs for sound effect data
class se_chorus_data {
	public:
		double wet = 0.5;        // Ratio of processed signal from 0.0 to 1.0
		double depth = 0.25;     // Percentage by which delay is modulated from 0.0 to 1.0
		double feedback = 0.0;   // Percentage of output to feed back into the input from 0.0 to 1.0
		double frequency = 10.0; // Frequency of the Low Frequency Oscillator from 0.0 to 20.0
		double delay = 5.0;      // Number of milliseconds to offset the processed signal from the input signal from 0.0 to 20.0

		std::vector<Sint16> stream; // Stores a copy of the processed stream data
		int ticks = 0; // The position of the stream, i.e. the size of the stream data
};
class se_echo_data {
	public:
		double wet = 0.5;      // Ratio of processed signal from 0.0 to 1.0
		double feedback = 0.0; // Percentage of output to feed back into the input from 0.0 to 1.0
		double delay = 300.0;  // Number of milliseconds to offset the processed signal from the input signal from 1.0 to 2000.0

		std::vector<Sint16> stream; // Stores a copy of the processed stream data
		int ticks = 0; // The position of the stream, i.e. the size of the stream data
};
class se_flanger_data {
	public:
		double wet = 0.5;        // Ratio of processed signal from 0.0 to 1.0
		double depth = 0.25;     // Percentage by which delay is modulated from 0.0 to 1.0
		double feedback = 0.0;   // Percentage of output to feed back into the input from 0.0 to 1.0
		double frequency = 10.0; // Frequency of the Low Frequency Oscillator from 1.0 to 20.0
		double delay = 10.0;     // Maximum number of milliseconds to offset the processed signal from the input signal from 1.0 to 10.0

		std::vector<Sint16> stream; // Stores a copy of the processed stream data
		int ticks = 0; // The position of the stream, i.e. the size of the stream data
};
class se_gargle_data {
	public:
		int rate = 1, wavetype = 1;

		std::vector<Sint16> stream; // Stores a copy of the processed stream data
		int ticks = 0; // The position of the stream, i.e. the size of the stream data
};
class se_reverb_data {
	public:
		double gain = 0.0;
		double mix = 0.0;
		double time = 1000.0;
		double ratio = 0.001;

		std::vector<Sint16> stream; // Stores a copy of the processed stream data
		int ticks = 0; // The position of the stream, i.e. the size of the stream data
};
class se_compressor_data {
	public:
		double gain = 0.0;
		double attack = 0.01;
		double threshold = -10.0;
		double ratio = 0.1;
		int release = 50;
		int delay = 0;

		std::vector<Sint16> stream; // Stores a copy of the processed stream data
		int ticks = 0; // The position of the stream, i.e. the size of the stream data
};
class se_equalizer_data {
	public:
		double gain = 0.0;
		int center = 10000;
		int bandwidth = 36;

		std::vector<Sint16> stream; // Stores a copy of the processed stream data
		int ticks = 0; // The position of the stream, i.e. the size of the stream data
};

#endif // _BEE_UTIL_H
