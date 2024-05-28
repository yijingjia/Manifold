#include "BVH.h"

BVH::BVH() : left(0), right(0), bv(0), done(false) {}

BVH::~BVH() {
    clear();
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        done = true;
    }
    queueCV.notify_all();
    for (auto& worker : workers) {
        worker.join();
    }
}

void BVH::clear() {
    if (left)
        delete left;
    if (right)
        delete right;
    delete bv;
    left = 0;
    right = 0;
    bv = 0;
}

void BVH::updateBVH(std::vector<BV*>& bvs, int dim, int l, int r) {
    left = 0, right = 0;
    axis = dim;
    if (l == r) {
        bv = bvs[l];
        return;
    }
    if (l < 0) {
        l = 0;
        r = (int)bvs.size() - 1;
    }
    num = r - l + 1;
    bv = new BV();
    bv->Include(bvs, l, r);
    double pivot = (*bv)(dim);
    int i = l, j = r;
    while (i < j) {
        while ((*bvs[i])(dim) < pivot) {
            ++i;
        }
        while ((*bvs[j])(dim) > pivot) {
            --j;
        }
        if (i <= j) {
            BV* bv = bvs[i];
            bvs[i] = bvs[j];
            bvs[j] = bv;
            ++i, --j;
        }
    }
    if (i > l + 1)
        --i;
    if (i > l) {
        left = new BVH();
        left->updateBVH(bvs, dim % 3, l, i - 1);
    }
    if (i <= r) {
        right = new BVH();
        right->updateBVH(bvs, dim % 3, i, r);
    }
}

pair<glm::dvec3, bool> BVH::rayIntersect(glm::dvec3& o, glm::dvec3& d) {
    if (left == 0 && right == 0) {
        if (!bv->tris || !bv->HitBox(o, d))
            return make_pair(glm::dvec3(), false);
        return bv->rayIntersectsTriangle(o, d);
    }
    pair<glm::dvec3, bool> p1, p2;
    p1.second = false;
    p2.second = false;
    if (!bv->HitBox(o, d))
        return p1;
    if (left)
        p1 = left->rayIntersect(o, d);
    if (right)
        p2 = right->rayIntersect(o, d);
    if (!p2.second)
        return p1;
    if (!p1.second)
        return p2;
    if (glm::dot(p1.first - o, d) < glm::dot(p2.first - o, d))
        return p1;
    else
        return p2;
}

void BVH::processTask() {
    while (true) {
        std::tuple<std::vector<BV*>, int, int, int> task;
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            queueCV.wait(lock, [this] { return !taskQueue.empty() || done; });
            if (done && taskQueue.empty()) break;
            task = taskQueue.front();
            taskQueue.pop();
        }
        auto [bvs, dim, l, r] = task;
        updateBVH(bvs, dim, l, r);
    }
}

void BVH::buildParallel(std::vector<BV*>& bvs, int dim) {
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        taskQueue.push(std::make_tuple(bvs, dim, 0, bvs.size() - 1));
    }
    queueCV.notify_all();

    size_t numThreads = std::thread::hardware_concurrency();
    for (size_t i = 0; i < numThreads; ++i) {
        workers.emplace_back(&BVH::processTask, this);
    }
}
