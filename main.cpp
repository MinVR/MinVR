#include "MinVRSettings.h"
#include <iostream>


int main(int argc,char** argv)
{
    std::string file_name;
//    std::string tmp_str = "TileTopLeft             (-3.32802 , -0.191319, -2.53334)";
    std::cout << "A simple driver program for the MinVRSettings project." << std::endl;
    if(argc > 1)
    {
        file_name = argv[1];
        std::cout << "Opening file = " << file_name << std::endl;
        MinVRSettings minvr_settings;
//        vector<std::string> m_strings = minvr_settings.string_float_vec_test("TileTopLeft             (-3.32802 , -0.191319, -2.53334)");
//        for(int i=0; i< m_strings.size(); i++)
//            std::cout << m_strings[i] << std::endl;
        minvr_settings.readValues(file_name);
        std::cout << "WindowDepthBits = " << minvr_settings.getValueInt("WindowDepthBits") << std::endl;
        std::cout << "WindowDepthBits = " << minvr_settings.getValueInt("WindowDepthBits") << std::endl;
        std::cout << "WindowRefreshRate = " << minvr_settings.getValueFloat("WindowRefreshRate") << std::endl;
        std::vector<float> tmp;
        minvr_settings.getValueFloatVector("TileBotLeft",tmp);
        std::cout << "TileBotLeft: " << std::endl;
        for(int i=0; i< tmp.size(); i++)
            std::cout << "  " << tmp[i] << std::endl;
        std::cout << "ScalableMeshFile = " << minvr_settings.getValueString("ScalableMeshFile") << std::endl;
        // call the write() function
    }
    
    return 0;
}
