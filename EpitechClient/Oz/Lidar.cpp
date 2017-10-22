#include "Lidar.hh"

using namespace std;

Lidar::Lidar(){
    cout << "Starting LIDAR" << endl;
}

Lidar::~Lidar(){
    cout << "Ending LIDAR" << endl;
}

int startLidar() {
    Lidar = new Lidar();
    // do stuff

    Lidar.~Lidar()
}