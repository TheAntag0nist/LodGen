#include <mesh.h>

namespace lod_generator {
	mesh::mesh(){}
	mesh::~mesh(){}

	void mesh::set_vertexes(std::vector<double> vertexes_in) {
		m_vertexes = vertexes_in;
	}

	void mesh::set_vertexes(double* vert_arr, uint32_t size){
		for(int i=0;i<size;i++){
			m_vertexes.push_back(vert_arr[i]);
		}
	}

	void mesh::set_indexes(std::vector<uint32_t> indexes_in){
		m_indexes = indexes_in;
	}

	void mesh::set_indexes(uint32_t* ind_arr, uint32_t size){
		for(int i=0;i<size;i++){
			m_indexes.push_back(ind_arr[i]);
		}
	}

	void mesh::set_normals(std::vector<double> normals_in){
		m_normals = normals_in;
	}
    
	
	void mesh::set_normals(double* norm_arr, uint32_t size){
		for(int i=0;i<size;i++){
			m_normals.push_back(norm_arr[i]);
		}
	}

	std::vector<uint32_t> mesh::get_indexes(){
		return m_indexes;
	}
	
	std::vector<double> mesh::get_vertexes(){
		return m_vertexes;
	}
    
    std::vector<double> mesh::get_normals(){
		return m_normals;
	}

    std::vector<uint32_t>& mesh::get_indexes_link(){
		return m_indexes;
	}

    std::vector<double>& mesh::get_vertexes_link(){
		return m_vertexes;
	}

    std::vector<double>& mesh::get_normals_link(){
		return m_normals;
	}
}