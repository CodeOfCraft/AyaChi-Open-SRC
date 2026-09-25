#pragma once

#include <string>
#include <string_view>
#include <vector>

struct ImFont;

class FontManager {
public:
	enum class FontState {
		BuiltIn,
		Remote,
		Downloading,
		Ready,
		Failed
	};

	struct FontInfo {
		std::string id;
		std::string name;
		std::string fileName;
	};

	static bool init();
	static void shutdown();

	static std::vector<FontInfo> getFonts();
	static std::vector<std::string> getFontOptions();
	static std::string getFontDirectory();
	static std::string getConfiguredFont();
	static std::string getActiveFont();
	static int getConfiguredFontIndex();
	static FontState getFontState(int index);

	static bool setFont(std::string_view id);
	static bool setFontByIndex(int index);

	static void setDefaultFont(ImFont* font);
	static void processPendingApply();
};
