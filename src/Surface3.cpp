#include "gaden/Surface3.hpp"

#include "gaden/AutoMergingPointCloud.hpp"
#include "gaden/BoundBox.hpp"
#include "gaden/Logger.hpp"


bool gaden::Surface3::read(std::istream& is, double epsilon) {
    if (!is.good()) {
        Log_Error("Cannot read from bad stream");
        return false;
    }
    std::string buffer;
    char comma;
    BoundBox bb;
    int nMerged = 0;
    int nLines = 0;
    int nValidLines = 0;
    if (epsilon <= 0.0) {
        while (std::getline(is, buffer)) {
            Log_Debug4("Line=[" << buffer << "]");
            ++nLines;
            std::istringstream lineIss(buffer);
            char c = lineIss.peek();
            if (!Tools::isNumber(c)) {
                Log_Debug("First character is not a number, c=" << int(c));
                continue;
            }
            ++nValidLines;
            m_faceNumber.readElem(lineIss);
            lineIss >> comma;
            m_points.readElem(lineIss);
            bb.append(m_points.back());
            lineIss >> comma;
            m_normals.readElem(lineIss);
        }
    } else {
        AutoMergingPointCloud amp(0, epsilon, name());
        while (std::getline(is, buffer)) {
            Log_Debug4("Line=[" << buffer << "]");
            ++nLines;
            std::istringstream lineIss(buffer);
            char c = lineIss.peek();
            if (!Tools::isNumber(c)) {
                Log_Debug("First character is not a number, c=" << int(c));
                continue;
            }
            ++nValidLines;
            int fn;
            Vector3 pt;
            Vector3 nm;
            lineIss >> fn;
            lineIss >> comma;
            lineIss >> pt;
            lineIss >> comma;
            lineIss >> nm;

            std::pair<bool, int> newPtAndId = amp.append(pt);
            if (!newPtAndId.first) {
                // Point merged, skip
                ++nMerged;
                continue;
            }
            // faceNumber - fn?  or newPtAndId.second?
            m_faceNumber.push_back(fn);
            // m_points.push_back(pt);
            m_normals.push_back(nm);
            bb.append(pt);
        }
        m_points = std::move(amp).transfer();
    }
    Log_Info(""
        << "Read results:\n"
        << "\tLines         : " << nLines << "\n"
        << "\tValid points  : " << nValidLines << "\n"
        << "\tMerged points : " << nMerged << "\n"
        << "\tUnique points : " << m_points.size()
    );
    return true;
}
