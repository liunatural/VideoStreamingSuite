//
//网络模块基础消息类。恒心东方 赵德贤. 2017-11-30
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
			//新用户链接进来，内容体是一个ip地址的字符串
			ID_Connect		=	101,
			//某个用户断开链接
			ID_Disconnect	=	102,
			//只有客户端被触发，链接建立失败
			ID_ConnectFailure = 103,
			//巨大消息包开头
			ID_BigStreamBegin = 104,
			//巨大消息包数据
			ID_BigStreamData = 105,
			//巨大消息包结束
			ID_BigStreamEnd = 106,
			//心跳数据，系统内部自动处理
			ID_Heartbeat = 107,
			//用户逻辑需要的ID开始值，小于这个值属于保留值
			ID_UserIDBase	=	1000,
		};
		//断开连接信息体
		enum DisconnectID2 {
			//读取数据包时候出错
			Dissconnect_ReadError = 1,
			//写入数据包时候出错
			Dissconnect_WriteError = 2,
			//逻辑线程内主动关闭
			Dissconnect_ClosedByMain = 3,
		};
		//第一个主要参数
		unsigned short	id;
		//第二个二级ID参数
		unsigned short	id2;
		int				length;
	};
	struct ImageSize {
		unsigned short	width, height;
	};
	enum { header_length = sizeof(Header) };
	//最大单个消息包长度32k
	enum { max_body_length = 32 * 1024 };// 40 * 1024 * 3
	HXMessagePackage() { bigDataPointer = 0; }
	//
	Header*			header() { return (Header*)data_; }
	const Header*	header() const { return (Header*)data_; }
	//数据块指针
	const char*		data() const { return data_; }
	char*			data() { return data_; }
	//数据块大小
	size_t			length() const { return header_length + body_length(); }
	//数据内容指针
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
	//数据内容大小
	size_t			body_length() const { return ((Header*)data_)->length; }
	void			body_length(size_t new_length){
		if (new_length > max_body_length)
			new_length = max_body_length;
		header()->length = (int)new_length;
	}
	//传递的参数
	void			linkid(LinkID id) { m_linkid = id; }
	LinkID			linkid() { return m_linkid; }
	const LinkID	linkid() const { return m_linkid; }
	LinkID			m_linkid;
	char			data_[header_length + max_body_length];
	char*			bigDataPointer;
};
#pragma pack(pop)

//消息队列结构
class HXMessageMap
{
public:
	HXMessageMap(){}
	virtual ~HXMessageMap(){}
	//返回一个消息指针
	virtual	const HXMessagePackage*	GetPackage(unsigned int index) = 0;
	//返回消息包数量
	virtual	int						GetCount() = 0;
};

struct BigPackageData
{
	unsigned int			length;
	char*					data;//动态分配内存
};

//大的消息包发送类
class HXBigMessagePackage: public HXMessageMap
{
public:
	enum ZipLevel {
		ZipLevel_None = 0,//不进行任何压缩
	};
	//添加一个大消息块
	virtual	void				SetBigPackage(unsigned short id, unsigned short id2, const void* ptr, int dataLength, ZipLevel zipLevel = ZipLevel_None) = 0;
	//允许大数据发送丢帧，如果内存大小超过maxOfMemory量，将允许不发送最新的大的消息包，等之前的数据包发送完毕再发送，最大不能超过4G
	virtual	void				SetAllowLostPackage(bool bAllowLost, unsigned int maxOfMemory) = 0;
	//是否允许丢帧
	virtual	bool				IsAllowLost() = 0;
	//返回最大缓存大小
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
