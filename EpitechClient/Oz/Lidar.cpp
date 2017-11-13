<<<<<<< HEAD
#include "Lidar.hh"

using namespace std;

    int startLidar() {
        Lidar *lidar = new Lidar();

        delete lidar;
        return 0;
    }
=======
#include <iostream>
#include "Oz/Lidar.hh"

namespace Oz {
  
  Lidar::Lidar(void)
  {
    std::cout << "Lidar initialisation" << std::endl;
  }
}
>>>>>>> 437273d70176d43e2e39df55b8292fc6b2d78322
