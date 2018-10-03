//
//����ģ�������Ϣ�ࡣ���Ķ��� �Ե���. 2017-11-30
// ~~~~~~~~~~~~~~~~

#ifndef __IMAGE_MESSAGE_HPP__
#define __IMAGE_MESSAGE_HPP__

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <vector>

#pragma pack(push,1)
struct LinkID {
	union {
		int				id;
		struct {
			unsigned short	sid, sindex;
		};
	};
	bool operator == (const LinkID& other) const {
		return other.id == id;
	}
	LinkID() { id = 0; }
};
struct HXMessagePackage
{
	struct Header {
		enum {
			//���û����ӽ�������������һ��ip��ַ���ַ���
			ID_Connect		=	101,
			//ĳ���û��Ͽ�����
			ID_Disconnect	=	102,
			//ֻ�пͻ��˱����������ӽ���ʧ��
			ID_ConnectFailure = 103,
			//�޴���Ϣ����ͷ
			ID_BigStreamBegin = 104,
			//�޴���Ϣ������
			ID_BigStreamData = 105,
			//�޴���Ϣ������
			ID_BigStreamEnd = 106,
			//�������ݣ�ϵͳ�ڲ��Զ�����
			ID_Heartbeat = 107,
			//�û��߼���Ҫ��ID��ʼֵ��С�����ֵ���ڱ���ֵ
			ID_UserIDBase	=	1000,
		};
		//�Ͽ�������Ϣ��
		enum DisconnectID2 {
			//��ȡ���ݰ�ʱ�����
			Dissconnect_ReadError = 1,
			//д�����ݰ�ʱ�����
			Dissconnect_WriteError = 2,
			//�߼��߳��������ر�
			Dissconnect_ClosedByMain = 3,
		};
		//��һ����Ҫ����
		unsigned short	id;
		//�ڶ�������ID����
		unsigned short	id2;
		int				length;
	};
	struct ImageSize {
		unsigned short	width, height;
	};
	enum { header_length = sizeof(Header) };
	//��󵥸���Ϣ������32k
	enum { max_body_length = 32 * 1024 };// 40 * 1024 * 3
	HXMessagePackage() { bigDataPointer = 0; }
	//
	Header*			header() { return (Header*)data_; }
	const Header*	header() const { return (Header*)data_; }
	//���ݿ�ָ��
	const char*		data() const { return data_; }
	char*			data() { return data_; }
	//���ݿ��С
	size_t			length() const { return header_length + body_length(); }
	//��������ָ��
	const char*		body() const {
		if (bigDataPointer)
			return bigDataPointer;
		return data_ + header_length;
	}
	char*			body() { return data_ + header_length; }
	template <typename T>
	const T*		body() const {
		if (bigDataPointer)
			return (T*)(bigDataPointer);
		return (T*)(data_ + header_length);
	}
	//�������ݴ�С
	size_t			body_length() const { return ((Header*)data_)->length; }
	void			body_length(size_t new_length){
		if (new_length > max_body_length)
			new_length = max_body_length;
		header()->length = (int)new_length;
	}
	//���ݵĲ���
	void			linkid(LinkID id) { m_linkid = id; }
	LinkID			linkid() { return m_linkid; }
	const LinkID	linkid() const { return m_linkid; }
	LinkID			m_linkid;
	char			data_[header_length + max_body_length];
	char*			bigDataPointer;
};
#pragma pack(pop)

//��Ϣ���нṹ
class HXMessageMap
{
public:
	HXMessageMap(){}
	virtual ~HXMessageMap(){}
	//����һ����Ϣָ��
	virtual	const HXMessagePackage*	GetPackage(unsigned int index) = 0;
	//������Ϣ������
	virtual	int						GetCount() = 0;
};

struct BigPackageData
{
	unsigned int			length;
	char*					data;//��̬�����ڴ�
};

//�����Ϣ��������
class HXBigMessagePackage: public HXMessageMap
{
public:
	enum ZipLevel {
		ZipLevel_None = 0,//�������κ�ѹ��
	};
	//���һ������Ϣ��
	virtual	void				SetBigPackage(unsigned short id, unsigned short id2, const void* ptr, int dataLength, ZipLevel zipLevel = ZipLevel_None) = 0;
	//��������ݷ��Ͷ�֡������ڴ��С����maxOfMemory�����������������µĴ����Ϣ������֮ǰ�����ݰ���������ٷ��ͣ�����ܳ���4G
	virtual	void				SetAllowLostPackage(bool bAllowLost, unsigned int maxOfMemory) = 0;
	//�Ƿ�����֡
	virtual	bool				IsAllowLost() = 0;
	//������󻺴��С
	virtual unsigned int		GetAllowMaxOfMemory() = 0;
};

struct streamWriter {
	streamWriter() { length = 0; }
	void			Reset() { length = 0; }
	bool operator << (const char* text) {
		unsigned short len = (unsigned short)strlen(text);
		if (!this->operator<<(len))
			return false;
		if ((length + len) > HXMessagePackage::max_body_length)
			return false;
		memcpy(buffer + length, text, len);
		length += len;
		return true;
	}
	template <typename T>
	bool	operator << (const T& t) {
		if ((length + sizeof(T)) > HXMessagePackage::max_body_length)
			return false;
		memcpy(buffer + length, &t, sizeof(T));
		length += sizeof(T);
		return true;
	}
	char			buffer[HXMessagePackage::max_body_length];
	unsigned short	length;
};
struct streamReader {
	streamReader(const char* _data, unsigned short _length)
	{
		data = _data;
		dataLength = _length;
		length = 0;
	}
	struct ResultText {
		char* text;
		ResultText() { text = 0; }
		~ResultText() { if (text)delete[] text; }
	};
	bool	operator >> (ResultText& result) {
		unsigned short len = 0;
		if (!this->operator >> (len))
			return false;
		result.text = new char[len + 1];
		if ((length + len) > dataLength)
			return false;
		memcpy(result.text, data + length, len);
		length += len;
		return true;
	}
	template <typename T>
	bool	operator >> (T& result) {
		if ((length + sizeof(T)) > dataLength)
			return false;
		memcpy(&result, data + length, sizeof(T));
		length += sizeof(T);
		return true;
	}
	//
	const char*		data;
	unsigned short	dataLength;
	unsigned short	length;
};


#endif // CHAT_MESSAGE_HPP
