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
    if (m_nodeTree != NULL)
    {
        delete[] m_nodeTree;
        m_nodeTree = NULL;
    }

    if (m_particleIndexArray != NULL)
    {
        // TODO this is the line. i am so done.
        delete[] m_particleIndexArray;
        m_particleIndexArray = NULL;
    }

    m_nodeTree = new BVHNode[2 * particleCount - 1];
    if (m_nodeTree == NULL)
    {
        return;
    }

	m_particleIndexArray = new size_t[particleCount];
    if (m_particleIndexArray == NULL)
    {
        delete[] m_nodeTree;
        return;
    }

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
    Coordinate particlePosition = m_particleList[node.firstParticleIndex].getPosition();
    size_t particleRadius = m_particleList[node.firstParticleIndex].getRadius();
    node.aabbMin = { particlePosition.x - particleRadius, particlePosition.y - particleRadius, particlePosition.z - particleRadius, };
    node.aabbMax = { particlePosition.x + particleRadius, particlePosition.y + particleRadius, particlePosition.z + particleRadius, };

    for (size_t first = node.firstParticleIndex, i = 0; i < node.particleCount; i++)
    {
        size_t particleIndex = m_particleIndexArray[first + i];
        Particle& leafParticle = m_particleList[particleIndex];

        particlePosition = leafParticle.getPosition();
        particleRadius = leafParticle.getRadius();

        node.aabbMin = node.aabbMin.coordinateMin({ particlePosition.x - particleRadius, particlePosition.y - particleRadius, particlePosition.z - particleRadius, });
        node.aabbMax = node.aabbMax.coordinateMax({ particlePosition.x + particleRadius, particlePosition.y + particleRadius, particlePosition.z + particleRadius, });
    }

    // print BVH status
    //node.PrintBVH();
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
            m_particleIndexArray[i] = m_particleIndexArray[j];
            m_particleIndexArray[j] = temp;
            j--;
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
    node.rightChild = rightChildIdx;
    node.particleCount = 0;
    UpdateNodeBounds(leftChildIdx);
    UpdateNodeBounds(rightChildIdx);

    // recurse
    Subdivide(leftChildIdx);
    Subdivide(rightChildIdx);
}

void BVH::HandleCollision()
{
    for (size_t i = 0; i < m_particleCount; i++)
    {
        Particle& particle = m_particleList[i];
        Coordinate particlePosition = particle.getPosition();
        float particleRaius = particle.getRadius();

        HandleParticleCollisions(particle, 0, { particlePosition.x - particleRaius, particlePosition.y - particleRaius, particlePosition.z - particleRaius },
            { particlePosition.x + particleRaius, particlePosition.y + particleRaius, particlePosition.z + particleRaius });
    }
}

void BVH::HandleParticleCollisions(Particle& particle, size_t nodeIndex, Coordinate particleMinCoordinate,Coordinate particleMaxCoordinate)
{
    BVHNode& node = m_nodeTree[nodeIndex];

    // stop state
    if (!(((node.aabbMax.x >= particleMaxCoordinate.x && node.aabbMin.x <= particleMaxCoordinate.x) || (node.aabbMax.x >= particleMinCoordinate.x && node.aabbMin.x <= particleMinCoordinate.x)) &&
        ((node.aabbMax.y >= particleMaxCoordinate.y && node.aabbMin.y <= particleMaxCoordinate.y) || (node.aabbMax.y >= particleMinCoordinate.y && node.aabbMin.y <= particleMinCoordinate.y)) &&
        ((node.aabbMax.z >= particleMaxCoordinate.z && node.aabbMin.z <= particleMaxCoordinate.z) || (node.aabbMax.z >= particleMinCoordinate.z && node.aabbMin.z <= particleMinCoordinate.z))))
    {
        return; // no intersection with volume
    }
    if (node.isLeaf())
    {
        for (size_t i = 0; i < node.particleCount; i++)
        {
            particle.handleCollision(m_particleList[m_particleIndexArray[node.firstParticleIndex + i]]);
        }

        return; // collision handled
    }

    // recurse
    HandleParticleCollisions(particle, node.leftChild, particleMinCoordinate, particleMaxCoordinate);
    HandleParticleCollisions(particle, node.rightChild, particleMinCoordinate, particleMaxCoordinate);
}

BVHNode* BVH::GetRoot()
{
    return m_nodeTree;
}

size_t BVH::GetNodeCount()
{
    return m_nodesUsed;
}

size_t BVH::GetGloabalIndexFromNodeIndex(size_t index)
{
    return m_particleIndexArray[index];
}
