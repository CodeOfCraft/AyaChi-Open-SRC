#include "NetworkUtil.h"
#include "../Libs/json.hpp"
#include <windows.h>
#include <WinInet.h>

#pragma comment(lib, "wininet.lib")

namespace {
	class InternetHandle {
	public:
		explicit InternetHandle(HINTERNET value = nullptr) : value(value) {}

		~InternetHandle() {
			if (value != nullptr)
				InternetCloseHandle(value);
		}

		InternetHandle(const InternetHandle&) = delete;
		InternetHandle& operator=(const InternetHandle&) = delete;

		HINTERNET get() const { return value; }
		explicit operator bool() const { return value != nullptr; }

	private:
		HINTERNET value;
	};
}

bool NetworkUtil::canAccessInternet() {
	const char url[] = "https://www.google.com/";
	return InternetCheckConnectionA(url, FLAG_ICC_FORCE_CONNECTION, 0);
}

bool NetworkUtil::downloadFile(const std::string& name, const std::string& path, const std::string& url) {
	const std::string filePath = path + name;
	InternetHandle session(InternetOpenA("Mozilla/5.0", INTERNET_OPEN_TYPE_DIRECT, nullptr, nullptr, 0));
	if (!session)
		return false;
	DWORD connectTimeout = 10000;
	DWORD receiveTimeout = 30000;
	InternetSetOptionA(session.get(), INTERNET_OPTION_CONNECT_TIMEOUT,
		&connectTimeout, sizeof(connectTimeout));
	InternetSetOptionA(session.get(), INTERNET_OPTION_RECEIVE_TIMEOUT,
		&receiveTimeout, sizeof(receiveTimeout));

	InternetHandle request(InternetOpenUrlA(session.get(), url.c_str(), nullptr, 0,
		INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_NO_UI, 0));
	if (!request)
		return false;

	DWORD statusCode = 0;
	DWORD statusCodeSize = sizeof(statusCode);
	if (!HttpQueryInfoA(request.get(), HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER,
		&statusCode, &statusCodeSize, nullptr) || statusCode != HTTP_STATUS_OK)
		return false;

	std::ofstream outputFile(filePath, std::ios::binary);
	if (!outputFile.is_open())
		return false;

	char buffer[2000];
	DWORD bytesRead = 0;
	do {
		if (!InternetReadFile(request.get(), buffer, sizeof(buffer), &bytesRead))
			return false;
		outputFile.write(buffer, bytesRead);
	} while (bytesRead > 0 && outputFile.good());

	return outputFile.good();
}

std::string NetworkUtil::getStrings(const std::string& url) {
	InternetHandle session(InternetOpenA(xorstr_("Mozilla/5.0"), INTERNET_OPEN_TYPE_DIRECT,
		nullptr, nullptr, 0));
	if (!session)
		return xorstr_("ERROR");

	const DWORD flags = INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE
		| INTERNET_FLAG_NO_UI | INTERNET_FLAG_SECURE;
	InternetHandle request(InternetOpenUrlA(session.get(), url.c_str(), nullptr, 0, flags, 0));
	if (!request)
		return xorstr_("ERROR");

	DWORD statusCode = 0;
	DWORD statusCodeSize = sizeof(statusCode);
	if (!HttpQueryInfoA(request.get(), HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER,
		&statusCode, &statusCodeSize, nullptr) || statusCode != HTTP_STATUS_OK)
		return xorstr_("ERROR");

	std::string result;
	char buffer[2000];
	DWORD bytesRead = 0;
	do {
		if (!InternetReadFile(request.get(), buffer, sizeof(buffer), &bytesRead))
			return xorstr_("ERROR");
		result.append(buffer, bytesRead);
	} while (bytesRead > 0);

	return result;
}

bool NetworkUtil::sendWebhook(const char* botName, const char* content, const char* webhookAddr) {
	if (webhookAddr == nullptr)
		return false;

	URL_COMPONENTSA components{};
	components.dwStructSize = sizeof(components);
	components.dwHostNameLength = static_cast<DWORD>(-1);
	components.dwUrlPathLength = static_cast<DWORD>(-1);
	components.dwExtraInfoLength = static_cast<DWORD>(-1);
	if (!InternetCrackUrlA(webhookAddr, 0, 0, &components))
		return false;
	if ((components.nScheme != INTERNET_SCHEME_HTTP && components.nScheme != INTERNET_SCHEME_HTTPS)
		|| components.dwHostNameLength == 0)
		return false;

	const std::string host(components.lpszHostName, components.dwHostNameLength);
	std::string path = components.dwUrlPathLength > 0
		? std::string(components.lpszUrlPath, components.dwUrlPathLength)
		: "/";
	if (components.dwExtraInfoLength > 0)
		path.append(components.lpszExtraInfo, components.dwExtraInfoLength);

	InternetHandle session(InternetOpenA("Mozilla/5.0", INTERNET_OPEN_TYPE_DIRECT,
		nullptr, nullptr, 0));
	if (!session)
		return false;

	InternetHandle connection(InternetConnectA(session.get(), host.c_str(), components.nPort,
		nullptr, nullptr, INTERNET_SERVICE_HTTP, 0, 0));
	if (!connection)
		return false;

	DWORD flags = INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_NO_UI;
	if (components.nScheme == INTERNET_SCHEME_HTTPS)
		flags |= INTERNET_FLAG_SECURE;
	InternetHandle request(HttpOpenRequestA(connection.get(), "POST", path.c_str(), nullptr,
		nullptr, nullptr, flags, 0));
	if (!request)
		return false;

	const std::string payload = nlohmann::json{
		{ "content", content == nullptr ? "" : content },
		{ "username", botName == nullptr ? "" : botName }
	}.dump();
	constexpr char headers[] = "Content-Type: application/json\r\n";
	if (!HttpSendRequestA(request.get(), headers, static_cast<DWORD>(sizeof(headers) - 1),
		const_cast<char*>(payload.data()), static_cast<DWORD>(payload.size())))
		return false;

	DWORD statusCode = 0;
	DWORD statusCodeSize = sizeof(statusCode);
	return HttpQueryInfoA(request.get(), HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER,
		&statusCode, &statusCodeSize, nullptr)
		&& statusCode >= 200 && statusCode < 300;
}
