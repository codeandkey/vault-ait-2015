#include "io.hpp"

#include <unistd.h>

using namespace Vault;

static std::string conf_file = "/usr/local/share/vault/vaultio.conf";

int main(int argc, char** argv) {
	if (argc < 2) {
		std::cout << "usage: vaultio <upload/download/list/delete> <localfile> <remotefile> [<username_override>]\n";
		return -1;
	}

	bool mode_upload, mode_download, mode_list, mode_delete;

	mode_upload = argv[1] == std::string("upload");
	mode_download = argv[1] == std::string("download");
	mode_list = argv[1] == std::string("list");
	mode_delete = argv[1] == std::string("delete");

	std::string localfile, remotefile;

	if (mode_upload || mode_download) {
		localfile = argv[2];
		remotefile = argv[3];
	}

	if (mode_delete) {
		remotefile = argv[2];
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

	if ((mode_upload || mode_download) && argc >= 5) {
		key_map["azure_container"] = argv[4];
	}

	if (mode_list && argc >= 3) {
		key_map["azure_container"] = argv[2];
	}

	if (mode_delete && argc >= 4) {
		key_map["azure_container"] = argv[3];
	}

	IO_Interface* interface = NULL;

	if (key_map["protocol"] == "azure") {
		interface = new IO_AzureInterface(key_map);
	} else {
		interface = NULL;
	}

	bool result = true;

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
	} else if (mode_delete) {
		result = interface->Delete(remotefile);
	} else {
		std::cout << "vaultio: unknown mode " << argv[1] << "\n";
		return -1;
	}

	if (!result) {
		std::cout << "vaultio: Interface returned failure.\nFormat : ";

		for (int i = 0; i < argc; i++) {
			std::cout << argv[i] << " ";
		}

		std::cout << "\n";

		std::cout << "localfile = " << localfile << "\n";
		std::cout << "remotefile = " << remotefile << "\n";
		std::cout << "container = " << key_map["azure_container"] << "\n";

		return -1;
	}

	return 0;
}
