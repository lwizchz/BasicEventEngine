#include <iostream>
#include <string>
#include <map>

class Resource {};
class ResourceList {
		std::map<int,Resource> resources;
		int next_index;
	public:
		ResourceList();
		int add_resource(Resource);
		Resource get_resource(int);
		int get_amount();
		int remove_resource(int);
};
class MetaResourceList {
	public:
		ResourceList sprites, sounds, backgrounds; //, all;
} resource_list;

ResourceList::ResourceList() {
	next_index = 0;
}
int ResourceList::add_resource(Resource new_resource) {
	int index = next_index;
	std::pair<std::map<int,Resource>::iterator,bool> ret;
	ret = resources.insert(std::pair<int,Resource>(index, new_resource));
	if (ret.second) {
		next_index++;
		return index;
	}
	return -1;
}
Resource ResourceList::get_resource(int id) {
	return resources[id];
}
int ResourceList::get_amount() {
	return resources.size();
}
int ResourceList::remove_resource(int id) {
	resources.erase(id);
	return 0;
}

class Sprite: public Resource {
		// Add new variables to the print() debugging method
		int id;
		std::string name;
		std::string image_path;
		//Texture texture;
		int width, height;
		int subimage_amount;
		int origin_x, origin_y;
	public:
		Sprite();
		Sprite(std::string, std::string);
		~Sprite();
		int reset();
		int print();
		
		int get_id();
		std::string get_name();
		std::string get_path();
		int get_width();
		int get_height();
		int get_subimage_amount();
		int get_origin_x();
		int get_origin_y();
		int set_name(std::string);
		int load_path(std::string);
		int set_origin_x(int);
		int set_origin_y(int);
		int set_origin_xy(int, int);
		int set_origin_center();
};
Sprite::Sprite () {
	id = resource_list.sprites.add_resource(*this);
	if (id < 0) {
		fprintf(stderr, "Failed to add sprite resource: %d", id);
	}
	
	reset();
}
Sprite::Sprite (std::string new_name, std::string path) {
	id = resource_list.sprites.add_resource(*this);
	if (id < 0) {
		fprintf(stderr, "Failed to add sprite resource: %d", id);
	}
	
	reset();
	set_name(new_name);
	load_path(path);
}
Sprite::~Sprite() {
	resource_list.sprites.remove_resource(id);
}
int Sprite::reset() {
	name = "";
	image_path = "";
	//texture = 
	width = 0;
	height = 0;
	subimage_amount = 0;
	origin_x = 0;
	origin_y = 0;
	
	return 0;
}
int Sprite::print() {
	std::cout <<
	"Sprite { "
	"\n	id		" << id <<
	"\n	name		" << name <<
	"\n	image_path	" << image_path <<
	"\n	texture		" << "//" <<
	"\n	width		" << width <<
	"\n	height		" << height <<
	"\n	subimage_amount	" << subimage_amount <<
	"\n	origin_x	" << origin_x <<
	"\n	origin_y	" << origin_y <<
	"\n}\n";
	
	return 0;
}
int Sprite::get_id() {
	return id;
}
std::string Sprite::get_name() {
	return name;
}
std::string Sprite::get_path() {
	return image_path;
}
int Sprite::get_width() {
	return width;
}
int Sprite::get_height() {
	return height;
}
int Sprite::get_subimage_amount() {
	return subimage_amount;
}
int Sprite::get_origin_x() {
	return origin_x;
}
int Sprite::get_origin_y() {
	return origin_y;
}
int Sprite::set_name(std::string new_name) {
	// Deny name change if game is currently running (?)
	// if (!game.is_running()) {
	name = new_name;
	return 0;
}
int Sprite::load_path(std::string path) {
	image_path = path;
	// Load OpenGL texture
	/* texture = 
	 * width = 
	 * height = 
	 * subimage_amount = 
	 */
	return 0;
}
int Sprite::set_origin_x(int new_origin_x) {
	origin_x = new_origin_x;
	return 0;
}
int Sprite::set_origin_y(int new_origin_y) {
	origin_y = new_origin_y;
	return 0;
}
int Sprite::set_origin_xy(int new_origin_x, int new_origin_y) {
	set_origin_x(new_origin_x);
	set_origin_y(new_origin_y);
	return 0;
}
int Sprite::set_origin_center() {
	set_origin_x(width/2);
	set_origin_y(height/2);
	return 0;
}

class Sound: public Resource {
		// Add new variables to the print() debugging method
		int id;
		std::string name;
		std::string sound_path;
		int play_type; // In memory vs continuous
		int channel_amount; // Mono vs stereo
		int sample_rate;
		int sample_format;
		int bit_rate;
	public:
		Sound();
		Sound(std::string, std::string);
		~Sound();
		int reset();
		int print();
		
