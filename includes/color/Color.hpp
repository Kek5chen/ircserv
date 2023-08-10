#pragma once

#include <iostream>
#include <string>

class ColorFormatter {
public:
	ColorFormatter(const std::string& text, const std::string& colorCode);
	std::string format() const;

private:
	std::string mText;
	std::string mColorCode;
};

#define RED(text) ColorFormatter(text, "\033[31m").format()
#define GREEN(text) ColorFormatter(text, "\033[1;32m").format()
#define YELLOW(text) ColorFormatter(text, "\033[1;33m").format()
#define BLUE(text) ColorFormatter(text, "\033[1;34m").format()
#define MAGENTA(text) ColorFormatter(text, "\033[1;35m").format()
#define CYAN(text) ColorFormatter(text, "\033[1;36m").format()
#define WHITE(text) ColorFormatter(text, "\033[1;37m").format()
