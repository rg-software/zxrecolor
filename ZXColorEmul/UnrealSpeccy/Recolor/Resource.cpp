#include "Resource.h"
#include <fstream>
#include "zip.h"

// archive handling is base on kuba's zip library, see https://github.com/kuba--/zip
namespace Impl
{
	std::string RESOURCE_LOCATION = "game";
}

Resource::Resource(const std::string& resName)
{
	if(!readFromFile(resName) && !readFromArchive(resName))
		throw std::runtime_error("Resource " + resName + " not found");
}

const std::string& Resource::Data() const
{
	return mData;
}

bool Resource::readFromArchive(const std::string& resName)
{
	void *buf = NULL;
	size_t bufsize;

	zip_t *zip = zip_open((Impl::RESOURCE_LOCATION + ".zip").c_str(), 0, 'r');
	{
		zip_entry_open(zip, resName.c_str());
		{
			if (zip_entry_read(zip, &buf, &bufsize) == -1)
				return false;
			mData.assign((const char*)buf, bufsize);
		}
		zip_entry_close(zip);
	}
	zip_close(zip);
	free(buf);

	return true;
}

bool Resource::readFromFile(const std::string& resName)
{
	std::ifstream file(Impl::RESOURCE_LOCATION + "\\" + resName, std::ios::binary);
	if (!file)
		return false;

	mData.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
	return true;
}
