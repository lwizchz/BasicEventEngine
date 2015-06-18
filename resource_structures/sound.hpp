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
