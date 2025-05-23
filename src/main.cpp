#include <string>
#include <vector>
#include <iostream>

//Ui
#include "mainwindow.h"
#include <QApplication>
bool isHorizontalErrorLessThan15(std::vector<double> horizontalPos){
    int threshold = 15;
    //Checking if receiver pass the test or not
    bool isLessThan15Meters = true;
    for(double value : horizontalPos){
        if (value > threshold) isLessThan15Meters = false;
    }
    return isLessThan15Meters;
}
//2.2.1 test to check if there is a message with value 2 at 6th position
bool isField6Correct(std::vector<std::vector<std::string>> ggaMessage){
    int i = 0;
    while(i < ggaMessage.size() && ggaMessage[i][6] != "2"){
        i++;
    }
    if(i < ggaMessage.size()){
        std::cout << "\" "  << std::endl;
        for(auto i : ggaMessage[i]){
            std::cout << i << ", ";
        }
        std::cout << " \""  << std::endl;
        return true;
    }
    return false;
}

int main(int argc, char *argv[]){
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}