#include "color/Color.hpp"

ColorFormatter::ColorFormatter(const std::string& text, const std::string& colorCode)
	: mText(text), mColorCode(colorCode) {
}

std::string ColorFormatter::format() const {
	return mColorCode + mText + "\033[0m";
}

