#include <lod_math.h>


glm::vec3 get_face_normal(const glm::vec3 v1,const glm::vec3 v2,const glm::vec3 v3){
    glm::vec3 face_normal = {0,0,0};

    glm::vec3 ab = v1 - v2;
    glm::vec3 ac = v1 - v3;
    
    face_normal = glm::cross(ab, ac);
    return glm::normalize(face_normal);
}