#include <string>

namespace debug {

enum Color {thin_red = 42, red = 43, thin_green = 44, green = 45, thin_yellow = 46, yellow = 47, thin_blue = 48, blue = 49};

inline std::string colorize(const std::string& str, Color color) {
  switch (color) {
  case thin_red:
    return "\033[0m\033[0;31m" + str + "\033[0m";
  case red:
    return "\033[0m\033[1;31m" + str + "\033[0m";
  case thin_green:
    return "\033[0m\033[0;32m" + str + "\033[0m";
  case green:
    return "\033[0m\033[1;32m" + str + "\033[0m";
  case thin_yellow:
    return "\033[0m\033[0;33m" + str + "\033[0m";
  case yellow:
    return "\033[0m\033[1;33m" + str + "\033[0m";
  case thin_blue:
    return "\033[0m\033[0;34m" + str + "\033[0m";
  case blue:
    return "\033[0m\033[0;34m" + str + "\033[0m";
  default:
    return str;
  }
}

} // namespace debug