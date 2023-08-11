#pragma once

#define LOG(x) std::cout << x << std::endl;

#define COLOR_FORMAT(text, color) (color + std::string(text) + "\033[0m")

#define RED(text) COLOR_FORMAT(text, "\033[31m")
#define GREEN(text) COLOR_FORMAT(text, "\033[1;32m")
#define YELLOW(text) COLOR_FORMAT(text, "\033[1;33m")
#define BLUE(text) COLOR_FORMAT(text, "\033[1;34m")
#define MAGENTA(text) COLOR_FORMAT(text, "\033[1;35m")
#define CYAN(text) COLOR_FORMAT(text, "\033[1;36m")
#define WHITE(text) COLOR_FORMAT(text, "\033[1;37m")
