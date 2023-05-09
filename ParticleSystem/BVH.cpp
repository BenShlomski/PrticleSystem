#include "BVH.h"

BVH::BVH()
{
    m_nodeTree = NULL;
    m_particleIndexArray = NULL;
    m_particleList = NULL;
    m_particleCount = 0;
    m_nodesUsed = 0;
}

BVH::~BVH()
{
    delete[] m_nodeTree;
    delete[] m_particleIndexArray;
}

void BVH::Update(Particle* particleList, size_t particleCount)
{
    if (m_nodeTree != NULL && m_particleIndexArray != NULL)
    {
        delete[] m_nodeTree;
        delete[] m_particleIndexArray;
    }

    m_nodeTree = new BVHNode[2 * particleCount - 1];
	m_particleIndexArray = new size_t[particleCount];
    for (size_t i = 0; i < particleCount; i++) m_particleIndexArray[i] = i;
    m_particleList = particleList;
    m_particleCount = particleCount;
    m_nodesUsed = 1;

    // assign all particles to root node
    BVHNode& root = m_nodeTree[0];
    root.leftChild = root.rightChild = 0;
    root.firstParticleIndex = 0;
    root.particleCount = particleCount;
    UpdateNodeBounds(0);
    // subdivide recursively
    Subdivide(0);
}

void BVH::UpdateNodeBounds(size_t nodeIdx)
{
    BVHNode& node = m_nodeTree[nodeIdx];
    node.aabbMin = m_particleList[node.firstParticleIndex].getPosition();
    node.aabbMax = m_particleList[node.firstParticleIndex].getPosition();

    for (size_t first = node.firstParticleIndex, i = 0; i < node.particleCount; i++)
    {
        size_t particleIndex = m_particleIndexArray[first + i];
        Particle& leafParticle = m_particleList[particleIndex];

        node.aabbMin = node.aabbMin.coordinateMin(leafParticle.getPosition());
        node.aabbMax = node.aabbMax.coordinateMax(leafParticle.getPosition());
    }
}

void BVH::Subdivide(size_t nodeIdx)
{
    // terminate recursion
    BVHNode& node = m_nodeTree[nodeIdx];
    if (node.particleCount <= 2) return;

    // determine split axis and position
    Coordinate extent = node.aabbMax - node.aabbMin;
    int axis = 0;
    if (extent.y > extent.x) axis = 1;
    if (extent.z > extent.y) axis = 2;
    float splitPos = node.aabbMin[axis] + extent[axis] * 0.5f;

    // in-place partition
    int i = node.firstParticleIndex;
    int j = i + node.particleCount - 1;

    while (i <= j)
    {
        if (m_particleList[m_particleIndexArray[i]].getPosition()[axis] < splitPos)
            i++;
        else
        {
            // swap particle positions
            size_t temp = m_particleIndexArray[i];
            m_particleIndexArray[i] = m_particleIndexArray[j--];
            m_particleIndexArray[j--] = temp;

        }
    }
    // abort split if one of the sides is empty
    int leftCount = i - node.firstParticleIndex;
    if (leftCount == 0 || leftCount == node.particleCount) return;

    // create child nodes
    int leftChildIdx = m_nodesUsed++;
    int rightChildIdx = m_nodesUsed++;
    m_nodeTree[leftChildIdx].firstParticleIndex = node.firstParticleIndex;
    m_nodeTree[leftChildIdx].particleCount = leftCount;
    m_nodeTree[rightChildIdx].firstParticleIndex = i;
    m_nodeTree[rightChildIdx].particleCount = node.particleCount - leftCount;
    node.leftChild = leftChildIdx;
    node.particleCount = 0;
    UpdateNodeBounds(leftChildIdx);
    UpdateNodeBounds(rightChildIdx);

    // recurse
    Subdivide(leftChildIdx);
    Subdivide(rightChildIdx);
}