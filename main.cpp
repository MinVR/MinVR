#include "MinVRSettings.h"
#include <iostream>


int main(int argc,char** argv)
{
    std::string file_name, file_name1;
//    std::string tmp_str = "TileTopLeft             (-3.32802 , -0.191319, -2.53334)";
    std::cout << "A simple driver program for the MinVRSettings project." << std::endl;
    if(argc > 2)
    {
        file_name = argv[1];
        file_name1 = argv[2];
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
        if(minvr_settings.setValueInt("WindowAlphaBits",10) == SUCCESS)
        {
            std::cout << "WindowAlphaBits = " << minvr_settings.getValueInt("WindowAlphaBits") << std::endl;
        }
        if(minvr_settings.setValueInt("MyWindowAlphaBits",100) == SUCCESS)
        {
            std::cout << "MyWindowAlphaBits = " << minvr_settings.getValueInt("MyWindowAlphaBits") << std::endl;
        }
        vector<int> m_tmp,m_tmp1;
        for(int i=0; i<16; i++)
            m_tmp.push_back(1);
        if(minvr_settings.setValueIntVector("MyTileInitialHeadFrame",m_tmp) == SUCCESS)
        {
            minvr_settings.getValueIntVector("MyTileInitialHeadFrame",m_tmp1);
            std::cout << "MyTileInitialHeadFrame: ";
            for(int i=0; i< m_tmp1.size(); i++)
                std::cout << " " << m_tmp1[i] << ",";
            std::cout << endl;
        }
        vector<float> f_tmp,f_tmp1;
        for(int i=0; i<3; i++)
            f_tmp.push_back(-1.164);
        if(minvr_settings.setValueFloatVector("MyTileTopLeft",f_tmp) == SUCCESS)
        {
            minvr_settings.getValueFloatVector("MyTileTopLeft",f_tmp1);
            std::cout << "MyTileTopLeft: ";
            for(int i=0; i< f_tmp1.size(); i++)
                std::cout << " " << f_tmp1[i] << ",";
            std::cout << endl;
        }
        minvr_settings.writeValues(file_name1);
        // call the write() function
    }
    
    return 0;
}
