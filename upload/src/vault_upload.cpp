#include "vault_upload.hpp"

using namespace Vault;

int main(int argc, char** argv) {
	AzureUploadRequest tr;

	/* We find the configuration file for the uploader and parse out the keys. */

	std::map<std::string, std::string> confargs;

	std::ifstream conf("vault_upload.conf");

	if (!conf) {
		std::cout << "[main] Failed to open configuration file (vault_upload.conf)\n";
		return 1;
	}

	while (!conf.eof()) {
		std::string line;
		std::getline(conf, line);

		if (!line.length()) {
			break;
		}

		std::string key = line.substr(0, line.find("="));
		line.erase(0, line.find("=") + 1);

		confargs[key] = line;

		std::cout << "[main] Pushing key " << key << ", value " << line << "\n";
	}

	if (argc < 3) {
		std::cout << "[main] Usage: vaultupload <localfile> <remotefile>\n";
		return 1;
	}

	tr.SetParameters(confargs, argv[1], argv[2]);

	if (tr.Start()) {
		std::cout << "[main] Upload function reported success.\n";
	} else {
		std::cout << "[main] Upload function reported failure.\n";
	}

	return 0;
}
