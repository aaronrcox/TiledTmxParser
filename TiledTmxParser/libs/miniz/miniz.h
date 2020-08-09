#pragma once

// This is a wrapper around the miniz single header only lib - see miniz_impl.h
// MINIZ available at: https://code.google.com/archive/p/miniz/

#include <vector>
#include <string>

namespace miniz
{
	std::vector<unsigned char> uncompress(const std::string& src, unsigned int decomressedSize);
}