		int get_id();
		std::string get_name();
		std::string get_path();
		int get_play_type();
		int get_channel_amount();
		int get_sample_rate();
		int get_sample_format();
		int get_bit_rate();
		int set_name(std::string);
		int load_path(std::string);
		int set_play_type(int);
		int set_channel_ammount(int);
		int set_sample_rate(int);
		int set_sample_format(int);
		int set_bit_rate(int);
};
Sound::Sound () {
	id = resource_list.sounds.add_resource(*this);
	if (id < 0) {
		fprintf(stderr, "Failed to add sound resource: %d", id);
	}
	
	reset();
}
Sound::Sound (std::string new_name, std::string path) {
	id = resource_list.sounds.add_resource(*this);
	if (id < 0) {
		fprintf(stderr, "Failed to add sound resource: %d", id);
	}
	
	reset();
	set_name(new_name);
	load_path(path);
}
Sound::~Sound() {
	resource_list.sounds.remove_resource(id);
}
int Sound::reset() {
	name = "";
	sound_path = "";
	play_type = 0;
	channel_amount = 1;
	sample_rate = 44100;
	sample_format = 16;
	bit_rate = 192;
	
	return 0;
}
int Sound::print() {
	std::cout <<
	"Sound { "
	"\n	id		" << id <<
	"\n	name		" << name <<
	"\n	sound_path	" << sound_path <<
	"\n	play_type	" << play_type <<
	"\n	channel_amount	" << channel_amount <<
	"\n	sample_rate	" << sample_rate <<
	"\n	sample_format	" << sample_format <<
	"\n	bit_rate	" << bit_rate <<
	"\n}\n";
	
	return 0;
}
int Sound::get_id() {
	return id;
}
std::string Sound::get_name() {
	return name;
}
std::string Sound::get_path() {
	return sound_path;
}
int Sound::get_play_type() {
	return play_type;
}
int Sound::get_channel_amount() {
	return channel_amount;
}
int Sound::get_sample_rate() {
	return sample_rate;
}
int Sound::get_sample_format() {
	return sample_format;
}
int Sound::get_bit_rate() {
	return bit_rate;
}
int Sound::set_name(std::string new_name) {
	// Deny name change if game is currently running (?)
	// if (!game.is_running()) {
	name = new_name;
	return 0;
}
int Sound::load_path(std::string path) {
	sound_path = path;
	// Load OpenAL sound
	/*
	 * 
	 */
	return 0;
}
int Sound::set_play_type(int new_play_type) {
	play_type = new_play_type;
	return 0;
}
int Sound::set_channel_ammount(int new_channel_amount) {
	channel_amount = new_channel_amount;
	return 0;
}
int Sound::set_sample_rate(int new_sample_rate) {
	sample_rate = new_sample_rate;
	return 0;
}
int Sound::set_sample_format(int new_sample_format) {
	sample_format = new_sample_format;
	return 0;
}
int Sound::set_bit_rate(int new_bit_rate) {
	bit_rate = new_bit_rate;
	return 0;
}

class Background: public Resource {
		// Add new variables to the print() debugging method
		int id;
		std::string name;
		std::string background_path;
		int width, height;
		bool is_tiling;
	public:
		Background();
		Background(std::string, std::string);
		~Background();
		int reset();
		int print();
		
		int get_id();
		std::string get_name();
		std::string get_path();
		int get_width();
		int get_height();
		bool get_is_tiling();
		int set_name(std::string);
		int load_path(std::string);
		int set_is_tiling(bool);
};
Background::Background () {
	id = resource_list.backgrounds.add_resource(*this);
	if (id < 0) {
		fprintf(stderr, "Failed to add background resource: %d", id);
	}
	
	reset();
}
Background::Background (std::string new_name, std::string path) {
	id = resource_list.backgrounds.add_resource(*this);
	if (id < 0) {
		fprintf(stderr, "Failed to add background resource: %d", id);
	}
	
	reset();
	set_name(new_name);
	load_path(path);
}
Background::~Background() {
	resource_list.backgrounds.remove_resource(id);
}
int Background::reset() {
	name = "";
	background_path = "";
	is_tiling = false;
	
	return 0;
}
int Background::print() {
	std::cout <<
	"Background { "
	"\n	id		" << id <<
	"\n	name		" << name <<
	"\n	background_path	" << background_path <<
	"\n	is_tiling	" << is_tiling <<
	"\n}\n";
	
	return 0;
}
int Background::get_id() {
	return id;
}
std::string Background::get_name() {
	return name;
}
std::string Background::get_path() {
	return background_path;
}
int Background::get_width() {
	return width;
}
int Background::get_height() {
	return height;
}
bool Background::get_is_tiling() {
	return is_tiling;
}
int Background::set_name(std::string new_name) {
	// Deny name change if game is currently running (?)
	// if (!game.is_running()) {
	name = new_name;
	return 0;
}
int Background::load_path(std::string path) {
	background_path = path;
	// Load OpenGL texture
	/* texture = 
	 * width = 
	 * height = 
	 */
	return 0;
}
int Background::set_is_tiling(bool new_is_tiling) {
	is_tiling = new_is_tiling;
	return 0;
}
