#include "gaden/AutoMergingPointsArray.hpp"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace gaden
{
    const std::vector<int> AutoMergingPointsArray::m_empty;
}


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

void gaden::AutoMergingPointsArray::addToBucketIndices(double msd, int pointIndex)
{
    const int bucketIndex = getBucketIndex(msd);
    auto iter = m_msdBucketIndices.find(bucketIndex);
    if (iter == m_msdBucketIndices.end())
    {
        // Create a new list and add it
        // DynamicList<int> candidates(1);
        std::vector<int> candidates;
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

gaden::AutoMergingPointsArray::AutoMergingPointsArray(int estimatedSize, double mergeTol)
:
    m_mergeTol(mergeTol),
    m_mergeTolSqr(mergeTol*mergeTol),
    m_invMergeTol(1.0/mergeTol)
{
    m_points.reserve(estimatedSize);
    m_magSqrDist.reserve(estimatedSize);
    m_scaledTolSqr.reserve(estimatedSize);
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

int gaden::AutoMergingPointsArray::append(const Vector3& pt)
{
    double msd = pt.magSqr();
    double scaledTol = 2*m_mergeTol*(abs(pt.x()) + abs(pt.y()) + abs(pt.z()));

    int from = getBucketIndex(msd - scaledTol);
    int to = getBucketIndex(msd + scaledTol);
    for (int bucketI = from; bucketI <= to; ++bucketI)
    {
        const std::vector<int>& candidates = getCandidateIndicies(bucketI);
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
