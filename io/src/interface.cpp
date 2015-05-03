#include "interface.hpp"

#include <iostream>

using namespace Vault;

IO_Interface::IO_Interface(std::map<std::string, std::string> keys) : key_map(keys) {}
IO_Interface::~IO_Interface(void) {}

bool IO_Interface::Upload(std::string localfile, std::string remotefile) {
	std::cout << "[IO_Interface::Upload] Calling the parent class, will not do anything!\n";
	return false;
}

bool IO_Interface::Download(std::string localfile, std::string remotefile) {
	std::cout << "[IO_Interface::Download] Calling the parent class, will not do anything!\n";
	return false;
}

std::vector<std::string> IO_Interface::List(void) {
	std::cout << "[IO_Interface::List] Calling the parent class, will not do anything!\n";
	return std::vector<std::string>();
}
