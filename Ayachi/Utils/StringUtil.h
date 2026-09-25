#pragma once
#include "../SDK/Text/MCTextFormat.h"
#include <algorithm>
#include <array>
#include <cstddef>
#include <random>
#include <string>
#include <string_view>
#include <vector>

namespace StringUtil {
	inline char toLowerAscii(char value) {
		const unsigned char c = static_cast<unsigned char>(value);
		return c >= 'A' && c <= 'Z' ? static_cast<char>(c + ('a' - 'A')) : value;
	}

	inline std::string toLower(std::string_view value) {
		std::string result(value);
		std::transform(result.begin(), result.end(), result.begin(), toLowerAscii);
		return result;
	}

	inline bool equalsIgnoreCase(std::string_view left, std::string_view right) {
		return left.size() == right.size() &&
			std::equal(left.begin(), left.end(), right.begin(),
				[](char lhs, char rhs) { return toLowerAscii(lhs) == toLowerAscii(rhs); });
	}

	inline std::string join(
		const std::vector<std::string>& values,
		std::size_t first,
		std::string_view separator = " ") {
		if (first >= values.size())
			return {};

		std::size_t length = 0;
		for (std::size_t index = first; index < values.size(); ++index)
			length += values[index].size() + (index == first ? 0 : separator.size());

		std::string result;
		result.reserve(length);
		for (std::size_t index = first; index < values.size(); ++index) {
			if (index != first)
				result.append(separator);
			result.append(values[index]);
		}
		return result;
	}

	inline bool invalidChar(char c) {
		return static_cast<unsigned char>(c) >= 128;
	}

	inline std::string sanitize(const std::string& string) {
		std::string out;
		out.reserve(string.size());
		for (size_t i = 0; i < string.size();) {
			const auto c = static_cast<unsigned char>(string[i]);
			if (c == 0xC2 && i + 1 < string.size()
				&& static_cast<unsigned char>(string[i + 1]) == 0xA7) {
				i += std::min<size_t>(3, string.size() - i);
				continue;
			}
			if (c < 128)
				out.push_back(static_cast<char>(c));
			++i;
		}
		return out;
	}

	inline bool isNumber(char c) {
		return c >= '0' && c <= '9';
	}

	inline std::wstring format(const std::string& string) {
		if (string.empty())
			return {};

		const int size = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, string.data(),
			static_cast<int>(string.size()), nullptr, 0);
		if (size <= 0)
			return {};

		std::wstring result(static_cast<size_t>(size), L'\0');
		if (MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, string.data(),
			static_cast<int>(string.size()), result.data(), size) == 0)
			return {};
		return result;
	}

	inline std::string format(const std::wstring& wString) {
		if (wString.empty())
			return {};

		const int size = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, wString.data(),
			static_cast<int>(wString.size()), nullptr, 0, nullptr, nullptr);
		if (size <= 0)
			return {};

		std::string result(static_cast<size_t>(size), '\0');
		if (WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, wString.data(),
			static_cast<int>(wString.size()), result.data(), size, nullptr, nullptr) == 0)
			return {};
		return result;
	}

	inline std::string formatToGame(const std::string& string) {
		return string;
	}

	inline std::string hash(const std::string& input) {
		uint64_t hash1 = 0xcbf29ce484222325;
		uint64_t hash2 = 0x14650fb0739d0383;
		static constexpr uint64_t prime1 = 6016682199633653;
		static constexpr uint64_t prime2 = 7909364740601323;
		for (char c : input) {
			hash1 ^= c;
			hash1 *= prime1;
			hash2 ^= c;
			hash2 *= prime2;

			hash1 ^= hash1 >> 33;
			hash1 *= 0xff51afd7ed558ccd;
			hash1 ^= hash1 >> 33;
			hash1 *= 0xc4ceb9fe1a85ec53;
			hash1 ^= hash1 >> 33;

			hash2 ^= hash2 >> 33;
			hash2 *= 0xff51afd7ed558ccd;
			hash2 ^= hash2 >> 33;
			hash2 *= 0xc4ceb9fe1a85ec53;
			hash2 ^= hash2 >> 33;

			unsigned int count = 5;
			hash1 += (hash1 << count) | (hash1 >> (64 - count));
			hash2 += (hash2 << count) | (hash2 >> (64 - count));

			count = 15;
			hash1 ^= (hash1 << count) | (hash1 >> (64 - count));
			hash2 ^= (hash2 << count) | (hash2 >> (64 - count));

			count = 27;
			hash1 += (hash1 << count) | (hash1 >> (64 - count));
			hash2 += (hash2 << count) | (hash2 >> (64 - count));
		}

		std::stringstream ss;
		ss << std::hex << std::setw(16) << std::setfill('0') << hash1
			<< std::setw(16) << std::setfill('0') << hash2;

		return ss.str();
	}

	inline std::vector<std::string> getAllLines(std::string& str) {
		std::vector<std::string> tokens;
		std::stringstream ss(str);
		std::string token;
		while (std::getline(ss, token, '\n')) {
			tokens.push_back(token);
		}
		str.clear();
		return tokens;
	}

	inline std::string random(const int size) {
		if (size <= 0)
			return {};

		std::string str("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
		static thread_local std::mt19937 generator(std::random_device{}());
		std::shuffle(str.begin(), str.end(), generator);
		return str.substr(0, std::min(static_cast<size_t>(size), str.size()));
	}

	inline const std::array<unsigned int, 256>& crc32Table() {
		static const std::array<unsigned int, 256> table = [] {
			std::array<unsigned int, 256> values{};
			constexpr unsigned int polynomial = 0xEDB88320;
			for (unsigned int i = 0; i < values.size(); ++i) {
				unsigned int crc = i;
				for (unsigned int bit = 0; bit < 8; ++bit)
					crc = (crc >> 1) ^ ((0u - (crc & 1u)) & polynomial);
				values[i] = crc;
			}
			return values;
		}();
		return table;
	}

	inline unsigned int getFileHash(const char* path, int seed) {
		std::ifstream file(path, std::ios::binary);
		if (!file.is_open()) return 0;

		const auto& table = crc32Table();
		unsigned int crc = ~static_cast<unsigned int>(seed);
		char byte;
		while (file.get(byte))
			crc = (crc >> 8) ^ table[(crc & 0xFF) ^ static_cast<uint8_t>(byte)];
		return ~crc;
	}

	inline unsigned int getHashedCRC(const char* str, int seed = 0) {
		const auto& table = crc32Table();
		const unsigned int resetValue = ~static_cast<unsigned int>(seed);
		unsigned int crc = resetValue;
		const auto* current = reinterpret_cast<const uint8_t*>(str);
		while (uint8_t c = *current++) {
			if (c == '#' && current[0] == '#' && current[1] == '#') crc = resetValue;
			crc = (crc >> 8) ^ table[(crc & 0xFF) ^ c];
		}
		return ~crc;
	}

	inline unsigned int getFileHash(const char* path, const char* seed) {
		return getFileHash(path, getHashedCRC(seed, 0x6a747764));
	}
}
