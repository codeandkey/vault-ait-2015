#pragma once

#include <string>
#include <map>

namespace Vault {
	class IO_Interface {
	public:
		IO_Interface(std::map<std::string, std::string> key_map);
		~IO_Interface(void);

		virtual bool Upload(std::string localfile, std::string remotefile);
		virtual bool Download(std::string localfile, std::string remotefile);
	protected:
		std::map<std::string, std::string> key_map;
	};
}