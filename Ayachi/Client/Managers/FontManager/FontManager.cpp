#include "FontManager.h"

#include <algorithm>
#include <cctype>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <future>
#include <mutex>
#include <unordered_set>

#include <imgui.h>

#include "../../../Utils/FileUtil.h"
#include "../../../Utils/Logger.h"
#include "../../../Utils/NetworkUtil.h"

namespace {
	constexpr std::string_view defaultFontId = "default";
	constexpr std::string_view fontBaseUrl = "https://raw.deepseek.gay/Ayachi/fonts/";

	struct DownloadResult {
		std::string id;
		bool success = false;
	};

	const std::vector<FontManager::FontInfo>& catalog() {
		static const std::vector<FontManager::FontInfo> fonts = {
			{ "default", "Default", "" },
			{ "misans_thin", "MiSans Thin", "MiSans-Thin.ttf" },
			{ "misans_extralight", "MiSans ExtraLight", "MiSans-ExtraLight.ttf" },
			{ "misans_light", "MiSans Light", "MiSans-Light.ttf" },
			{ "misans_normal", "MiSans Normal", "MiSans-Normal.ttf" },
			{ "misans_regular", "MiSans Regular", "MiSans-Regular.ttf" },
			{ "misans_medium", "MiSans Medium", "MiSans-Medium.ttf" },
			{ "misans_demibold", "MiSans Demibold", "MiSans-Demibold.ttf" },
			{ "misans_semibold", "MiSans Semibold", "MiSans-Semibold.ttf" },
			{ "misans_bold", "MiSans Bold", "MiSans-Bold.ttf" },
			{ "misans_heavy", "MiSans Heavy", "MiSans-Heavy.ttf" }
		};
		return fonts;
	}

	std::mutex stateMutex;
	std::string configuredFont(defaultFontId);
	std::string activeFont(defaultFontId);
	std::string downloadingFont;
	bool applyPending = true;
	bool shuttingDown = false;
	ImFont* builtInFont = nullptr;
	ImFont* customFont = nullptr;
	std::string customFontId;
	std::future<DownloadResult> downloadFuture;
	std::unordered_set<std::string> failedFonts;

	std::string normalizeId(std::string_view id) {
		std::string result(id);
		std::transform(result.begin(), result.end(), result.begin(), [](unsigned char ch) {
			return static_cast<char>(std::tolower(ch));
		});
		return result;
	}

	const FontManager::FontInfo* findFont(std::string_view id) {
		const std::string normalized = normalizeId(id);
		auto found = std::find_if(catalog().begin(), catalog().end(), [&](const auto& font) {
			return font.id == normalized;
		});
		return found == catalog().end() ? nullptr : &*found;
	}

	std::filesystem::path fontPath(const FontManager::FontInfo& font) {
		return std::filesystem::path(FontManager::getFontDirectory()) / font.fileName;
	}

	bool isValidFontFile(const std::filesystem::path& path) {
		std::error_code error;
		if (!std::filesystem::is_regular_file(path, error) || std::filesystem::file_size(path, error) < 1024)
			return false;

		std::ifstream input(path, std::ios::binary);
		unsigned char signature[4]{};
		input.read(reinterpret_cast<char*>(signature), sizeof(signature));
		if (input.gcount() != sizeof(signature))
			return false;

		const bool trueType = signature[0] == 0x00 && signature[1] == 0x01
			&& signature[2] == 0x00 && signature[3] == 0x00;
		const bool openType = signature[0] == 'O' && signature[1] == 'T'
			&& signature[2] == 'T' && signature[3] == 'O';
		return trueType || openType;
	}

	DownloadResult downloadFont(FontManager::FontInfo font) {
		DownloadResult result{ font.id, false };
		const std::filesystem::path finalPath = fontPath(font);
		const std::filesystem::path temporaryPath = finalPath.string() + ".download";
		std::error_code error;
		std::filesystem::remove(temporaryPath, error);

		const std::string directory = FontManager::getFontDirectory();
		const std::string url = std::string(fontBaseUrl) + font.fileName;
		if (!NetworkUtil::downloadFile(temporaryPath.filename().string(), directory, url)
			|| !isValidFontFile(temporaryPath)) {
			std::filesystem::remove(temporaryPath, error);
			return result;
		}

		std::filesystem::remove(finalPath, error);
		error.clear();
		std::filesystem::rename(temporaryPath, finalPath, error);
		result.success = !error && isValidFontFile(finalPath);
		if (!result.success)
			std::filesystem::remove(temporaryPath, error);
		return result;
	}

	void startDownloadLocked(const FontManager::FontInfo& font) {
		if (shuttingDown || downloadFuture.valid() || font.fileName.empty())
			return;
		downloadingFont = font.id;
		failedFonts.erase(font.id);
		downloadFuture = std::async(std::launch::async, [font]() { return downloadFont(font); });
	}
}

bool FontManager::init() {
	const std::string directory = getFontDirectory();
	if (!FileUtil::doesFilePathExist(directory))
		FileUtil::createPath(directory);

	std::lock_guard lock(stateMutex);
	configuredFont = std::string(defaultFontId);
	activeFont = std::string(defaultFontId);
	downloadingFont.clear();
	failedFonts.clear();
	applyPending = true;
	shuttingDown = false;
	return true;
}

