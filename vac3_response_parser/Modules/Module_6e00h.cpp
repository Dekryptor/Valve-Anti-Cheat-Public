#include "ModuleParser.hpp"

#include "Blacklist.hpp"

bool Module6e00h::ParseOutput(int FunctionId, unsigned char *VacOut, unsigned long *VacOutSize, void *ParserData)
{
	if (FunctionId != 4)
		throw std::exception("FunctionID not implemented");

	return ParseFunction4(VacOut, VacOutSize, ParserData);
}

unsigned char* GetBlock(unsigned char* start, unsigned char* end, const std::string& blacklisted)
{
	//start must point to a starting block
	while (start < end)
	{
		auto _str = (char*)start + 0x28;
		std::string str = _str; //point to str

		if (str.find(blacklisted) != std::string::npos)
			return (unsigned char*)start;

		start += 0x40;
	}

	return nullptr;
}

void RemoveBlock(unsigned char *TestList, unsigned long TestListSize, unsigned char *Block)
{
	//Overwrite that shit
	auto tmpSize = (TestList + TestListSize) - (Block + 0x40);
	unsigned char *tmpCopy = new unsigned char[tmpSize];
	std::copy(Block + 0x40, TestList + TestListSize, tmpCopy);

	memset(Block, 0x00, 0x40);

	std::copy(tmpCopy, tmpCopy + tmpSize, Block);

	delete[] tmpCopy;
}

void Parser6e00(unsigned char *TestList, unsigned long TestListSize)
{
	auto Start = TestList + 0x40;
	unsigned char *ret = nullptr;
	unsigned int BlacklistIndex = 0;

	do
	{
		ret = GetBlock(Start, TestList + TestListSize, Blacklist6e00h[BlacklistIndex][0]);

		//Double blacklist
		if (ret && !Blacklist6e00h[BlacklistIndex][1].empty())
		{
			std::string _str = (char*)ret + 0x28;

			if (_str.find(Blacklist6e00h[BlacklistIndex][1]) != std::string::npos)
				RemoveBlock(TestList, TestListSize, ret);
		}
		else if (ret != nullptr)
		{
			RemoveBlock(TestList, TestListSize, ret);
		}
		else
			++BlacklistIndex;
	} while (BlacklistIndex < Blacklist6e00hMax);

}

bool Module6e00h::ParseFunction4(unsigned char *VacOut, unsigned long *VacOutSize, void *ParserData)
{
	memcpy(VacOut, ParserData, *VacOutSize);

	Parser6e00(VacOut, *VacOutSize);

	return true;
}
