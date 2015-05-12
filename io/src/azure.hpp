#pragma once

/* The Azure interface uses 3 keys :
 * 	- account_name : The storage account name.
 *	- account_key  : The storage accont key.
 *	- account_container : The storage account container.
 */

#include "interface.hpp"

namespace Vault {
	class IO_AzureInterface : public IO_Interface {
	public:
		IO_AzureInterface(std::map<std::string, std::string>);
		~IO_AzureInterface(void);

		bool Upload(std::string localfile, std::string remotefile);
		bool Download(std::string localfile, std::string remotefile);
		std::vector<std::string> List(void);
		bool Delete(std::string remotefile);
	};
}
