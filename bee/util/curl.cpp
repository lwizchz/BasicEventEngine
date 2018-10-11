/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_UTIL_CURL
#define BEE_UTIL_CURL 1

// CURL functions

#include <iostream>
#include <cstring>

#include "curl.hpp"

#include "files.hpp"

namespace util { namespace curl {
	namespace internal {
		bool is_initialized = false;
		bool has_init_failed = false;
		bool is_verbose = false;

		long timeout = 10;
		std::string current_transfer;

		std::function<int (curl_off_t, curl_off_t)> progress_func = nullptr;

		size_t read(char*, size_t, size_t, void*);
		size_t write(void*, size_t, size_t, FILE*);
		size_t nowrite(void*, size_t, size_t, FILE*);
		int progress(void*, curl_off_t, curl_off_t, curl_off_t, curl_off_t);
	}
	size_t internal::read(char* buffer, size_t size, size_t nitems, void* userdata) {
		size_t bytes = size*nitems;
		if (current_transfer.size() < bytes) {
			bytes = current_transfer.size();
		}

		memcpy(buffer, current_transfer.c_str(), bytes);
		current_transfer = current_transfer.substr(bytes);
		return bytes;
	}
	size_t internal::write(void* ptr, size_t size, size_t nmemb, FILE* stream) {
		current_transfer += std::string(static_cast<char*>(ptr), nmemb);
		return nmemb;
	}
	size_t internal::nowrite(void* ptr, size_t size, size_t nmemb, FILE* stream) {
		return nmemb;
	}
	int internal::progress(void* clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow) {
		if (progress_func != nullptr) {
			if (dltotal != 0) {
				return progress_func(dltotal, dlnow);
			} else if (ultotal != 0) {
				return progress_func(ultotal, ulnow);
			}
		}
		return 0;
	}

	/**
	* Initialize the libcurl.
	*/
	int init() {
		if (internal::has_init_failed) {
			return -1;
		}

		int r = curl_global_init(CURL_GLOBAL_ALL);
		if (!r) {
			internal::is_initialized = true;
		} else {
			internal::has_init_failed = true;
		}
		return r;
	}
	/**
	* Download the given URL with a progress callback.
	* @param url the URL to attempt to download
	* @param dest the path at which to store the downloaded file
	* @param progress_func the function to call to update file download progress
	*
	* @retval -1 failed to initialize curl
	* @retval 0 success
	* @retval 1 failed to initialize the curl handle
	* @retval 2 failed to download file
	*/
	int download(const std::string& url, const std::string& dest, std::function<int (curl_off_t, curl_off_t)> progress_func) {
		if (!internal::is_initialized) {
			if (init()) {
				std::cerr << "UTIL CURL failed to init libcurl\n";
				return -1;
			}
		}

		CURL* curl = curl_easy_init();
		if (curl == nullptr) {
			std::cerr << "UTIL CURL failed to init handle\n";
			return 1;
		}

		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, internal::timeout);
		curl_easy_setopt(curl, CURLOPT_FAILONERROR, true);
		curl_easy_setopt(curl, CURLOPT_VERBOSE, internal::is_verbose);

		char* errorbuf = new char[CURL_ERROR_SIZE];
		curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorbuf);

		internal::current_transfer.clear();
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, internal::write);

		if (progress_func != nullptr) {
			curl_easy_setopt(curl, CURLOPT_NOPROGRESS, false);
			internal::progress_func = progress_func;
			curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, internal::progress);
		}

		CURLcode res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);

		std::string error (errorbuf);
		delete[] errorbuf;
		internal::progress_func = nullptr;

		if (res != CURLE_OK) {
			internal::current_transfer.clear();
			std::cerr << "UTIL CURL download(\"" << url << "\", \"" << dest << "\") : ";
			if (error.length()) {
				std::cerr << error;
			} else {
				std::cerr << curl_easy_strerror(res);
			}
			std::cerr << "\n";
			return 2;
		}

		file_put_contents(dest, internal::current_transfer);
		internal::current_transfer.clear();

		return 0;
	}
	/**
	* Download the given URL.
	* @param url the URL to attempt to download
	* @param dest the path at which to store the downloaded file
	*
	* @see download(const std::string&, const std::string&, std::function<int (curl_off_t, curl_off_t)>) for return values
	*/
	int download(const std::string& url, const std::string& dest) {
		return download(url, dest, nullptr);
	}
	/**
	* Upload the given file to a URL with a progress callback.
	* @param url the URL to attempt to upload to
	* @param src the path of the file to upload
	*
	* @retval -1 failed to initialize curl
	* @retval 0 success
	* @retval 1 failed to initialize the curl handle
	* @retval 2 failed to upload file
	*/
	int upload(const std::string& url, const std::string& src, std::function<int (curl_off_t, curl_off_t)> progress_func) {
		if (!internal::is_initialized) {
			if (init()) {
				std::cerr << "UTIL CURL failed to init libcurl\n";
				return -1;
			}
		}

		CURL* curl = curl_easy_init();
		if (curl == nullptr) {
			std::cerr << "UTIL CURL failed to init handle\n";
			return 1;
		}

		internal::current_transfer.clear();
		internal::current_transfer = file_get_contents(src);

		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, internal::timeout);
		curl_easy_setopt(curl, CURLOPT_FAILONERROR, true);
		curl_easy_setopt(curl, CURLOPT_VERBOSE, internal::is_verbose);
		if (!internal::is_verbose) {
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, internal::nowrite);
		}

		char* errorbuf = new char[CURL_ERROR_SIZE];
		curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorbuf);
		errorbuf[0] = '\0';

		curl_easy_setopt(curl, CURLOPT_UPLOAD, true);
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, internal::read);
		curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, internal::current_transfer.size());

		if (progress_func != nullptr) {
			curl_easy_setopt(curl, CURLOPT_NOPROGRESS, false);
			internal::progress_func = progress_func;
			curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, internal::progress);
		}

		CURLcode res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);

		std::string error (errorbuf);
		delete[] errorbuf;
		internal::current_transfer.clear();
		internal::progress_func = nullptr;

		if (res != CURLE_OK) {
			std::cerr << "UTIL CURL upload(\"" << url << "\", \"" << src << "\") : ";
			if (error.length()) {
				std::cerr << error;
			} else {
				std::cerr << curl_easy_strerror(res);
			}
			std::cerr << "\n";
			return 2;
		}

		return 0;
	}
	/**
	* Upload the given file to a URL.
	* @param url the URL to attempt to upload to
	* @param src the path of the file to upload
	*
	* @see upload(const std::string&, const std::string&, std::function<int (curl_off_t, curl_off_t)>)
	*/
	int upload(const std::string& url, const std::string& src) {
		return upload(url, src, nullptr);
	}
}}

#endif // BEE_UTIL_CURL
