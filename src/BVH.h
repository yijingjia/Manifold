#ifndef BVH_H_
#define BVH_H_

#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
using namespace std;

class BV {
public:
    BV();
    BV(glm::dvec3& d1, glm::dvec3& d2, glm::dvec3& d3);
    void Include(std::vector<BV*>& array, int l, int r);
    bool HitBox(glm::dvec3& o, glm::dvec3& d);
    pair<glm::dvec3, bool> rayIntersectsTriangle(glm::dvec3& p, glm::dvec3& d);
    double operator()(int dim);
    glm::dvec3 min_corner, max_corner;
    glm::dvec3* tris;
};

class BVH {
public:
    BVH();
    ~BVH();
    void clear();
    void updateBVH(std::vector<BV*>& bvs, int dim, int l, int r);
    pair<glm::dvec3, bool> rayIntersect(glm::dvec3& o, glm::dvec3& d);
    void buildParallel(std::vector<BV*>& bvs, int dim);

private:
    int axis;
    BVH *left, *right;
    BV* bv;
    int num;
    
    void processTask();
    std::queue<std::tuple<std::vector<BV*>, int, int, int>> taskQueue;
    std::mutex queueMutex;
    std::condition_variable queueCV;
    std::vector<std::thread> workers;
    bool done;
};

#endif
