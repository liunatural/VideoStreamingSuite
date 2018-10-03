
#pragma once 
// Socket服务器端封装类，恒心东方 赵德贤. 2017-11-30
#ifdef HXLIBNETWORK_EXPORTS
#define HXLIBNETWORK_API __declspec(dllexport)
#else
#define HXLIBNETWORK_API __declspec(dllimport)
#endif
#include "HXLibMessage.h"

//最大LOG输出的一行文本为8192字节，并且换行符需要自己写入
#define	MAX_LOG_TEXT_LENGTH		8192
//连接池最大同时在线数量
#define	MAX_ACCEPT_ITEM			10000
//安全删除接口
#define	SAFE_DELETE_LIB(p)		{if(p)p->DeleteMe(); p = 0;}
//
struct Statistics {
	//发送部分统计
	unsigned int			sendPackages;//发送的数据包总数量
	unsigned int			sendPackagesComplete;//发送的数据包已经发送数量
	int64_t					sendBytes;//发送的总字节数量
	unsigned int			sendPackagesPerSecond;//每秒发送包
	unsigned int			sendPackagesPerSecondMax;//每秒最高发送包
	unsigned int			sendPackagesPerSecondAvg;//每秒平均发送包
	double					sendBytesPerSecond;//每秒发送字节
	double					sendBytesPerSecondMax;//最高每秒发送字节
	double					sendBytesPerSecondAvg;//平均每秒发送字节
	//
	double					sendQueueRetentionRate;//发送队列滞留率
												  //接收部分统计
	unsigned int			recvPackages;//接收的数据包数量
	int64_t					recvBytes;//接收的总字节数量
	unsigned int			recvPackagesPerSecond;//每秒发送包
	unsigned int			recvPackagesPerSecondMax;//每秒最高发送包
	unsigned int			recvPackagesPerSecondAvg;//每秒平均发送包
	double					recvBytesPerSecond;//每秒发送字节
	double					recvBytesPerSecondMax;//最高每秒发送字节
	double					recvBytesPerSecondAvg;//平均每秒发送字节
	Statistics() { memset(this, 0, sizeof(Statistics)); }
};

//客户端类接口
class HXLibClient {
public:
	HXLibClient() {}
	virtual ~HXLibClient(){}
	//链接服务器，提供IP	和端口号，返回0是正常，返回1是已经存在连接，大于1网络环境异常
	virtual	int					Start(const char* ip, const char* port) = 0;
	//发送消息给服务器端
	virtual	bool				Send(const HXMessagePackage& msg) = 0;
	//发送大的消息给服务器端
	virtual	bool				Send(HXBigMessagePackage& msg) = 0;
	//发送消息给服务器端
	virtual	bool				Send(unsigned short id, unsigned short id2, const char* data, unsigned int datalength) = 0;
	template <typename T>bool	Send(unsigned short id, T& data) { return Send(id, 0, (const char*)&data, sizeof(T)); }
	//关闭与服务器的链接
	virtual	void				Stop() = 0;
	//交换一次消息队列中的数据，并且返回给主线程，主线程可以循环调用，即使不处理任何消息接收也要进行消息队列清空
	virtual	HXMessageMap&		SwapQueue() = 0;
	//返回统计信息
	virtual Statistics&			GetStatistics() = 0;
	//返回与服务器的网络延迟
	virtual double				GetNetDelay() = 0;
};

//服务器类接口
class HXLibService {
public:
	HXLibService(void){}
	virtual ~HXLibService(){}
	//开启网络服务，指定一个端口号,maxLinks设置最大可链接数量，不能超过MAX_ACCEPT_ITEM个数量
	virtual	bool				Start(int port, int maxLinks = 1000) = 0;
	//关闭网络服务
	virtual	void				Stop() = 0;
	//交换一次消息队列中的数据，并且返回给主线程，主线程可以循环调用
	virtual	HXMessageMap&		SwapQueue() = 0;
	//发送消息，如果返回false只有在链接失效的时候才会被触发
	virtual	bool				Send(LinkID linkid, const HXMessagePackage& msg) = 0;
	//发送大块消息，如果返回false只有在链接失效的时候才会被触发
	virtual	bool				Send(LinkID linkid, HXBigMessagePackage& msg) = 0;
	//发送消息给服务器端
	virtual	bool				Send(LinkID linkid, unsigned short id, unsigned short id2, const char* data, unsigned int datalength) = 0;
	template <typename T>bool	Send(LinkID linkid, unsigned short id, T& data) { return Send(linkid, id, 0, (const char*)&data, sizeof(T)); }
	//发送消息，如果返回false只有在链接失效的时候才会被触发
	virtual	bool				Send(LinkID linkid, unsigned short id, streamWriter& msg) {
		return Send(linkid, id, 0, msg.buffer, msg.length);
	}
	//设置链接器参数
	virtual void				SetContext(LinkID linkid, const char* context) = 0;
	//获取连接器参数，返回false表示链接可能已经断开
	virtual bool				GetContext(LinkID linkid, const char** result) = 0;
	//关闭一个客户链接
	virtual void				Close(LinkID linkid) = 0;
	//发送图像数据，以后扩展
	//virtual	void	SendImage(unsigned int linkid, const char* pixel, int width, int height, int pitch);
	//服务器是否已经在运行状态
	virtual	bool				IsRunning() = 0;
	//返回统计信息
	virtual Statistics&			GetStatistics() = 0;
};

