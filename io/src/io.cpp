#include "io.hpp"

using namespace Vault;

static const std::string upload_file = "vaultio.conf";

int main(int argc, char** argv) {
	if (argc <= 4) {
		std::cout << "usage: vaultio <upload/download> <localfile> <remotefile> [<username_override>]\n";
		return 1;
	}

	bool upload = std::string(argv[1]) == "upload";
	std::string localfile = argv[2], remotefile = argv[3];

	/* Parse in the keys. */

	std::map<std::string, std::string> key_map;

	std::ifstream file(upload_file);

	if (argc >= 5) {
		key_map["azure_container"] = argv[4];
	}

	if (!file) {
		std::cout << "vaultio: Failed to open configuration file. [" + upload_file + "]\n";
		return 1;
	}

	while (file) {
		std::string key, value;

		std::getline(file, key, '=');
		std::getline(file, value);

		key_map[key] = value;
	}

	IO_Interface* interface = NULL;

	if (key_map["protocol"] == "azure") {
		interface = new IO_AzureInterface(key_map);
	} else {
		interface = NULL;
	}

	bool result = false;

	if (upload) {
		result = interface->Upload(localfile, remotefile);
	} else {
		result = interface->Download(localfile, remotefile);
	}

	if (!result) {
		std::cout << "vaultio: Interface returned failure.\n";
	} else {
		std::cout << "vaultio: Interface returned success.\n";
	}

	return 0;
}
