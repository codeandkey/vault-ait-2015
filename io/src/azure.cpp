#include "azure.hpp"

#include <iostream>

#include <was/storage_account.h>
#include <was/blob.h>
#include <cpprest/filestream.h>
#include <cpprest/containerstream.h>

using namespace Vault;

IO_AzureInterface::IO_AzureInterface(std::map<std::string, std::string> key_map) : IO_Interface(key_map) {}
IO_AzureInterface::~IO_AzureInterface(void) {}

bool IO_AzureInterface::Upload(std::string localfile, std::string remotefile) {
	std::string connection_string, account_name, account_key, container;

	try {
		account_name = key_map.at("azure_account_name");
		account_key = key_map.at("azure_account_key");
		container = key_map.at("azure_container");
	} catch (std::out_of_range& e) {
		std::cout << "[IO_AzureInterface::Upload] Key missing!\n";
		return false;
	}

	try {
		connection_string += "DefaultEndpointsProtocol=https;";
		connection_string += "AccountName=" + account_name;
		connection_string += ";AccountKey=" + account_key + ";";

		azure::storage::cloud_storage_account storage_account = azure::storage::cloud_storage_account::parse(connection_string);
		azure::storage::cloud_blob_client blob_client = storage_account.create_cloud_blob_client();
		azure::storage::cloud_blob_container blob_container = blob_client.get_container_reference(container);

		blob_container.create_if_not_exists();

		azure::storage::blob_container_permissions permissions;
		permissions.set_public_access(azure::storage::blob_container_public_access_type::blob);
		blob_container.upload_permissions(permissions);

		concurrency::streams::istream input_stream = concurrency::streams::file_stream<uint8_t>::open_istream(localfile).get();
		azure::storage::cloud_block_blob blob = blob_container.get_block_blob_reference(remotefile);

		blob.upload_from_stream(input_stream);
		input_stream.close().wait();
	} catch (const azure::storage::storage_exception& e) {
		std::cout << "[IO_AzureInterface::Upload] azure::storage::storage_exception thrown : " << e.what() << "\n";

		azure::storage::request_result req_result = e.result();
		azure::storage::storage_extended_error extended_error = req_result.extended_error();

		if (!extended_error.message().empty()){
			std::cout << "[IO_AzureInterface::Upload] extended error : " << extended_error.message() << "\n";
		}

		return false;
	} catch (const std::exception& e) {
		std::cout << "[IO_AzureInterface::Upload] std::exception thrown : " << e.what() << "\n";
		return false;
	}

	return true;
}

bool IO_AzureInterface::Download(std::string localfile, std::string remotefile) {
	std::string connection_string, account_name, account_key, container;

	try {
		account_name = key_map.at("azure_account_name");
		account_key = key_map.at("azure_account_key");
		container = key_map.at("azure_container");
	} catch (std::out_of_range& e) {
		std::cout << "[IO_AzureInterface::Download] Key missing!\n";
		return false;
	}

	try {
		connection_string += "DefaultEndpointsProtocol=https;";
		connection_string += "AccountName=" + account_name;
		connection_string += ";AccountKey=" + account_key + ";";

		azure::storage::cloud_storage_account storage_account = azure::storage::cloud_storage_account::parse(connection_string);
		azure::storage::cloud_blob_client blob_client = storage_account.create_cloud_blob_client();
		azure::storage::cloud_blob_container blob_container = blob_client.get_container_reference(container);

		blob_container.create_if_not_exists();

		azure::storage::blob_container_permissions permissions;
		permissions.set_public_access(azure::storage::blob_container_public_access_type::blob);
		blob_container.upload_permissions(permissions);

		concurrency::streams::ostream output_stream = concurrency::streams::file_stream<uint8_t>::open_ostream(localfile).get();
		azure::storage::cloud_block_blob blob = blob_container.get_block_blob_reference(remotefile);

		blob.download_to_stream(output_stream);
		output_stream.close().wait();
	} catch (const azure::storage::storage_exception& e) {
		std::cout << "[IO_AzureInterface::Download] azure::storage::storage_exception thrown : " << e.what() << "\n";

		azure::storage::request_result req_result = e.result();
		azure::storage::storage_extended_error extended_error = req_result.extended_error();

		if (!extended_error.message().empty()){
			std::cout << "[IO_AzureInterface::Download] extended error : " << extended_error.message() << "\n";
		}

		return false;
	} catch (const std::exception& e) {
		std::cout << "[IO_AzureInterface::Download] std::exception thrown : " << e.what() << "\n";
		return false;
	}

	return true;
}

std::vector<std::string> IO_AzureInterface::List(void) {
	std::string connection_string, account_name, account_key, container;
	std::vector<std::string> output;

	output.push_back("<starting directory>");

	try {
		account_name = key_map.at("azure_account_name");
		account_key = key_map.at("azure_account_key");
		container = key_map.at("azure_container");
	} catch (std::out_of_range& e) {
		std::cout << "[IO_AzureInterface::List] Key missing!\n";
		return std::vector<std::string>();
	}

	try {
		connection_string += "DefaultEndpointsProtocol=https;";
		connection_string += "AccountName=" + account_name;
		connection_string += ";AccountKey=" + account_key + ";";

		azure::storage::cloud_storage_account storage_account = azure::storage::cloud_storage_account::parse(connection_string);
		azure::storage::cloud_blob_client blob_client = storage_account.create_cloud_blob_client();
		azure::storage::cloud_blob_container blob_container = blob_client.get_container_reference(container);

		blob_container.create_if_not_exists();

		azure::storage::continuation_token token;

		do {
			azure::storage::blob_result_segment result = blob_container.list_blobs_segmented(token);
			std::vector<azure::storage::cloud_blob> blobs = result.blobs();

			for (std::vector<azure::storage::cloud_blob>::const_iterator it = blobs.cbegin(); it != blobs.cend(); ++it) {
				output.push_back(it->uri().path());
			}

			token = result.continuation_token();
		} while (!token.empty());
	} catch (const azure::storage::storage_exception& e) {
		std::cout << "[IO_AzureInterface::List] azure::storage::storage_exception thrown : " << e.what() << "\n";

		azure::storage::request_result req_result = e.result();
		azure::storage::storage_extended_error extended_error = req_result.extended_error();

		if (!extended_error.message().empty()){
			std::cout << "[IO_AzureInterface::List] extended error : " << extended_error.message() << "\n";
		}

		return std::vector<std::string>();
	} catch (const std::exception& e) {
		std::cout << "[IO_AzureInterface::Download] std::exception thrown : " << e.what() << "\n";
		return std::vector<std::string>();
	}

	return output;
}
