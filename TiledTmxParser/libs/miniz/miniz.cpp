#include "miniz.h"
using namespace miniz;

#include "miniz_impl.h"
#undef uncompress;


std::vector<unsigned char> miniz::uncompress(const std::string& src, unsigned int decomressedSize)
{

	std::vector<unsigned char> data(decomressedSize, 0);
	const unsigned char* pSrc = (const unsigned char*)src.c_str();

	int result = mz_uncompress(
		(unsigned char *)&data[0], 
		(mz_ulong *)&decomressedSize, 
		pSrc, 
		(mz_ulong)src.length()
	);


	return data;

}