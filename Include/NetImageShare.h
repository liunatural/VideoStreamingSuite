#ifndef __NET_IMAGE_SHARE_H__
#define __NET_IMAGE_SHARE_H__
#include "HXLibMessage.h"

#pragma pack(push, 1)

struct ImageHeader
{
	int			width;
	int			height;
	double		dTimer;
	int			size;
	int			fps;
	int			timestamp;
	int			netPacketSize;
	int			sliceIndex;
};
#define id(mid)	id_##mid

enum ImageMessageID {
	ImageMessageIDBase= HXMessagePackage::Header::ID_UserIDBase +1000,
	id(ImageHeader),
};

#pragma pack(pop)

#endif
