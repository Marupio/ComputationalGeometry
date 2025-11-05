#include "gaden/AutoMergingPointCloud.hpp"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace gaden
{
    const IntField AutoMergingPointCloud::m_empty;
}


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

void gaden::AutoMergingPointCloud::addToBucketIndices(double msd, int pointIndex)
{
    const int bucketIndex = getBucketIndex(msd);
    auto iter = m_msdBucketIndices.find(bucketIndex);
    if (iter == m_msdBucketIndices.end())
    {
        // Create a new list and add it
        // DynamicList<int> candidates(1);
        IntField candidates;
        candidates.push_back(pointIndex);
        m_msdBucketIndices.insert({bucketIndex, candidates});
    }
    else
    {
        // Append to existing list
        iter->second.push_back(pointIndex);
    }
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

gaden::AutoMergingPointCloud::AutoMergingPointCloud(
    int estimatedSize, double mergeTol, std::string name
) :
    m_mergeTol(mergeTol),
    m_mergeTolSqr(mergeTol*mergeTol),
    m_invMergeTol(1.0/mergeTol)
{
    if (!name.empty()) {
        m_points.rename(name);
    }
    m_points.reserve(estimatedSize);
    m_magSqrDist.reserve(estimatedSize);
    m_scaledTolSqr.reserve(estimatedSize);
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

int gaden::AutoMergingPointCloud::append(const Vector3& pt)
{
    double msd = pt.magSqr();
    double scaledTol = 2*m_mergeTol*(abs(pt.x()) + abs(pt.y()) + abs(pt.z()));

    int from = getBucketIndex(msd - scaledTol);
    int to = getBucketIndex(msd + scaledTol);
    for (int bucketI = from; bucketI <= to; ++bucketI)
    {
        const IntField& candidates = getCandidateIndicies(bucketI);
        int nCandidates = candidates.size();
        for (int ci = 0; ci < nCandidates; ++ci)
        {
            const int candidateI = candidates[ci];
            // Quick reject
            if (abs(m_magSqrDist[candidateI] - msd) <= scaledTol)
            {
                // Actual test
                if ( (pt - m_points[candidateI]).magSqr() <= m_mergeTolSqr )
                {
                    // Found match
                    return candidateI;
                }
            }
        }
    }
    // No merged point, append to lists
    const int newPtIndex = m_points.size();
    m_points.push_back(pt);
    m_magSqrDist.push_back(msd);
    m_scaledTolSqr.push_back(scaledTol);
    addToBucketIndices(msd, newPtIndex);
    return newPtIndex;
}


// ************************************************************************* //
