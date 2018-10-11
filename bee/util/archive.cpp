/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_UTIL_ARCHIVE
#define BEE_UTIL_ARCHIVE 1

// Archive handling functions

#include <iostream>
#include <sstream>
#include <cstring>
#include <fcntl.h>

#include <lzma.h>
#include <libtar.h>

#include "archive.hpp"

#include "files.hpp"

namespace util { namespace archive {
	namespace internal {
		const int lzma_compression_preset = 6;

		lzma_ret lzma_code(lzma_stream*, std::stringstream*, std::stringstream*);
	}
	/**
	* Create a Tar archive from a directory.
	* @param directory the directory path to archive
	* @param archive_path the file path to store the archive at
	*
	* @retval 0 success
	* @retval 1 failed to open the new tar archive
	* @retval 2 failed to add the directory to the archive
	* @retval 3 failed to close the tar archive
	*/
	int tar_create(const std::string& directory, const std::string& archive_path) {
		TAR* tar;
		if (tar_open(&tar, archive_path.c_str(), nullptr, O_WRONLY | O_CREAT, 0644, TAR_GNU) == -1) {
			std::cerr << "UTIL ARCHIVE Failed to open tarfile \"" << archive_path << "\" for writing\n";
			return 1;
		}

		char* dir_prefix = new char[directory.size()+1];
		strcpy(dir_prefix, directory.c_str());
		char savedir[] = "";
		if (tar_append_tree(tar, dir_prefix, savedir) == -1) {
			delete[] dir_prefix;
			std::cerr << "UTIL ARCHIVE Failed to create tarfile \"" << archive_path << "\"\n";
			return 2;
		}
		delete[] dir_prefix;

		if (tar_close(tar) == -1) {
			std::cerr << "UTIL ARCHIVE Failed to close tarfile \"" << archive_path << "\" after writing\n";
			return 3;
		}

		return 0;
	}
	/**
	* Archive a directory as a Tarfile.
	* @note The archive path will be directory+".tar"
	* @param directory the directory path to archive
	*
	* @see tar_create(const std::string&, const std::string&) for return values
	*/
	int tar_create(const std::string& directory) {
		std::string archive (directory);
		if (archive.back() == '/') {
			archive = archive.substr(0, archive.length()-1);
		}
		archive += ".tar";

		return tar_create(directory, archive);
	}
	/**
	* Extract the Tarfile to a directory.
	* @param archive_path the path of the tarfile
	* @param directory the directory path to store the extracted files at
	*
	* @retval 0 success
	* @retval 1 failed to open tarfile
	* @retval 2 failed to extract files from the archive
	* @retval 3 failed to close tarfile
	*/
	int tar_extract(const std::string& archive_path, const std::string& directory) {
		TAR* tar;
		if (tar_open(&tar, archive_path.c_str(), nullptr, O_RDONLY, 0644, TAR_GNU) == -1) {
			std::cerr << "UTIL ARCHIVE Failed to open tarfile \"" << archive_path << "\" for extraction\n";
			return 1;
		}

		char* dir_prefix = new char[directory.size()+1];
		strcpy(dir_prefix, directory.c_str());
		if (tar_extract_all(tar, dir_prefix) == -1) {
			delete[] dir_prefix;
			std::cerr << "UTIL ARCHIVE Failed to extract tarfile \"" << archive_path << "\"\n";
			return 2;
		}
		delete[] dir_prefix;

		if (tar_close(tar) == -1) {
			std::cerr << "UTIL ARCHIVE Failed to close tarfile \"" << archive_path << "\" after extraction\n";
			return 3;
		}

		return 0;
	}
	/**
	* Extract an archived directory.
	* @note The name of the directory will be the archive path without the ".tar" extension
	* @param archive_path the path of the tarfile
	*
	* @retval -1 invalid archive extension
	* @see tar_extract(const std::string&, const std::string&) for other return values
	*/
	int tar_extract(const std::string& archive_path) {
		if (file_extname(archive_path) == ".tar") {
			return tar_extract(archive_path, file_plainname(archive_path));
		}
		return -1;
	}
	/**
	* Extract an archived directory to the temp directory.
	* @note The extracted directory path will be "/tmp/bee-XXXXX/" + archive_name without the ".tar" extension
	* @param archive_path the path of the tarfile
	*
	* @returns the name of the extracted directory or an empty string on failure
	*/
	std::string tar_extract_temp(const std::string& archive_path) {
		const std::string tmp_dir (directory_get_temp() + file_plainname(file_basename(archive_path)));
		if (tar_extract(archive_path, tmp_dir) != 0) {
			return "";
		}
		return tmp_dir;
	}