void FontManager::shutdown() {
	std::future<DownloadResult> pendingDownload;
	{
		std::lock_guard lock(stateMutex);
		shuttingDown = true;
		if (downloadFuture.valid())
			pendingDownload = std::move(downloadFuture);
	}
	if (pendingDownload.valid())
		pendingDownload.wait();

	std::lock_guard lock(stateMutex);
	failedFonts.clear();
	builtInFont = nullptr;
	customFont = nullptr;
	customFontId.clear();
	downloadingFont.clear();
}

std::vector<FontManager::FontInfo> FontManager::getFonts() {
	return catalog();
}

std::vector<std::string> FontManager::getFontOptions() {
	std::vector<std::string> result;
	result.reserve(catalog().size());
	for (const FontInfo& font : catalog())
		result.push_back(font.name);
	return result;
}

std::string FontManager::getFontDirectory() {
	return FileUtil::getClientPath() + "Assets\\Fonts\\";
}

std::string FontManager::getConfiguredFont() {
	std::lock_guard lock(stateMutex);
	return configuredFont;
}

std::string FontManager::getActiveFont() {
	std::lock_guard lock(stateMutex);
	return activeFont;
}

int FontManager::getConfiguredFontIndex() {
	std::lock_guard lock(stateMutex);
	for (std::size_t i = 0; i < catalog().size(); ++i) {
		if (catalog()[i].id == configuredFont)
			return static_cast<int>(i);
	}
	return 0;
}

FontManager::FontState FontManager::getFontState(int index) {
	if (index <= 0)
		return FontState::BuiltIn;
	if (static_cast<std::size_t>(index) >= catalog().size())
		return FontState::Failed;

	const FontInfo& font = catalog()[static_cast<std::size_t>(index)];
	std::lock_guard lock(stateMutex);
	if (font.id == downloadingFont)
		return FontState::Downloading;
	if (isValidFontFile(fontPath(font)))
		return FontState::Ready;
	return failedFonts.contains(font.id) ? FontState::Failed : FontState::Remote;
}

bool FontManager::setFont(std::string_view id) {
	const FontInfo* font = findFont(id);
	if (font == nullptr)
		return false;

	std::lock_guard lock(stateMutex);
	configuredFont = font->id;
	applyPending = true;
	if (font->id != defaultFontId && !isValidFontFile(fontPath(*font)))
		startDownloadLocked(*font);
	return true;
}

bool FontManager::setFontByIndex(int index) {
	if (index < 0 || static_cast<std::size_t>(index) >= catalog().size())
		return false;
	return setFont(catalog()[static_cast<std::size_t>(index)].id);
}

void FontManager::setDefaultFont(ImFont* font) {
	std::lock_guard lock(stateMutex);
	builtInFont = font;
	applyPending = true;
}

void FontManager::processPendingApply() {
	std::future<DownloadResult> finishedDownload;
	{
		std::lock_guard lock(stateMutex);
		if (downloadFuture.valid()
			&& downloadFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
			finishedDownload = std::move(downloadFuture);
	}

	if (finishedDownload.valid()) {
		const DownloadResult result = finishedDownload.get();
		std::lock_guard lock(stateMutex);
		downloadingFont.clear();
		if (result.success) {
			failedFonts.erase(result.id);
			if (configuredFont == result.id)
				applyPending = true;
			LOGI("Font downloaded (id=%s)", result.id.c_str());
		}
		else {
			failedFonts.insert(result.id);
			LOGE("Font download failed (id=%s)", result.id.c_str());
		}
	}

	std::string targetId;
	std::filesystem::path targetPath;
	ImFont* cachedFont = nullptr;
	ImFont* fallbackFont = nullptr;
	ImFont* previousCustomFont = nullptr;
	{
		std::lock_guard lock(stateMutex);
		const FontInfo* target = findFont(configuredFont);
		if (target == nullptr)
			return;
		if (target->id != defaultFontId && !isValidFontFile(fontPath(*target))) {
			startDownloadLocked(*target);
			return;
		}
		if (!applyPending || builtInFont == nullptr)
			return;

		targetId = target->id;
		fallbackFont = builtInFont;
		previousCustomFont = customFont;
		if (targetId != defaultFontId) {
			targetPath = fontPath(*target);
			if (customFontId == targetId)
				cachedFont = customFont;
		}
	}

	ImGuiIO& io = ImGui::GetIO();
	ImFont* selectedFont = targetId == defaultFontId ? fallbackFont : cachedFont;
	if (previousCustomFont != nullptr && previousCustomFont != selectedFont) {
		io.FontDefault = fallbackFont;
		io.Fonts->RemoveFont(previousCustomFont);
		previousCustomFont = nullptr;
	}
	if (selectedFont == nullptr) {
		static const ImWchar glyphRanges[] = {
			0x0020, 0x00FF,
			0x2000, 0x206F,
			0x3000, 0x303F,
			0x4E00, 0x9FFF,
			0
		};
		ImFontConfig config;
		config.OversampleH = 2;
		config.OversampleV = 1;
		config.PixelSnapH = false;
		selectedFont = io.Fonts->AddFontFromFileTTF(
			targetPath.string().c_str(), 25.f, &config, glyphRanges);
	}

	std::lock_guard lock(stateMutex);
	if (selectedFont == nullptr) {
		failedFonts.insert(targetId);
		customFont = nullptr;
		customFontId.clear();
		activeFont = std::string(defaultFontId);
		applyPending = false;
		return;
	}
	customFont = targetId == defaultFontId ? nullptr : selectedFont;
	customFontId = targetId == defaultFontId ? std::string() : targetId;
	io.FontDefault = selectedFont;
	activeFont = targetId;
	applyPending = false;
	LOGI("Font applied (id=%s)", targetId.c_str());
}
