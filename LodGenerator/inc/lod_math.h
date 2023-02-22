#ifndef LOD_MATH_H
#define LOD_MATH_H
#include <glm/glm.hpp>
#include <global.h>

namespace lod_generator {
    glm::vec3 get_face_normal(const glm::vec3,const glm::vec3,const glm::vec3);
    int get_faces_normals(std::vector<double>& vertexes, std::vector<uint32_t>& indexes, std::vector<glm::vec3>& normals);
}

#endif