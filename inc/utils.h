#include <string>

// //FIXME: TEMP TO MAKE A GRAPH, WILL BE IN A UI COMPONENT
// void graph(nmea nmea, std::vector<double> horizontalPos){
//     std::vector<std::string> time;
//     std::string filePath = "graph.csv";
//     // file pointer
//     std::fstream fout;
    
//     // opens an existing csv file or creates a new file.
//     fout.open(filePath, std::fstream::out);
//     fout << "Time(UTC),Position error (m)\n";
//     int delta =nmea.rmc.size() - horizontalPos.size();
//     delta = abs(delta);
//     for(int i = 0; i < nmea.rmc.size();i++){
//         if(nmea.rmc[i][2] != "V"){
//             std::string temp = nmea.rmc[i][1];
//             std::string hours = temp.substr(0, 2);  // Get the first two characters for hours
//             std::string mins = temp.substr(2, 2);   // Get the next two characters for minutes
//             std::string sec = temp.substr(4, 2);   // Get the last two characters for seconds
//             std::string t = hours + ":" + mins + ":" + sec ;
//             time.push_back(t);
//         }
//     }
//     for(int i = 0; i < time.size();i++){
//         fout << time[i] << "," << horizontalPos[i] << std::endl;
//     }
//     fout.close();
// }

std::string logTime();