#include <string>
#include <fstream>

bool read_file(const char* path, std::string& output) {
	std::ifstream t(path);
	t.seekg(0, std::ios::end);
	size_t size = t.tellg();
	output.reserve(size);
	t.seekg(0);
	t.read(&output[0], size);
	return 0;
}

struct Vertex {
	Vector3f m_pos;
	Vector2f m_tex;
	
	Vertex() {}
	
	Vertex(Vector3f pos, Vector2f tex) {
		m_pos = pos;
		m_tex = tex;
	}
};
