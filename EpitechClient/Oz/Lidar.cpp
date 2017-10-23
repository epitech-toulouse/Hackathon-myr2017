#include "Lidar.hh"

using namespace std;

class Lidar {

    Lidar::Lidar(){
        cout << "Starting LIDAR" << endl;
    }

    Lidar::~Lidar(){
        cout << "Ending LIDAR" << endl;
    }

    int startLidar() {
        Lidar lidar = new Lidar();

        delete lidar;
    }
}