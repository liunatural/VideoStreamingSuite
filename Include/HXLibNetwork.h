
#pragma once 
// Socket�������˷�װ�࣬���Ķ��� �Ե���. 2017-11-30
#ifdef HXLIBNETWORK_EXPORTS
#define HXLIBNETWORK_API __declspec(dllexport)
#else
#define HXLIBNETWORK_API __declspec(dllimport)
#endif
#include "HXLibMessage.h"

//���LOG�����һ���ı�Ϊ8192�ֽڣ����һ��з���Ҫ�Լ�д��
#define	MAX_LOG_TEXT_LENGTH		8192
//���ӳ����ͬʱ��������
#define	MAX_ACCEPT_ITEM			10000
//��ȫɾ���ӿ�
#define	SAFE_DELETE_LIB(p)		{if(p)p->DeleteMe(); p = 0;}
//
struct Statistics {
	//���Ͳ���ͳ��
	unsigned int			sendPackages;//���͵����ݰ�������
	unsigned int			sendPackagesComplete;//���͵����ݰ��Ѿ���������
	int64_t					sendBytes;//���͵����ֽ�����
	unsigned int			sendPackagesPerSecond;//ÿ�뷢�Ͱ�
	unsigned int			sendPackagesPerSecondMax;//ÿ����߷��Ͱ�
	unsigned int			sendPackagesPerSecondAvg;//ÿ��ƽ�����Ͱ�
	double					sendBytesPerSecond;//ÿ�뷢���ֽ�
	double					sendBytesPerSecondMax;//���ÿ�뷢���ֽ�
	double					sendBytesPerSecondAvg;//ƽ��ÿ�뷢���ֽ�
	//
	double					sendQueueRetentionRate;//���Ͷ���������
												  //���ղ���ͳ��
	unsigned int			recvPackages;//���յ����ݰ�����
	int64_t					recvBytes;//���յ����ֽ�����
	unsigned int			recvPackagesPerSecond;//ÿ�뷢�Ͱ�
	unsigned int			recvPackagesPerSecondMax;//ÿ����߷��Ͱ�
	unsigned int			recvPackagesPerSecondAvg;//ÿ��ƽ�����Ͱ�
	double					recvBytesPerSecond;//ÿ�뷢���ֽ�
	double					recvBytesPerSecondMax;//���ÿ�뷢���ֽ�
	double					recvBytesPerSecondAvg;//ƽ��ÿ�뷢���ֽ�
	Statistics() { memset(this, 0, sizeof(Statistics)); }
};

//�ͻ�����ӿ�
class HXLibClient {
public:
	HXLibClient() {}
	virtual ~HXLibClient(){}
	//���ӷ��������ṩIP	�Ͷ˿ںţ�����0������������1���Ѿ��������ӣ�����1���绷���쳣
	virtual	int					Start(const char* ip, const char* port) = 0;
	//������Ϣ����������
	virtual	bool				Send(const HXMessagePackage& msg) = 0;
	//���ʹ����Ϣ����������
	virtual	bool				Send(HXBigMessagePackage& msg) = 0;
	//������Ϣ����������
	virtual	bool				Send(unsigned short id, unsigned short id2, const char* data, unsigned int datalength) = 0;
	template <typename T>bool	Send(unsigned short id, T& data) { return Send(id, 0, (const char*)&data, sizeof(T)); }
	//�ر��������������
	virtual	void				Stop() = 0;
	//����һ����Ϣ�����е����ݣ����ҷ��ظ����̣߳����߳̿���ѭ�����ã���ʹ�������κ���Ϣ����ҲҪ������Ϣ�������
	virtual	HXMessageMap&		SwapQueue() = 0;
	//����ͳ����Ϣ
	virtual Statistics&			GetStatistics() = 0;
	//������������������ӳ�
	virtual double				GetNetDelay() = 0;
};

//��������ӿ�
class HXLibService {
public:
	HXLibService(void){}
	virtual ~HXLibService(){}
	//�����������ָ��һ���˿ں�,maxLinks���������������������ܳ���MAX_ACCEPT_ITEM������
	virtual	bool				Start(int port, int maxLinks = 1000) = 0;
	//�ر��������
	virtual	void				Stop() = 0;
	//����һ����Ϣ�����е����ݣ����ҷ��ظ����̣߳����߳̿���ѭ������
	virtual	HXMessageMap&		SwapQueue() = 0;
	//������Ϣ���������falseֻ��������ʧЧ��ʱ��Żᱻ����
	virtual	bool				Send(LinkID linkid, const HXMessagePackage& msg) = 0;
	//���ʹ����Ϣ���������falseֻ��������ʧЧ��ʱ��Żᱻ����
	virtual	bool				Send(LinkID linkid, HXBigMessagePackage& msg) = 0;
	//������Ϣ����������
	virtual	bool				Send(LinkID linkid, unsigned short id, unsigned short id2, const char* data, unsigned int datalength) = 0;
	template <typename T>bool	Send(LinkID linkid, unsigned short id, T& data) { return Send(linkid, id, 0, (const char*)&data, sizeof(T)); }
	//������Ϣ���������falseֻ��������ʧЧ��ʱ��Żᱻ����
	virtual	bool				Send(LinkID linkid, unsigned short id, streamWriter& msg) {
		return Send(linkid, id, 0, msg.buffer, msg.length);
	}
	//��������������
	virtual void				SetContext(LinkID linkid, const char* context) = 0;
	//��ȡ����������������false��ʾ���ӿ����Ѿ��Ͽ�
	virtual bool				GetContext(LinkID linkid, const char** result) = 0;
	//�ر�һ���ͻ�����
	virtual void				Close(LinkID linkid) = 0;
	//����ͼ�����ݣ��Ժ���չ
	//virtual	void	SendImage(unsigned int linkid, const char* pixel, int width, int height, int pitch);
	//�������Ƿ��Ѿ�������״̬
	virtual	bool				IsRunning() = 0;
	//����ͳ����Ϣ
	virtual Statistics&			GetStatistics() = 0;
};

