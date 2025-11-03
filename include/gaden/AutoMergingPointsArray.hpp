#pragma once

#include <vector>
#include <unordered_map>

#include "gaden/Vector3.hpp"

namespace gaden {

class AutoMergingPointsArray {

    // Private data

        static const std::vector<int> m_empty;

        // Indexed together

            // Underlying pointList
            // autoPtr<DynamicList<point>> pointsPtr_;
            std::vector<Vector3> m_points;

            // Associated squared distance
            std::vector<double> m_magSqrDist;

            // Associated scaled squared tolerance
            std::vector<double> m_scaledTolSqr;


        // Break up magSqrDist list into buckets with half-tolerance width based on scaled tolerance
        //  Given a magSqrDist, merge candidates can be determined:
        //      * bucketIndex = msdBucket(magSqrDist)
        //      * candidateIndices = msdBucketIndices_[bucketIndex]

        // HashTable<DynamicList<label>, label> msdBucketIndices_;
        std::unordered_map<int, std::vector<int>> m_msdBucketIndices;

        // Merge tolerance
        const double m_mergeTol;
        const double m_mergeTolSqr;
        const double m_invMergeTol;


    // Private Member Functions

        //- Get bucket index for a given magSqrDist
        inline int getBucketIndex(double magSqrDist) const
        {
            return magSqrDist*m_invMergeTol;
        }

        //- Get list of candidate indices for a given bucketIndex
        //  Returns empty list if not found
        inline const std::vector<int>& getCandidateIndicies(int bucketIndex) const
        {
            const auto iter = m_msdBucketIndices.find(bucketIndex);
            if (iter == m_msdBucketIndices.cend())
            {
                return m_empty;
            }
            return iter->second;
        }

        void addToBucketIndices(double msd, int pointIndex);


public:

    // Constructors

        //- Construct given input components
        AutoMergingPointsArray(int estimatedSize, double mergeTol);

        // //- Copy constructor
        // AutoMergingPointsArray(const AutoMergingPointsArray&);


    //- Destructor
    ~AutoMergingPointsArray() = default;


    // Member Functions

        // Access

            //- Return underlying points
            //  Non-const access achieved through transfer()
            inline const std::vector<Vector3>& points() const
            {
                return m_points;
            }

            //- Return current number of points
            inline int size() const
            {
                return m_points.size();
            }

            //- Return i^th element
            inline const Vector3& operator[](const int i)
            {
                return m_points.at(i);
            }


        // Modify

            //- Append new point to list, merging as required
            //  Returns point index after merging
            int append(const Vector3&);

            //- Transfer underlying points to caller, reset this object
            inline std::vector<Vector3>&& transfer()
            {
                m_magSqrDist.clear();
                m_scaledTolSqr.clear();
                return std::move(m_points);
            }

};

} // end namespace gaden