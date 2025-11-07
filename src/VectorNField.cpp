#include "gaden/VectorNField.hpp"

gaden::IndexedVector2Field gaden::convertToIndexed(
    const Vector2Field& fldIn, int start, int step
) {
    IndexedVector2Field fldOut(fldIn.name());
    fldOut.reserve(fldIn.size());
    int idx = start;
    for (const Vector2& elemIn : fldIn) {
        fldOut.emplace_back(elemIn, idx);
        idx += step;
    }
    return fldOut;
}


gaden::IndexedVector3Field gaden::convertToIndexed(
    const Vector3Field& fldIn, int start, int step
) {
    IndexedVector3Field fldOut(fldIn.name());
    fldOut.reserve(fldIn.size());
    int idx = start;
    for (const Vector3& elemIn : fldIn) {
        fldOut.emplace_back(elemIn, idx);
        idx += step;
    }
    return fldOut;
}


gaden::Vector2Field gaden::stripIndexing(const IndexedVector2Field& fldIn) {
    Vector2Field fldOut(fldIn.name());
    fldOut.reserve(fldIn.size());
    for (const IndexedVector2& elemIn : fldIn) {
        fldOut.push_back(Vector2(elemIn.x(), elemIn.y()));
    }
    return fldOut;
}


gaden::Vector3Field gaden::stripIndexing(const IndexedVector3Field& fldIn) {
    Vector3Field fldOut(fldIn.name());
    fldOut.reserve(fldIn.size());
    for (const IndexedVector3& elemIn : fldIn) {
        fldOut.push_back(Vector3(elemIn.x(), elemIn.y(), elemIn.z()));
    }
    return fldOut;
}
