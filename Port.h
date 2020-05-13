#ifndef SERIAL_READ_PORT_H
#define SERIAL_READ_PORT_H
#include <windows.h>
#include <string>
#include <ios> //ios::exception
#include <vector>

template <typename dataType = int>
class Port
{
    typedef std::vector <dataType> vec;
    HANDLE hComm;
    DCB serialConfig = {0};
    COMMTIMEOUTS timeouts = {0};
    vec dataVec;
    std::string readLine()
    {
        char tempChar;
        std::string buffer;
        DWORD countBytesRead = 0;
        do
        {
            ReadFile(hComm,            //file handle
                     &tempChar,         //char buffer
                     sizeof(tempChar),  //number of bytes to read
                     &countBytesRead,   //number of bytes currently read
                     nullptr            //lpOverlapped https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-readfile
            );
            buffer += tempChar;
        }while(tempChar != '\n');
        return buffer;
    }
    vec getDataFromStr(std::string str)
    {
        vec result;
        auto i = str.begin();

        auto j = i;
        while(true)
        {
            while(*j != '\n' && *j !=' ')
                j++;
            std::string tmpStr = str.substr(i - str.begin(), j - i);
            dataType data = (std::stod(tmpStr));
            result.push_back(data);

            if(*j != '\n')
                j++;
            else
                break;
            i = j;
        }

        return result;
    }
public:
    Port(const char* portName = "COM3", DWORD BaudRate = CBR_9600)
    {
        hComm = CreateFile(
                portName,      //port name
                GENERIC_READ,  //read only
                0,             //share mode (0 ==> cannot be shared)
                nullptr,       //security attributes (nullptr ==> handle cannot be inherited) http://winapi.freetechsecrets.com/win32/WIN32CreateFile.htm
                OPEN_EXISTING, //creation distribution (open only if existing)
                0,             //flags and attributes (link ^)
                nullptr        //handle to files to copy (none)
        );
        if (hComm == INVALID_HANDLE_VALUE)
        {
            std::string errMsg = "Cannot open port ";
            errMsg += portName;
            throw(std::ios_base::failure(errMsg));
        }



        serialConfig.DCBlength = sizeof(serialConfig);
        serialConfig.BaudRate = BaudRate;
        serialConfig.ByteSize = 8;
        serialConfig.StopBits = ONESTOPBIT;
        serialConfig.Parity = NOPARITY;
        serialConfig.fRtsControl = RTS_CONTROL_ENABLE;
        SetCommState(hComm, &serialConfig);

        timeouts.ReadIntervalTimeout = 50;
        timeouts.ReadTotalTimeoutConstant = 100;
        timeouts.ReadTotalTimeoutMultiplier = 1;
        timeouts.WriteTotalTimeoutConstant = 50;
        timeouts.WriteTotalTimeoutMultiplier = 10;
        SetCommTimeouts(hComm, &timeouts);

        readLine(); //ignore first line (mostly crap)
    }
    vec getData()
    {
        std::string str = readLine();
        return getDataFromStr(str);
    }
    ~Port()
    {
        CloseHandle(hComm);
    }
};
#endif //SERIAL_READ_PORT_H
