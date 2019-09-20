#pragma once
#include <string>
#include <vector>

class Resource
{
public:
    Resource(const std::string& resName);
    const std::string& Data() const;

private:
	std::string mData;

	bool readFromFile(const std::string& resName);
	bool readFromArchive(const std::string& resName);
};