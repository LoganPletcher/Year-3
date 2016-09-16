#include "TXTFileReader.h"
#include <fstream>
#include <iostream>

txtfilereader::txtfilereader()
{

}

std::string txtfilereader::loadFile(char fileName[])
{
	std::ifstream f;
	
	std::string fileData, FullData;
	
	f.open(fileName, std::ios::in | std::ios::_Nocreate);
	if (f.is_open())
	{
		while (!f.eof())
		{
			std::getline(f, fileData);
			FullData += "\n" + fileData;
		}
	
	}	
	
	f.close();	
	
	return FullData;
}