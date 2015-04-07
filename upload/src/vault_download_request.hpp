#pragma once

#include <string>
#include <map>

namespace Vault {
	class DownloadRequest {
	public:
		virtual void SetParameters(std::map<std::string, std::string> argmap, std::string localfile, std::string remotefile) = 0;
		virtual bool Start(void) = 0;
	};
}