//�����ļ���ȡ���ܣ���־
class HXLibConfigService
{
public:
	HXLibConfigService(){}
	virtual ~HXLibConfigService(){}
	//��XML�ļ������жϷ���ֵ
	virtual bool				OpenFile(const char* file) = 0;
	//����·����ȡXML�ڵ���ַ���ֵ
	virtual bool				GetStr(const char* path, char* rel) = 0;
	//����·����ȡXML�ڵ������ֵ
	virtual int					GetInt(const char* path) = 0;
	//����·����ȡXML�ڵ�ĸ�����ֵ
	virtual double				GetDouble(const char* path) = 0;
};

HXLIBNETWORK_API HXLibConfigService* CreateConfigReader();

class HXLibTimer
{
public:
	//����ʼʱ��ʱ��ʼ
	HXLibTimer(){}
	virtual ~HXLibTimer(){}
	//���¿�ʼ��ʱ
	virtual	void				RestartTimer() = 0;
	//���ص�ǰʱ�䣬����Ϊ��λ
	virtual	double				GetTickTimer() = 0;
};

//����FPS����
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

//�ٽ�������
class HXLIBNETWORK_API HXLibCritical
{
public:
	HXLibCritical();
	~HXLibCritical();
	//����
	void			Enter();
	//����
	void			Leave();
	struct Lock {
		Lock(HXLibCritical& _c) { c = &_c; c->Enter(); }
		~Lock() { c->Leave(); }
		HXLibCritical* c;
	};
protected:
	void*			sect;
};

//���߳̿���
class HXLIBNETWORK_API HXLibThread
{
public:
	HXLibThread();
	virtual ~HXLibThread();
	bool				OpenThread(const void* arg);
	bool				CloseThread(int timeOut = -1);
	//�Ƿ��Ǽ���״̬,���Գ���ѭ��
	inline	bool		IsAlive() { return m_bAlive; }
protected:
	//��Ҫ�̳е��߳���
	virtual void		OnThread(bool& bAlive, const void* arg){}
	static unsigned int __stdcall	HXThreadFunction(void* lpThreadParameter);
protected:
	const void*			m_arg;
	void*				m_hThread;
private:
	bool				m_bAlive;
	bool				m_bClosedInThread;
};

//���߳�ͬ������
class HXLibMessageQueueAB
{
public:
	HXLibMessageQueueAB() {}
	virtual ~HXLibMessageQueueAB() {}
	//�����Ϣ
	virtual void						Push(const HXMessagePackage& msg) = 0;
	//������ǰ��Ϣ����
	virtual HXMessageMap&				Swap() = 0;
	//���ص�ǰ���߳̿���ʹ�õĶ���
	virtual HXMessageMap&				Get() = 0;
};


//����һ��������
HXLIBNETWORK_API HXLibService*			CreateNewService(void);
//����һ���ͻ���
HXLIBNETWORK_API HXLibClient*			CreateNewClient(void);
//����һ�������Ϣ����
HXLIBNETWORK_API HXBigMessagePackage*	CreateBigPackage(void);
//����ʱ�Ӽ�ʱ��
HXLIBNETWORK_API HXLibTimer*			GetTimer(void);
//����ʱ�Ӽ�ʱ��
HXLIBNETWORK_API HXLibTimer*			CreateTimer(void);
//����ʱ�Ӽ�ʱ��
HXLIBNETWORK_API HXLibMessageQueueAB*	CreateMessageQueue(void);
//
enum LogChannel{
	//Ĭ�ϵ���־����ļ�
	LogDefault = 0,
	LogError,
	LogDebug,
	LogChannel_Count,
};
struct LogChannelInfo {
	bool			bOutput;//�Ƿ�������ļ�
	bool			bConsole;//�Ƿ����������̨
	char			name[32];//��־ͨ������
	char			index;
	LogChannelInfo() {
		bOutput = true;
		bConsole = true;
		name[0] = 0;
		index = 0;
	}
};
typedef void(__stdcall* OnLogCallback)(int channel, const char* text, int length);
//һ��ֻ�ᴴ��һ��Log�ļ������������������Զ�׷�ӵ��Ǹ��ļ��У�ÿ������Log���ԡ�StartLogSystem����ͷ�͡�StopLogSystem������
HXLIBNETWORK_API void			SetupLogFiles(const char* szPath, bool bUseDebugLog);
//�Ƿ�ͬʱ���������̨printf��
HXLIBNETWORK_API void			SetLogInfo(int channel/*LogChannel*/, const LogChannelInfo& info);
HXLIBNETWORK_API void			SetLogCallback(OnLogCallback cb);
//������־����������ʱ��֧��
//HXLIBNETWORK_API void			SetLogServer(const char* ip, int nPort);
//д����־
HXLIBNETWORK_API void			LOG(int channel, const char* format, ...);
HXLIBNETWORK_API void			SleepInThread(int millisecond);
