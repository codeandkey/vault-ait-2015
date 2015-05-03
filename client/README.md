Vault cloud storage system
==========================

Purpose
-------

The Vault cloud storage system uses a client-side encrypted model to ensure file security locally and remotely.
The initial version of the system will only support a single Azure blob account representing the entire system storage. (for multiple users)

Encryption schemes
------------------

Vault uses AES-256 in GCM/GHASH mode for symmetric key encryption. This is used on all of the files to ensure security for the file content and integrity.
Vault uses RSA with a 2048-bit key for security, integrity, and authentication with regard to sharing transactions.

During initial configuration, a global user key is generated at random and stored locally.
This functions as the user password and is not encrypted. (and also stores no sensitive information)

The user's globalkey is used for all symmetric cryptography.

File header format
------------------

	[1 byte] File version
	[32 bytes] Owner ID
	[32 bytes] Group ID
	[32 bytes] File size
	[N bytes] File data
