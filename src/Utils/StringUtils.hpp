#ifndef __UTILS__STRINGUTILS_HPP__
#define __UTILS__STRINGUTILS_HPP__

#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <stdlib.h>
#include <memory>
#include <algorithm>
#include <cctype>

#include <RakNet/BitStream.h>

namespace StringUtils {

	inline std::vector<std::string> splitString(const std::string &s, char delim) {
		std::stringstream ss;
		ss.str(s);
		std::string item;
		std::vector<std::string> elems;
		while (std::getline(ss, item, delim)) {
			elems.push_back(item);
		}
		return elems;
	}

	inline std::vector<std::wstring> splitWString(const std::wstring &s, wchar_t delim) {
		std::wstringstream ss;
		ss.str(s);
		std::wstring item;
		std::vector<std::wstring> elems;
		while (std::getline(ss, item, delim)) {
			elems.push_back(item);
		}
		return elems;
	}

	inline bool replace(std::string& str, const std::string& from, const std::string& to) {
		size_t start_pos = str.find(from);
		if (start_pos == std::string::npos)
			return false;
		str.replace(start_pos, from.length(), to);
		return true;
	};

	template<class T>
	inline std::wstring readWStringFromBitStream(RakNet::BitStream * bitstream) {
		std::uint32_t size; bitstream->Read<T>(size);
		std::unique_ptr<char[]> buff = std::make_unique<char[]>(size * 2);
		bitstream->Read(buff.get(), size * 2);
		wchar_t * data = reinterpret_cast<wchar_t*>(buff.get());

		return std::wstring(data, size);
	}

	template<class T>
	inline std::string readStringFromBitStream(RakNet::BitStream * bitstream) {
		std::uint32_t size; bitstream->Read<T>(size);
		if (size == 0xcccccccc) throw std::runtime_error("Unable to read string!");
		std::unique_ptr<char[]> buff = std::make_unique<char[]>(size);
		bitstream->Read(buff.get(), size);
		char * data = static_cast<char*>(buff.get());

		return std::string(data, size);
	}

	inline std::wstring readBufferedWStringFromBitStream(RakNet::BitStream * bitstream, int len = 33) {
		std::unique_ptr<char[]> buff = std::make_unique<char[]>(len * 2);
		bitstream->Read(buff.get(), len * 2);

		wchar_t * data = reinterpret_cast<wchar_t*>(buff.get());

		int i;
		for (i = 0; i < len; ++i) {
			if (*(data + i) == 0x0000) break;
		}

		std::wstring returnVal = std::wstring(reinterpret_cast<wchar_t*>(buff.get()), i);

		return returnVal;
	};

	inline std::string readBufferedStringFromBitStream(RakNet::BitStream * bitstream, int len = 33) {
		std::unique_ptr<char[]> buff = std::make_unique<char[]>(len);
		bitstream->Read(buff.get(), len);

		char * data = reinterpret_cast<char*>(buff.get());

		int i;
		for (i = 0; i < len; ++i) {
			if (*(data + i) == 0x00) break;
		}

		std::string returnVal = std::string(reinterpret_cast<char*>(buff.get()), i);

		return returnVal;
	};

	inline void FillZero(RakNet::BitStream * bitstream, int count) {
		bitstream->Write(std::string(count, 0x00).c_str(), count);
	}

	template<class T>
	inline void writeStringToBitStream(RakNet::BitStream * bitstream, std::string text) {
		std::uint32_t size = text.size();
		bitstream->Write<T>(size);

		bitstream->Write(text.c_str(), text.size());
	}

	template<class T>
	inline void writeWStringToBitStream(RakNet::BitStream * bitstream, std::wstring text) {
		T size = text.size();
		bitstream->Write<T>(size);

		bitstream->Write(reinterpret_cast<const char*>(text.c_str()), text.size() * 2);
	}

	inline void writeBufferedStringToBitStream(RakNet::BitStream * bitstream, std::string text, unsigned int len = 33) {
		if (text.length() > len) text = text.substr(0, len);
		bitstream->Write(text.c_str(), static_cast<const unsigned int>(text.length()));
		FillZero(bitstream, len - static_cast<unsigned int>(text.length()));
	}

	inline void writeBufferedWStringToBitStream(RakNet::BitStream * bitstream, std::wstring text, unsigned int len = 33) {
		if (text.length() > len) text = text.substr(0, len);
		bitstream->Write(reinterpret_cast<const char*>(text.c_str()), text.length() * 2);
		FillZero(bitstream, (len - text.length()) * 2);
	}

	inline void ToLowerSelf(std::string &data) {
		std::transform(data.begin(), data.end(), data.begin(), [](unsigned char c) -> unsigned char {	
			return std::tolower(c);
		});
	}

	inline std::string ToLower(std::string data) {
		std::string out(data);
		ToLowerSelf(out);
		return out;
	}

	inline char NibleToHex(char nible) {
		static const char* hexLookup = "0123456789ABCDEF";
		return hexLookup[nible];
	}

	inline std::string CharToHex(char c) {
		std::string o = "";
		o += NibleToHex(c >> 4);
		o += NibleToHex(c & 0x0F);
		return o;
	}

	inline std::string StringToHex(std::string input, char divider = '\0') {
		std::string out = "";
		for (int i = 0; i < input.size(); ++i) {
			if (i != 0) out += divider;
			out += CharToHex(input.at(i));
		}
	}
}

#endif // !__UTILS__STRINGUTILS_HPP__
