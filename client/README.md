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
		user_groups
		public_key
		<groupnameN/filenameN>

user _ info format
------------------

	[32 bytes] Update timestamp
	[4 bytes] Group count
	[32 bytes * N] Group names

The group count and group names list indicates the groups which this user OWNS.
Groups which the user belong to are listed in the user _ groups file.

user _ groups format (simply a list of groups the user belongs to)
--------------------
	<username>/<groupname>

Group file format
-----------------

	username/groupname/group_version [32 bytes] Group version timestamp [4 bytes] Group version ID, incremented with each modification
	username/groupname/group_version.sig [signature]
	username/groupname/group_list [newline-separated list of all group users]
	username/groupname/group_list.sig [signature]
	username/groupname/key/memberusername [512] User key, encrypted with memberusername's public
	username/groupname/key/memberusername.sig [Signature of user key to verify authenticity]
