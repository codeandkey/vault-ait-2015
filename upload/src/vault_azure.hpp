#pragma once

/*
 * This file contains declarations for the Azure-based uploading system.
 * The Azure C++ API is based on insane lambda usage, so this code might get nasty.
 */

#include <cpprest/http_client.h>
#include <cpprest/filestream.h>

#include "vault_upload_request.hpp"
#include "vault_download_request.hpp"

namespace Vault {
	class AzureUploadRequest : public UploadRequest {
	public:
		AzureUploadRequest(void);
		~AzureUploadRequest(void);

		void SetParameters(std::map<std::string, std::string> argmap, std::string localfile, std::string remotefile);

		bool Start(void);

		bool GetStatus(void);
		bool GetComplete(void);
	private:
		bool status, completed;

		std::string localfile, remotefile;
		std::string account_name, account_key, account_container;
	};

	class AzureDownloadRequest : public DownloadRequest {
	public:
		AzureDownloadRequest(void);
		~AzureDownloadRequest(void);

		void SetParameters(std::map<std::string, std::string>, std::string localfile, std::string remotefile);

		bool Start(void);
	private:
		bool status, completed;

		std::string localfile, remotefile;
		std::string account_name, account_key, account_container;
	};
}
