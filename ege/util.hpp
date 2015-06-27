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

void calculate_normals(const unsigned int* pIndices, unsigned int IndexCount, Vertex* pVertices, unsigned int VertexCount) {
	// Accumulate each triangle normal into each of the triangle vertices
	for (unsigned int i = 0 ; i < IndexCount ; i += 3) {
		unsigned int Index0 = pIndices[i];
		unsigned int Index1 = pIndices[i + 1];
		unsigned int Index2 = pIndices[i + 2];
		Vector3f v1 = pVertices[Index1].m_pos - pVertices[Index0].m_pos;
		Vector3f v2 = pVertices[Index2].m_pos - pVertices[Index0].m_pos;
		Vector3f Normal = v1.Cross(v2);
		Normal.Normalize();
		
		pVertices[Index0].m_normal += Normal;
		pVertices[Index1].m_normal += Normal;
		pVertices[Index2].m_normal += Normal;
	}
	
	// Normalize all the vertex normals
	for (unsigned int i = 0 ; i < VertexCount ; i++) {
		pVertices[i].m_normal.Normalize();
	}
}
