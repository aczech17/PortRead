#include "Port.h"
#include <iostream>
using std::cin, std::cout, std::endl, std::cerr, std::string;
int main()
{
    const int portsCount = 25;
    for(int i = 0; i < portsCount; i++)
    {
        std::string portName = "COM" + std::to_string(i);
        try
        {
            Port port(portName.data()); // const char* required in constructor
            std::vector<int> vec;
            for (int j = 0; j < 200; j++)
            {
                vec = port.getData();
                cout << vec[0] << " " << vec[1] << " " << vec[2] << endl;
            }
        }
        catch (std::ios_base::failure &exc)
        {
            cerr << exc.what() << endl;
            continue;
        }
        return 0;
    }
    cerr << "No ports" << endl;
    return 1;
}