
#pragma once // include the current header file only once during a single compilation (translation unit)



#include<string>
#include<fstream>
#include<stdexcept>
#include<iterator>



class TextFile {
public:
    std::string content; 


    //Throws std::runtime_error if the file cannot be opened or read
    explicit TextFile(std::string const& filePath) {
        std::ifstream file(filePath);
        if (!file)  //checks if an error has occurred (synonym of fail())
            throw std::runtime_error("Could not open '" + filePath + "'");

        //we use an interator to read all characters
        content = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());

        if (!file)  
            throw std::runtime_error("An I/O error occurred while reading '" + filePath + "'");
    }
};