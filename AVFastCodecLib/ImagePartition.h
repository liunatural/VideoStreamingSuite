#pragma once

class CImagePartition
{
public:
	CImagePartition();
	virtual ~CImagePartition();
	bool  Partition(void* in_buffer, int OriginalW, int OriginalH, void** out_buffer, int wBlocks, int hBlocks);
	bool  YUVImageUnite(void** in_buffer, int  wBlocks, int hBlocks, int width, int height, void* out_buffer);
	bool  ImageUnite(void** in_buffer, int  wBlocks, int hBlocks, int width, int height, void* out_buffer);
	bool  ImageUnite(void** in_buffer, int  wBlocks, int hBlocks, int width, int height, void* out_buffer, int bits);
};

