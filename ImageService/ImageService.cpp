// ImageService.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "HXLibNetwork.h"
#include <Windows.h>
#include "../Include/NetImageShare.h"

char *firstHeadCache;

class Receiver
{
public:
	Receiver() {
		m_bIsSender = false;
		m_bHasSentFirstFrame = false;
	}
	~Receiver() {
	}
public:
	LinkID		m_id;
	bool		m_bIsSender;
	bool		m_bHasSentFirstFrame;
};
int main()
{
	SetupLogFiles("", true);
	HXLibService*	service = CreateNewService();
	if (!service->Start(12111, 100))
		return 0;
	int count = 0;
	HXBigMessagePackage* bigData = CreateBigPackage();
	HXBigMessagePackage* firstFramebigData = CreateBigPackage();

	std::vector<ImageHeader*>	imageCached;
	std::vector<Receiver*>		clients;
	double dSyncLastTime = GetTimer()->GetTickTimer();
	double dFPS = 10.0f;
	while (true)
	{
		HXMessageMap& mmap = service->SwapQueue();
		for (unsigned int i = 0; i < mmap.GetCount(); i++)
		{
			const HXMessagePackage* pack = mmap.GetPackage(i);
			LinkID linkID = pack->linkid();
			int plyId = (pack->linkid()).sid;
			switch (pack->header()->id)		//根据消息ID和命令ID分别进行逻辑处理
			{
			case HXMessagePackage::Header::ID_Connect:
			{
				Receiver* rcv = new Receiver();
				rcv->m_id = linkID;
				clients.push_back(rcv);
				service->SetContext(linkID, (const char*)rcv);
			}
			break;
			case HXMessagePackage::Header::ID_Disconnect:
			{
				for (int i = 0; i < clients.size(); i++) {
					if (clients[i]->m_id == linkID) {
						delete clients[i];
						clients.erase(clients.begin() + i);
						i--;
					}
				}
				break;
			}
			case id(ImageHeader):
			{
				Receiver* rcv = 0;
				if (service->GetContext(linkID, (const char**)&rcv)) {
					rcv->m_bIsSender = true;
					char* temp = new char[pack->body_length()];
					ImageHeader* header = (ImageHeader*)temp;
					memcpy(temp, pack->body(), pack->body_length());
					header->netPacketSize = pack->body_length();

					dFPS = header->fps;
					if (dFPS < 1.0)dFPS = 1.0;
					//printf("Recv:(%d)%.06f|%d\n", header->timestamp, header->dTimer, pack->body_length());
					imageCached.push_back(header);
					
					if (header->sliceIndex == 1)
					{
						
						firstHeadCache = new char[pack->body_length()];
						memcpy(firstHeadCache, pack->body(), pack->body_length());
						ImageHeader* header1 = (ImageHeader*)firstHeadCache;
						header1->netPacketSize = pack->body_length();
						firstFramebigData->SetBigPackage(id(ImageHeader), 0, header1, header1->netPacketSize);
					}


				}

				if (imageCached.size() > 0) {
					ImageHeader* header = imageCached[imageCached.size() - 1];
					bigData->SetBigPackage(id(ImageHeader), 0, header, header->netPacketSize);
					for (int i = 0; i < clients.size(); i++) {

						if (!clients[i]->m_bIsSender) 
						{
							if (!clients[i]->m_bHasSentFirstFrame)
							{
								service->Send(clients[i]->m_id, *firstFramebigData);
								clients[i]->m_bHasSentFirstFrame = true;
							}

							service->Send(clients[i]->m_id, *bigData);
						}
					}
					//printf("Sync To All\n");
				}
			}
			break;
			}
		}
		while (imageCached.size() > 1) {
			ImageHeader* header = imageCached[0];
			char* remove = (char*)header;
			delete[] remove;
			imageCached.erase(imageCached.begin());
		}
		Sleep(1);
	}
	return 0;
}

