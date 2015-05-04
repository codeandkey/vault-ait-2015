#include "io.hpp"

#include <unistd.h>

using namespace Vault;

static std::string conf_file = std::string(getenv("HOME")) + "/.vaultio.conf";

int main(int argc, char** argv) {
	if (argc < 2) {
		std::cout << "usage: vaultio <upload/download/list> <localfile> <remotefile> [<username_override>]\n";
		return -1;
	}

	bool mode_upload, mode_download, mode_list;

	mode_upload = argv[1] == std::string("upload");
	mode_download = argv[1] == std::string("download");
	mode_list = argv[1] == std::string("list");

	std::string localfile, remotefile;

	if (mode_upload || mode_download) {
		localfile = argv[2];
		remotefile = argv[3];
	}

	/* Parse in the keys. */

	std::map<std::string, std::string> key_map;

	std::ifstream file(conf_file);

	if (!file) {
		std::cout << "vaultio: Failed to open configuration file. [" + conf_file + "]\n";
		return -1;
	}

	while (file) {
		std::string key, value;

		std::getline(file, key, '=');
		std::getline(file, value);

		key_map[key] = value;
	}

	if (!mode_list && argc >= 5) {
		key_map["azure_container"] = argv[4];
		std::cout << "Overriding with container " << argv[4] << "\n";
	}

	if (mode_list && argc >= 3) {
		key_map["azure_container"] = argv[2];
		std::cout << "Overriding with container " << argv[4] << "\n";
	}

	IO_Interface* interface = NULL;

	if (key_map["protocol"] == "azure") {
		interface = new IO_AzureInterface(key_map);
	} else {
		interface = NULL;
	}

	bool result = false;

	if (mode_upload) {
		result = interface->Upload(localfile, remotefile);
	} else if (mode_download) {
		result = interface->Download(localfile, remotefile);
	} else if (mode_list) {
		std::vector<std::string> lresult = interface->List();
		result = lresult.size();

		for (auto it = lresult.begin(); it != lresult.end(); ++it) {
			std::cout << "$ " << *it << "\n";
		}
	} else {
		std::cout << "vaultio: unknown mode " << argv[1] << "\n";
		return -1;
	}

	if (!result) {
		std::cout << "vaultio: Interface returned failure.\n";
		return -1;
	} else {
		std::cout << "vaultio: Interface returned success.\n";
	}

	return 0;
}
