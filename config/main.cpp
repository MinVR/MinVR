#include "MinVRSettings.h"
#include <iostream>
#include <cstdlib>
#include <ctime>


int main(int argc,char** argv)
{
    std::string file_name_1, file_name_2;
    std::cout << "A simple driver program for the MinVRSettings project." << std::endl;
    if(argc > 2)
    {
        file_name_1 = argv[1]; // input file name
        file_name_2 = argv[2]; // output file name
        MinVRSettings minvr_settings;
        
        std::cout << "Opening file = " << file_name_1 << " for reading..." << std::endl;
        minvr_settings.readValues(file_name_1);
        
        // Print some options...
        std::cout << "WindowDepthBits = " << minvr_settings.getValueInt("WindowDepthBits") << std::endl;
        std::cout << "WindowDepthBits = " << minvr_settings.getValueInt("WindowDepthBits") << std::endl;
        std::cout << "WindowRefreshRate = " << minvr_settings.getValueFloat("WindowRefreshRate") << std::endl;
        
        std::vector<float> tmp;
        minvr_settings.getValueFloatVector("TileBotLeft",tmp);
        std::cout << "TileBotLeft: ";
        for(int i=0; i<tmp.size(); i++)
            std::cout << "  " << tmp[i];
        std::cout << std::endl;
        
        std::cout << "ScalableMeshFile = " << minvr_settings.getValueString("ScalableMeshFile") << std::endl;
        
        // Try setting a few options...
        if(minvr_settings.setValueInt("WindowAlphaBits",10) == SUCCESS)
        {
            std::cout << "WindowAlphaBits = " << minvr_settings.getValueInt("WindowAlphaBits") << std::endl;
        }
        if(minvr_settings.setValueInt("MyWindowAlphaBits",100) == SUCCESS)
        {
            std::cout << "MyWindowAlphaBits = " << minvr_settings.getValueInt("MyWindowAlphaBits") << std::endl;
        }
        
        vector<int> i_tmp,i_tmp1;
        srand(time(0));
        for(int i=0; i<16; i++)
            i_tmp.push_back(rand() % 2);
        if(minvr_settings.setValueIntVector("MyTileInitialHeadFrame",i_tmp) == SUCCESS)
        {
            minvr_settings.getValueIntVector("MyTileInitialHeadFrame",i_tmp1);
            std::cout << "MyTileInitialHeadFrame: ";
            for(int i=0; i<i_tmp1.size(); i++)
                std::cout << " " << i_tmp1[i] << ",";
            std::cout << endl;
        }
        
        vector<float> f_tmp,f_tmp1;
        for(int i=0; i<3; i++)
            f_tmp.push_back(static_cast<double>(rand())/RAND_MAX);
        if(minvr_settings.setValueFloatVector("MyTileTopLeft",f_tmp) == SUCCESS)
        {
            minvr_settings.getValueFloatVector("MyTileTopLeft",f_tmp1);
            std::cout << "MyTileTopLeft: ";
            for(int i=0; i<f_tmp1.size(); i++)
                std::cout << " " << f_tmp1[i] << ",";
            std::cout << endl;
        }
        
        // call the write() function
        std::cout << "Opening file = " << file_name_2 << " for writing..." << std::endl;
        minvr_settings.writeValues(file_name_2);

	cout << "============XML test=========" << endl;
	cout << "getValueString(CAVEViewer/headDeviceTransformation)=" << minvr_settings.getValueString("CAVEViewer/headDeviceTransformation") << endl;

    }
    else
    {
        std::cout << "Usage: driver input_file_name output_file_name" << std::endl;
    }
    return 0;
}
