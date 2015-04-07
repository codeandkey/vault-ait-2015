#include "vault_azure.hpp"

#include <was/storage_account.h>
#include <was/blob.h>
#include <cpprest/filestream.h>
#include <cpprest/containerstream.h>

using namespace Vault;

AzureUploadRequest::AzureUploadRequest(void) {}
AzureUploadRequest::~AzureUploadRequest(void) {}

void AzureUploadRequest::SetParameters(std::map<std::string, std::string> target, std::string local, std::string remote) {
	account_name = target["account_name"];
	account_key = target["account_key"];
	account_container = target["account_container"];

	localfile = local;
	remotefile = remote;
}

bool AzureUploadRequest::Start(void) {
	std::string connection_string= "";

	connection_string += "DefaultEndpointsProtocol=https;"; /* Force HTTPS. */
	connection_string += "AccountName=";
	connection_string += account_name;
	connection_string += ";AccountKey=";
	connection_string += account_key;
	connection_string += ";";

	bool result = true;

	std::cout << "[AzureUploadRequest] Starting upload function.\n";

	try {
		azure::storage::cloud_storage_account storage_account = azure::storage::cloud_storage_account::parse(connection_string);
		azure::storage::cloud_blob_client blob_client = storage_account.create_cloud_blob_client();
		azure::storage::cloud_blob_container blob_container = blob_client.get_container_reference(account_container);

		blob_container.create_if_not_exists();

		azure::storage::blob_container_permissions permissions;
		permissions.set_public_access(azure::storage::blob_container_public_access_type::blob);
		blob_container.upload_permissions(permissions);

		concurrency::streams::istream input_stream = concurrency::streams::file_stream<uint8_t>::open_istream(localfile).get();
		azure::storage::cloud_block_blob blob1 = blob_container.get_block_blob_reference(remotefile);
		blob1.upload_from_stream(input_stream);
		input_stream.close().wait();
	} catch (const azure::storage::storage_exception& e) {
		std::cout << "[AzureUploadRequest] azure::storage::storage_exception thrown: " << e.what() << "\n";

		azure::storage::request_result req_result = e.result();
		azure::storage::storage_extended_error extended_error = req_result.extended_error();

		if (!extended_error.message().empty()) {
			std::cout << "[AzureUploadRequest] Extended error message : " << extended_error.message() << "\n";
		}

		result = false;
	} catch (const std::exception& e) {
		std::cout << "[AzureUploadRequest] std::exception thrown: " << e.what() << "\n";

		result = false;
	}

	std::cout << "[AzureUploadRequest] Finished upload function.\n";
	return result;
}

bool AzureUploadRequest::GetStatus(void) { return status; }
bool AzureUploadRequest::GetComplete(void) { return completed; }

AzureDownloadRequest::AzureDownloadRequest(void) {}
AzureDownloadRequest::~AzureDownloadRequest(void) {}

void AzureDownloadRequest::SetParameters(std::map<std::string, std::string> target, std::string local, std::string remote) {
	account_name = target["account_name"];
	account_key = target["account_key"];
	account_container = target["account_container"];

	localfile = local;
	remotefile = remote;
}

bool AzureDownloadRequest::Start(void) {
	std::string connection_string= "", container_name = "";

	connection_string += "DefaultEndpointsProtocol=https;";
	connection_string += "AccountName=vaultstorage;";
	connection_string += "AccountKey=zKzlJ5lWlcjV89F8gBKXVYJILMl1xYc/MQkcHVklp8Uj9199hy4ibF8gv7yVLfCwzgLMtfd88tczRv60hiSVrQ==;";

	container_name = "hello-world";

	bool result = true;

	std::cout << "[AzureDownloadRequest] Starting upload function.\n";

	try {
		azure::storage::cloud_storage_account storage_account = azure::storage::cloud_storage_account::parse(connection_string);
		azure::storage::cloud_blob_client blob_client = storage_account.create_cloud_blob_client();
		azure::storage::cloud_blob_container blob_container = blob_client.get_container_reference(container_name);

		blob_container.create_if_not_exists();

		azure::storage::blob_container_permissions permissions;
		permissions.set_public_access(azure::storage::blob_container_public_access_type::blob);
		blob_container.upload_permissions(permissions);

		concurrency::streams::istream input_stream = concurrency::streams::file_stream<uint8_t>::open_istream(localfile).get();
		azure::storage::cloud_block_blob blob1 = blob_container.get_block_blob_reference(remotefile);
		blob1.upload_from_stream(input_stream);
		input_stream.close().wait();
	} catch (const azure::storage::storage_exception& e) {
		std::cout << "[AzureDownloadRequest] azure::storage::storage_exception thrown: " << e.what() << "\n";

		azure::storage::request_result req_result = e.result();
		azure::storage::storage_extended_error extended_error = req_result.extended_error();

		if (!extended_error.message().empty()) {
			std::cout << "[AzureDownloadRequest] Extended error message : " << extended_error.message() << "\n";
		}

		result = false;
	} catch (const std::exception& e) {
		std::cout << "[AzureDownloadRequest] std::exception thrown: " << e.what() << "\n";

		result = false;
	}

	std::cout << "[AzureDownloadRequest] Finished download function.\n";
	return result;
}
