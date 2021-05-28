#pragma once


#include <vector>
#include <tuple>
#include <string>






class Translator {
public:
	void OnNewFrame();
	void NewOutMessage(int nEntityIndex, std::string sChatMessage);
	void OnDispatchUserEvent(unsigned int nSize, const void* pData);
private:




	std::vector<std::tuple<int, std::string>> m_vOutMessages;
	std::vector<std::tuple<int, std::string>> m_vInMessages;
};