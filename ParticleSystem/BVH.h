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
    void PrintBVH()
    {
        std::cout << "{\n";
        std::cout << "  min point: (" << aabbMin.x << ", " << aabbMin.y << ", " << aabbMin.z  << ")\n";
        std::cout << "  max point: (" << aabbMax.x << ", " << aabbMax.y << ", " << aabbMax.z  << ")\n";
        std::cout << "  leftChild: " << leftChild << std::endl;
        std::cout << "  rightChild: " << rightChild << std::endl;
        std::cout << "  firstParticleIndex: " << firstParticleIndex << std::endl;
        std::cout << "  particleCount: " << particleCount << std::endl;
        std::cout << "}\n\n";
    }
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

