#ifndef  PORT_H
#define  PORT_H
#include <cstdlib>
#include <windows.h>
#include <string>
#include <ios> //ios::exception
#include <vector>

template <typename dataType>
class Port
{
    typedef std::vector <dataType> vec;
    HANDLE hComm;
    DCB serialConfig = {0};
    COMMTIMEOUTS timeouts = {0};

    std::string readLine()
    {
        char tempChar;
        std::string buffer;
        DWORD countBytesRead = 0;
        do
        {
            BOOL success = ReadFile(
					 hComm,             //file handle
                     &tempChar,         //char buffer
                     sizeof(tempChar),  //number of bytes to read
                     &countBytesRead,   //number of bytes currently read
                     nullptr            //lpOverlapped https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-readfile
            );
			if (!success)
				throw(std::ios_base::failure("Reading port failed"));
            buffer += tempChar;
        }while(tempChar != '\n');
        return buffer;
    }
    vec getDataFromStr(const std::string& str)
    {
        vec result;
        auto i = str.begin();

        auto j = i;
        while(true)
        {
            while(*j != '\n' && *j !=' ')
                j++;
            std::string tmpStr = str.substr(i - str.begin(), j - i);
			dataType data;
			try 
			{ 
				data = static_cast<dataType>(std::stod(tmpStr)); 
			}
			catch (std::bad_cast & exc)
			{
				throw exc;
			}
            result.push_back(data);

			if (*j == '\n')
				return result;
			j++;
            i = j;
        }
    }
public:
	Port(const std::string& portName, DWORD baudRate = CBR_9600)
	{
		this->Port(baudRate);
		this->init(portName);
	}
	Port(DWORD baudRate = CBR_9600)
	{
		serialConfig.DCBlength = sizeof(serialConfig);
		serialConfig.BaudRate = baudRate;
		serialConfig.ByteSize = 8;
		serialConfig.StopBits = ONESTOPBIT;
		serialConfig.Parity = NOPARITY;
		serialConfig.fRtsControl = RTS_CONTROL_ENABLE;

		timeouts.ReadIntervalTimeout = 50;
		timeouts.ReadTotalTimeoutConstant = 100;
		timeouts.ReadTotalTimeoutMultiplier = 1;
		timeouts.WriteTotalTimeoutConstant = 50;
		timeouts.WriteTotalTimeoutMultiplier = 10;
	}
    void init(const std::string portName = "COM3")
    {
        hComm = CreateFileA(
                portName.data(),	//port name (char* required)
                GENERIC_READ,		//read only
                0,					//share mode (0 ==> cannot be shared)
                nullptr,			//security attributes (nullptr ==> handle cannot be inherited) http://winapi.freetechsecrets.com/win32/WIN32CreateFile.htm
                OPEN_EXISTING,		//creation distribution (open only if existing)
                0,					//flags and attributes (link ^)
                nullptr				//handle to files to copy (none)
        );
        if (hComm == INVALID_HANDLE_VALUE)
        {
            std::string errMsg = "Cannot open port ";
            errMsg += portName;
            throw(std::ios_base::failure(errMsg));
        }

		SetCommState(hComm, &serialConfig);
		SetCommTimeouts(hComm, &timeouts);

        std::string tmp = readLine(); //ignore first line (mostly crap)
    }
    vec getData()
    {
		std::string str;
		try
		{
			str = readLine();
		}
		catch (std::exception & exc)
		{
			throw exc;
		}
		vec result;
		try
		{
			result = getDataFromStr(str);
		}
		catch (std::exception & exc)
		{
			throw exc;
		}
		return result;
    }
	void findPort(unsigned int portCount = 25)
	{
		for (unsigned int i = 0; i < portCount; i++)
		{

			std::string name = "COM";
			name += std::to_string(i);
			try
			{
				this->init(name);
			}
			catch (...)
			{
				continue;
			}
			return;
		}
		throw(std::ios_base::failure("Failed to find a port."));
	}
    ~Port()
    {
        CloseHandle(hComm);
    }
};
#endif //PORT_H
