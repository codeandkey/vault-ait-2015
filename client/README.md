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

During initial configuration, a root group is created with the owner as the only user.
The root group cannot be modified.

File header format
------------------

	[1 byte] File version
	[32 bytes] Owner ID
	[32 bytes] Group ID
	[32 bytes] Unused
	[32 bytes] File size
	[N bytes] File data

Server-side user directory format
---------------------------------

	username/
		user_info
		public_key
		<user files>
		<group files>

Users files are prefixed with "file-"
Group files are prefixed with "group-"

user _ info format
------------------

	[32 bytes] Update timestamp
	[4 bytes] Group count
	[32 bytes * N] Group names

Group file format
-----------------

	[4 bytes] Group version ID
	[32 bytes] Owner username
	[4 bytes] User count
	[32 bytes * N] User names
	[32 bytes * N] User keys (encrypted with respective user public keys, then signed with owner private)
