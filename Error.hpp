#pragma once

#include <sstream>
#include <cerrno>
#include <stdexcept>

#define CHECK_ERRNO(x) {int nError = (x); if (nError < 0){ std::stringstream ErrorStream; ErrorStream << __FILE__ << "(" << __LINE__ << "): " << strerror(errno); throw std::runtime_error(ErrorStream.str());}}
#define CHECK_EXPRESSION(x, e) {if (!(x)){ std::stringstream ErrorStream; ErrorStream << __FILE__ << "(" << __LINE__ << "): " << e; throw std::runtime_error(ErrorStream.str());}}