//配置文件读取功能，刘志
class HXLibConfigService
{
public:
	HXLibConfigService(){}
	virtual ~HXLibConfigService(){}
	//打开XML文件，需判断返回值
	virtual bool				OpenFile(const char* file) = 0;
	//根据路径获取XML节点的字符串值
	virtual bool				GetStr(const char* path, char* rel) = 0;
	//根据路径获取XML节点的整型值
	virtual int					GetInt(const char* path) = 0;
	//根据路径获取XML节点的浮点型值
	virtual double				GetDouble(const char* path) = 0;
};

HXLIBNETWORK_API HXLibConfigService* CreateConfigReader();

class HXLibTimer
{
public:
	//程序开始时计时开始
	HXLibTimer(){}
	virtual ~HXLibTimer(){}
	//重新开始计时
	virtual	void				RestartTimer() = 0;
	//返回当前时间，以秒为单位
	virtual	double				GetTickTimer() = 0;
};

//进行FPS计算
class HXLIBNETWORK_API HXFPSTimer
{
public:
	HXFPSTimer();
	~HXFPSTimer();
	void						Add(double dAdd);
	inline	double				GetFPS() { return m_dFPS; }
	inline	double				GetFPSMax() { return m_dFPSMax; }
	inline	double				GetFPSAvg() { return m_dFPSAvg; }
protected:
	double						m_dLastTime;
	double						m_dAvgDuration;
	double						m_dAlpha;
	double						m_dFPS;
	double						m_dFPSMax;
	double						m_dFPSAvg;
	double						m_dFrameAdd;
};

//临界锁定类
class HXLIBNETWORK_API HXLibCritical
{
public:
	HXLibCritical();
	~HXLibCritical();
	//锁定
	void			Enter();
	//解锁
	void			Leave();
	struct Lock {
		Lock(HXLibCritical& _c) { c = &_c; c->Enter(); }
		~Lock() { c->Leave(); }
		HXLibCritical* c;
	};
protected:
	void*			sect;
};

//多线程开启
class HXLIBNETWORK_API HXLibThread
{
public:
	HXLibThread();
	virtual ~HXLibThread();
	bool				OpenThread(const void* arg);
	bool				CloseThread(int timeOut = -1);
	//是否还是激活状态,可以持续循环
	inline	bool		IsAlive() { return m_bAlive; }
protected:
	//需要继承的线程类
	virtual void		OnThread(bool& bAlive, const void* arg){}
	static unsigned int __stdcall	HXThreadFunction(void* lpThreadParameter);
protected:
	const void*			m_arg;
	void*				m_hThread;
private:
	bool				m_bAlive;
	bool				m_bClosedInThread;
};

//多线程同步队列
class HXLibMessageQueueAB
{
public:
	HXLibMessageQueueAB() {}
	virtual ~HXLibMessageQueueAB() {}
	//添加消息
	virtual void						Push(const HXMessagePackage& msg) = 0;
	//交换当前消息队列
	virtual HXMessageMap&				Swap() = 0;
	//返回当前主线程可以使用的队列
	virtual HXMessageMap&				Get() = 0;
};


//创建一个服务类
HXLIBNETWORK_API HXLibService*			CreateNewService(void);
//创建一个客户类
HXLIBNETWORK_API HXLibClient*			CreateNewClient(void);
//创建一个大的消息包类
HXLIBNETWORK_API HXBigMessagePackage*	CreateBigPackage(void);
//返回时钟计时器
HXLIBNETWORK_API HXLibTimer*			GetTimer(void);
//创建时钟计时器
HXLIBNETWORK_API HXLibTimer*			CreateTimer(void);
//创建时钟计时器
HXLIBNETWORK_API HXLibMessageQueueAB*	CreateMessageQueue(void);
//
enum LogChannel{
	//默认的日志输出文件
	LogDefault = 0,
	LogError,
	LogDebug,
	LogChannel_Count,
};
struct LogChannelInfo {
	bool			bOutput;//是否输出到文件
	bool			bConsole;//是否输出到控制台
	char			name[32];//日志通道名字
	char			index;
	LogChannelInfo() {
		bOutput = true;
		bConsole = true;
		name[0] = 0;
		index = 0;
	}
};
typedef void(__stdcall* OnLogCallback)(int channel, const char* text, int length);
//一天只会创建一个Log文件，如果程序重启则会自动追加到那个文件中，每个程序Log都以“StartLogSystem”开头和“StopLogSystem”结束
HXLIBNETWORK_API void			SetupLogFiles(const char* szPath, bool bUseDebugLog);
//是否同时输出到控制台printf中
HXLIBNETWORK_API void			SetLogInfo(int channel/*LogChannel*/, const LogChannelInfo& info);
HXLIBNETWORK_API void			SetLogCallback(OnLogCallback cb);
//设置日志服务器，暂时不支持
//HXLIBNETWORK_API void			SetLogServer(const char* ip, int nPort);
//写入日志
HXLIBNETWORK_API void			LOG(int channel, const char* format, ...);
HXLIBNETWORK_API void			SleepInThread(int millisecond);