	/**
	* Process the LZMA stream with the given input and output.
	* @note This function is used by both xz_compress() and xz_decompress()
	* @param stream the LZMA stream for compression or decompression, initialized elsewhere
	* @param input the input stream
	* @param output the output stream
	*
	* @returns the LZMA return value from ::lzma_code()
	*/
	lzma_ret internal::lzma_code(lzma_stream* stream, std::stringstream* input, std::stringstream* output) {
		lzma_action action = LZMA_RUN;
		uint8_t inbuf[BUFSIZ];
		uint8_t outbuf[BUFSIZ];

		stream->next_in = nullptr;
		stream->avail_in = 0;
		stream->next_out = outbuf;
		stream->avail_out = sizeof(outbuf);
		lzma_ret ret = LZMA_OK;
		while (ret == LZMA_OK) {
			if ((stream->avail_in == 0)&&(input->good())) {
				stream->next_in = inbuf;
				input->read(reinterpret_cast<char*>(inbuf), sizeof(inbuf));
				stream->avail_in = input->gcount();

				if (input->eof()) {
					action = LZMA_FINISH;
				}
			}

			ret = lzma_code(stream, action);

			if ((stream->avail_out == 0)||(ret == LZMA_STREAM_END)) {
				output->write(reinterpret_cast<char*>(outbuf), sizeof(outbuf) - stream->avail_out);
				if (!output->good()) {
					ret = LZMA_PROG_ERROR;
					break;
				}
				stream->next_out = outbuf;
				stream->avail_out = sizeof(outbuf);
			}
		}
		lzma_end(stream);

		return ret;
	}
	/**
	* Compress the given file using LZMA.
	* @param file_path the path of the uncompressed file
	* @param archive_path the path of the compressed archive
	*
	* @retval 0 success
	* @retval 1 failed to initialize encoder
	* @retval 2 failed to encode stream
	*/
	int xz_compress(const std::string& file_path, const std::string& archive_path) {
		lzma_stream stream (LZMA_STREAM_INIT);
		lzma_ret ret = lzma_easy_encoder(&stream, internal::lzma_compression_preset, LZMA_CHECK_CRC64);
		if (ret != LZMA_OK) {
			std::cerr << "UTIL ARCHIVE Failed to initialize LZMA stream encoder for \"" << file_path << "\":";
			switch (ret) {
				case LZMA_MEM_ERROR: {
					std::cerr << "Memory allocation failed";
					break;
				}
				case LZMA_OPTIONS_ERROR: {
					std::cerr << "Unsupported preset " << internal::lzma_compression_preset;
					break;
				}
				case LZMA_UNSUPPORTED_CHECK: {
					std::cerr << "Unsupported integrity check";
					break;
				}
				default: {
					std::cerr << "Unknown error";
				}
			}
			std::cerr << "\n";
			return 1;
		}

		std::stringstream input (file_get_contents(file_path));
		std::stringstream output;
		ret = internal::lzma_code(&stream, &input, &output);

		if (ret != LZMA_STREAM_END) {
			std::cerr << "UTIL ARCHIVE Failed to read LZMA stream for \"" << archive_path << "\":";
			switch (ret) {
				case LZMA_MEM_ERROR: {
					std::cerr << "Memory allocation failed";
					break;
				}
				case LZMA_DATA_ERROR: {
					std::cerr << "File size limits exceeded";
					break;
				};
				default: {
					std::cerr << "Unknown error";
				}
			}
			std::cerr << "\n";
			return 2;
		}

		file_put_contents(archive_path, output.str());

		return 0;
	}
	/**
	* Compress the given file using LZMA.
	* @note The compressed archive path will be file_path+".xz"
	* @param file_path the path of the uncompressed file
	*
	* @see xz_compress(const std::string&, const std::string&) for return values
	*/
	int xz_compress(const std::string& file_path) {
		return xz_compress(file_path, file_path+".xz");
	}
	/**
	* Decompress the given archive using LZMA.
	* @param archive_path the path of the compressed archive
	* @param file_path the path of the uncompressed file
	*
	* @retval 0 success
	* @retval 1 failed to initialize decoder
	* @retval 2 failed to decode stream
	*/
	int xz_decompress(const std::string& archive_path, const std::string& file_path) {
		lzma_stream stream (LZMA_STREAM_INIT);
		lzma_ret ret = lzma_stream_decoder(&stream, UINT64_MAX, LZMA_CONCATENATED);
		if (ret != LZMA_OK) {
			std::cerr << "UTIL ARCHIVE Failed to initialize LZMA stream decoder for \"" << archive_path << "\":";
			switch (ret) {
				case LZMA_MEM_ERROR: {
					std::cerr << "Memory allocation failed";
					break;
				}
				case LZMA_OPTIONS_ERROR: {
					std::cerr << "Unsupported decompression flags";
					break;
				}
				default: {
					std::cerr << "Unknown error";
				}
			}
			std::cerr << "\n";
			return 1;
		}

		std::stringstream input (file_get_contents(archive_path));
		std::stringstream output;
		ret = internal::lzma_code(&stream, &input, &output);

		if (ret != LZMA_STREAM_END) {
			std::cerr << "UTIL ARCHIVE Failed to read LZMA stream for \"" << archive_path << "\":";
			switch (ret) {
				case LZMA_MEM_ERROR: {
					std::cerr << "Memory allocation failed";
					break;
				}
				case LZMA_FORMAT_ERROR: {
					std::cerr << "Not in the .xz format";
					break;
				}
				case LZMA_OPTIONS_ERROR: {
					std::cerr << "Unsupported decompression flags";
					break;
				}
				case LZMA_DATA_ERROR: {
					std::cerr << "Corrupt file";
					break;
				};
				case LZMA_BUF_ERROR: {
					std::cerr << "Truncated file";
					break;
				};
				default: {
					std::cerr << "Unknown error";
				}
			}
			std::cerr << "\n";
			return 2;
		}

		file_put_contents(file_path, output.str());

		return 0;
	}
	/**
	* Decompress the given archive using LZMA.
	* @note The uncompressed file path will be the archive path without the ".xz" extension
	* @param archive_path the path of the compressed archive
	*
	* @retval -1 invalid archive extension
	* @see xz_decompress(const std::string&, const std::string&) for other return values
	*/
	int xz_decompress(const std::string& archive_path) {
		if (file_extname(archive_path) == ".xz") {
			return xz_decompress(archive_path, file_plainname(archive_path));
		}
		return -1;
	}
	/**
	* Decompress a compressed archive to the temporary directory.
	* @note The uncompressed file path will be "/tmp/bee-XXXXX/" + archive_name without the ".xz" extension
	* @param archive_path the path of the compressed archive
	*
	* @returns the uncompressed file path or an empty string on failure
	*/
	std::string xz_decompress_temp(const std::string& archive_path) {
		std::string tmp_path (directory_get_temp() + file_basename(archive_path));
		if (file_extname(tmp_path) == ".xz") {
			tmp_path = file_plainname(tmp_path);
		}

		if (xz_decompress(archive_path, tmp_path) != 0) {
			return "";
		}
		return tmp_path;
	}
}}

#endif // BEE_UTIL_ARCHIVE
