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
	std::string tar_extract_temp(const std::string& archive_path) {
		const std::string tmp_dir (directory_get_temp() + file_plainname(file_basename(archive_path)));

		TAR* tar;
		if (tar_open(&tar, archive_path.c_str(), nullptr, O_RDONLY, 0644, TAR_GNU) == -1) {
			std::cerr << "UTIL ARCHIVE Failed to open tarfile \"" << archive_path << "\" for extraction\n";
			return "";
		}

		char* dir_prefix = new char[tmp_dir.size()+1];
		strcpy(dir_prefix, tmp_dir.c_str());
		if (tar_extract_all(tar, dir_prefix) == -1) {
			delete[] dir_prefix;
			std::cerr << "UTIL ARCHIVE Failed to extract tarfile \"" << archive_path << "\"\n";
			return "";
		}
		delete[] dir_prefix;

		if (tar_close(tar) == -1) {
			std::cerr << "UTIL ARCHIVE Failed to close tarfile \"" << archive_path << "\" after extraction\n";
			return "";
		}

		return tmp_dir;
	}

	std::string xz_decompress_temp(const std::string& archive_path) {
		const std::string tmp_dir (directory_get_temp());

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
			return "";
		}

		lzma_action action = LZMA_RUN;
		std::stringstream input (file_get_contents(archive_path));
		std::stringstream output;
		uint8_t inbuf[BUFSIZ];
		uint8_t outbuf[BUFSIZ];

		stream.next_in = nullptr;
		stream.avail_in = 0;
		stream.next_out = outbuf;
		stream.avail_out = sizeof(outbuf);
		ret = LZMA_OK;
		while (ret == LZMA_OK) {
			if ((stream.avail_in == 0)&&(!input.eof())) {
				stream.next_in = inbuf;
				input.read(reinterpret_cast<char*>(inbuf), sizeof(inbuf));
				stream.avail_in = input.gcount();

				if (input.eof()) {
					action = LZMA_FINISH;
				}
			}

			ret = lzma_code(&stream, action);

			if ((stream.avail_out == 0)||(ret == LZMA_STREAM_END)) {
				output.write(reinterpret_cast<char*>(outbuf), sizeof(outbuf) - stream.avail_out);
				if (!output.good()) {
					ret = LZMA_PROG_ERROR;
					break;
				}
				stream.next_out = outbuf;
				stream.avail_out = sizeof(outbuf);
			}
		}
		lzma_end(&stream);

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
			return "";
		}

		std::string tmp_path (tmp_dir + file_basename(archive_path));
		if (file_extname(tmp_path) == ".xz") {
			tmp_path = file_plainname(tmp_path);
		}
		file_put_contents(tmp_path, output.str());

		return tmp_path;
	}
}}

#endif // BEE_UTIL_ARCHIVE
