#pragma once

#include <vector>
#include <unordered_map>

#include "gaden/Vector3.hpp"
#include "gaden/Vector3Field.hpp"
#include "gaden/Field.hpp"

namespace gaden {

class AutoMergingPointCloud {

    // Private data

        static const IntField m_empty;

        // Indexed together

            // Underlying pointList
            // autoPtr<DynamicList<point>> pointsPtr_;
            Vector3Field m_points;

            // Associated squared distance
            ScalarField m_magSqrDist;

            // Associated scaled squared tolerance
            ScalarField m_scaledTolSqr;


        // Break up magSqrDist list into buckets with half-tolerance width based on scaled tolerance
        //  Given a magSqrDist, merge candidates can be determined:
        //      * bucketIndex = msdBucket(magSqrDist)
        //      * candidateIndices = msdBucketIndices_[bucketIndex]

        // HashTable<DynamicList<label>, label> msdBucketIndices_;
        std::unordered_map<int, IntField> m_msdBucketIndices;

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
        inline const IntField& getCandidateIndicies(int bucketIndex) const
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
        AutoMergingPointCloud(int estimatedSize, double mergeTol, std::string name="");

        // //- Copy constructor
        // AutoMergingPointCloud(const AutoMergingPointCloud&);


    //- Destructor
    ~AutoMergingPointCloud() = default;


    // Member Functions

        // Access

            //- Return underlying points
            //  Non-const access achieved through transfer()
            inline const Vector3Field& points() const
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


            // Hand over ownership of data, invalidates in-class data
            Vector3Field transfer() && noexcept {
                // tidy up related caches
                m_magSqrDist.clear();
                m_scaledTolSqr.clear();
                // move out the payload, leave this empty
                return std::exchange(m_points, {});
            }

            // Safety - forbid transfer() on lvalues
            Vector3Field transfer() & = delete;
};

} // end namespace gaden