#pragma once
#include <limits>
#include "Coordinate.h"
#include "Particle.h"

struct BVHNode
{
    Coordinate aabbMin;
    Coordinate aabbMax;
    size_t leftChild;
    size_t rightChild;
    size_t firstParticleIndex;
    size_t particleCount;

    bool isLeaf() { return particleCount > 0; }
};

class BVH
{
public:
	BVH();
	~BVH();
    void Update(Particle* particleList, size_t particleCount);
private:
    BVHNode* m_nodeTree;
    size_t* m_particleIndexArray;
    Particle* m_particleList;
    size_t m_particleCount;
    size_t m_nodesUsed;

    void UpdateNodeBounds(size_t nodeIdx);
    void Subdivide(size_t nodeIdx);
};

