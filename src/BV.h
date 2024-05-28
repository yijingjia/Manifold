#ifndef BV_H
#define BV_H

#include <vector>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

class BV {
public:
    BV();
    BV(glm::dvec3& d1, glm::dvec3& d2, glm::dvec3& d3);
    void Include(std::vector<BV*>& array, int l, int r);
    bool HitBox(glm::dvec3& o, glm::dvec3& d);
    std::pair<glm::dvec3, bool> rayIntersectsTriangle(glm::dvec3& p, glm::dvec3& d);
    double operator()(int dim);
    glm::dvec3 min_corner, max_corner;
    glm::dvec3* tris;
};

#endif // BV_H
