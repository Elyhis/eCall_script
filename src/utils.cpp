#include <sstream>
#include <chrono>
#include <filesystem>

#include "utils.h"

#include "matplotlibcpp.h"
namespace plt = matplotlibcpp;

// //FIXME: TEMP TO MAKE A GRAPH, WILL BE IN A UI COMPONENT
void graph(nmea nmea, std::vector<double> horizontalPos, std::string testRunning){
    std::vector<std::string> time;
    std::vector<double> validHorizontalPos;

    for(int i = 0; i < nmea.rmc.size();i++){
        if(nmea.rmc[i][2] != "V"){
            std::string temp = nmea.rmc[i][1];
            std::string hours = temp.substr(0, 2);  // Get the first two characters for hours
            std::string mins = temp.substr(2, 2);   // Get the next two characters for minutes
            std::string sec = temp.substr(4, 2);   // Get the last two characters for seconds
            std::string t = hours + ":" + mins + ":" + sec ;
            time.push_back(t);
            validHorizontalPos.push_back(horizontalPos[i]);
        }
    }
    std::vector<int> xticks;
    std::vector<std::string> xlabels;
    for (size_t i = 0; i < time.size(); ++i) {
        std::string t = time[i];
        // Extraire les minutes
        int minute = std::stoi(t.substr(3, 2));
        int second = std::stoi(t.substr(6, 2));
        if (minute % 5 == 0 && second == 0) {
            xticks.push_back(i);
            xlabels.push_back(t);
        }
    }


    // Utilise les indices pour l'axe X
    std::vector<int> x(time.size());
    for (size_t i = 0; i < time.size(); ++i)
        x[i] = static_cast<int>(i);

    // Tracer le graphique
    plt::figure_size(1200, 600);
    plt::plot(x, validHorizontalPos);
    plt::title("Position Error Over Time");
    plt::ylabel("Position error (m)");
    plt::xlabel("Time (UTC)");

    plt::xticks(xticks, xlabels);

    plt::grid(true);
    plt::tight_layout();
    plt::save("graph_" + testRunning + ".png");
}

std::string logTime(){
    auto now = std::chrono::system_clock::now();
    // Convert to local time
    auto localTime = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&localTime), "[%d/%m/%Y %T]");
    return ss.str();
}