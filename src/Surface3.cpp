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
    if (epsilon <= 0.0) {
        while (std::getline(is, buffer)) {
            Log_Debug4("Line=[" << buffer << "]");
            std::istringstream lineIss(buffer);
            char c = lineIss.peek();
            if (!Tools::isNumber(c)) {
                Log_Debug("First character is not a number, c=" << int(c));
                continue;
            }
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
            std::istringstream lineIss(buffer);
            char c = lineIss.peek();
            if (!Tools::isNumber(c)) {
                Log_Debug("First character is not a number, c=" << int(c));
                continue;
            }
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
    Log_Info("Read " << m_points.size() << " points with bounds " << bb);
    return true;
}
