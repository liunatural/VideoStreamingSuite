#include "ImagePartition.h"
#include <string>


CImagePartition::CImagePartition()
{
}


CImagePartition::~CImagePartition()
{
}


bool  CImagePartition::Partition(void* in_buffer, int OriginalW, int OriginalH, void** out_buffer, int wBlocks, int hBlocks)
{
	char* buffer = (char*)in_buffer;
	int inbufferStep = 0, outbufferStep = 0, tmpStep = 0, iDistance = 0;
	int iBlocks = 0;

	if (wBlocks > 1 && hBlocks > 1)
	{
		if (OriginalW % wBlocks != 0)
		{
			return false;
		}
		if (OriginalH % hBlocks != 0)
		{
			return false;
		}
		int newW = OriginalW / wBlocks, newH = OriginalH / hBlocks;
		iDistance = newW * 3;

		for (int i = 0; i < hBlocks; ++i, tmpStep = 0)
			for (int j = 0; j < wBlocks; ++j)
			{
				tmpStep = iDistance * j;
				inbufferStep = OriginalW * newH * 3 * i;
				char* dest = (char*)out_buffer[iBlocks++];

				for (int k = 0; k < newH; ++k, inbufferStep += OriginalW * 3, outbufferStep += iDistance)
					memcpy(dest + outbufferStep, buffer + inbufferStep + tmpStep, iDistance);

				inbufferStep = 0;
				outbufferStep = 0;
			}

		return true;
	}
	else
		if (wBlocks > 1)
		{
			if (OriginalW % wBlocks != 0)
			{
				return false;
			}
			int newW = OriginalW / wBlocks, newH = OriginalH;
			iDistance = newW * 3;
			for (int i = 0; i < wBlocks; ++i)
			{
				tmpStep = i * iDistance;
				char* dest = (char*)out_buffer[iBlocks++];

				for (int j = 0; j < newH; ++j, inbufferStep += OriginalW * 3, outbufferStep += iDistance)
					memcpy(dest + outbufferStep, buffer + inbufferStep + tmpStep, iDistance);

				outbufferStep = 0;
				inbufferStep = 0;
			}

			return true;
		}
		else
			if (hBlocks > 1)
			{
				if (OriginalH % hBlocks != 0)
				{
					return false;
				}
				int newW = OriginalW, newH = OriginalH / hBlocks;
				iDistance = newW * newH * 3;

				for (int i = 0; i < hBlocks; ++i, inbufferStep += iDistance)
				{
					char* dest = (char*)out_buffer[i];
					memcpy(dest, buffer + inbufferStep, iDistance);
				}

				return true;
			}
			else
				return false;

	return true;
}

bool  CImagePartition::ImageUnite(void** in_buffer, int  wBlocks, int hBlocks, int width, int height, void* out_buffer)
{
	return ImageUnite(in_buffer, wBlocks, hBlocks, width, height, out_buffer, 24);
}


bool  CImagePartition::YUVImageUnite(void** in_buffer, int  wBlocks, int hBlocks, int width, int height, void* out_buffer)
{
	int out_y_size = width * height* hBlocks  * wBlocks;
	int out_u_size = out_y_size / 4;
	int out_v_size = out_u_size;

	if (wBlocks > 1 && hBlocks > 1)
	{
		return false;
	}

	if (hBlocks > 1)
	{
		int in_y_size = width * height;
		int in_u_size = in_y_size / 4;
		int in_v_size = in_u_size;
		for (int i = 0; i < hBlocks; ++i)
		{
			char* tmp = (char*)(in_buffer[i]);
			memcpy((char*)out_buffer + in_y_size * i, tmp, in_y_size);
			memcpy((char*)out_buffer + out_y_size + in_u_size *i, tmp + in_y_size, in_u_size);
			memcpy((char*)out_buffer + out_y_size + out_u_size + in_v_size*i, tmp + in_y_size + in_v_size, in_v_size);
		}
		return true;
	}
	if (wBlocks > 1)
	{
		return false;
	}
	return false;
}


bool  CImagePartition::ImageUnite(void** in_buffer, int  wBlocks, int hBlocks, int width, int height, void* out_buffer, int bits)
{
	int inbufferStep = 0, outbufferStep = 0, iDistance = width * bits / 8;
	if (wBlocks > 1 && hBlocks > 1)
	{
		char* dest = (char*)out_buffer;

		for (int i = 0; i < hBlocks; ++i, inbufferStep = 0)//
		{
			for (int h = 0; h < height; ++h, inbufferStep += iDistance)//
			{
				for (int j = 0; j < wBlocks; ++j, outbufferStep += iDistance)//
				{
					char* tmp = (char*)in_buffer[i * wBlocks + j];
					memcpy(dest + outbufferStep, tmp + inbufferStep, iDistance);

				}
			}
		}

		return true;
	}
	else
		if (wBlocks > 1)
		{
			char* dest = (char*)out_buffer;

			for (int h = 0; h < height; ++h, inbufferStep += iDistance)
			{
				for (int i = 0; i < wBlocks; ++i, outbufferStep += iDistance)
				{
					char* tmp = (char*)in_buffer[i];
					memcpy(dest + outbufferStep, tmp + inbufferStep, iDistance);
				}
			}

			return true;
		}
		else
			if (hBlocks > 1)
			{
				char* dest = (char*)out_buffer;
				iDistance = width * height * 3;

				for (int i = 0; i < hBlocks; ++i, outbufferStep += iDistance)
				{
					char* tmp = (char*)in_buffer[i];
					memcpy(dest + outbufferStep, tmp, iDistance);
				}

				return true;
			}
			else
				return false;

	return true;

}
