
// HXCameraViewerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "HXCameraViewer.h"
#include "HXCameraViewerDlg.h"
#include "NetImageShare.h"
#include <direct.h>
#include <io.h>
#include "afxdialogex.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma warning(disable: 4996)

bool	g_bCanSending = true;
int		g_cameraWidth = 1920;
int		g_cameraHeight = 1080;

CHXCameraViewerDlg*	g_viewer = 0;

HXBigMessagePackage* bigData = CreateBigPackage();

void __stdcall MyDecodeVideo(VideoPackage* pkt)
{
	g_viewer->OnDecodeVideo(pkt);
}

void __stdcall MyEncodeVideo(VideoPackage* pkt)
{
	HXLibClient* client = g_viewer->GetNetClient();
	if (client&&g_bCanSending) {

		int iHeadSize = sizeof(ImageHeader);

		char* sender = new char[pkt->size + iHeadSize];
		ImageHeader* header = (ImageHeader*)sender;
		header->width = g_cameraWidth;
		header->height = g_cameraHeight;
		header->fps = 30;
		header->dTimer = GetTimer()->GetTickTimer();
		header->size = pkt->size;
		header->timestamp = pkt->timestamp;
		header->sliceIndex = pkt->index;
		header->netPacketSize = pkt->size + iHeadSize;
		memcpy(sender + iHeadSize, pkt->data, pkt->size);
		bigData->SetBigPackage(id(ImageHeader), 0, header, header->netPacketSize);
		client->Send(*bigData);
		delete[] sender;
	}

}




// CAboutDlg dialog used for App About
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()



CHXCameraViewerDlg::CHXCameraViewerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_HXCAMERAVIEWER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_decoder = 0;
	m_encoder = 0;
	m_camera = 0;
	m_client = 0;
	g_viewer = this;
}



void CHXCameraViewerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SEND_BUTTON, m_btSender);
	DDX_Control(pDX, IDC_RECV_BUTTON, m_btReceiver);
}

BEGIN_MESSAGE_MAP(CHXCameraViewerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_SEND_BUTTON, &CHXCameraViewerDlg::OnBnClickedSendButton)
	ON_BN_CLICKED(IDC_RECV_BUTTON, &CHXCameraViewerDlg::OnBnClickedRecvButton)
END_MESSAGE_MAP()




BOOL CHXCameraViewerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	if (initParam() != 0)
	{
		SendMessage(WM_CLOSE);
	}
	
	pAVFastCodecService = CreateAVFastCodecService();

	return TRUE;  // return TRUE  unless you set the focus to a control
}


int CHXCameraViewerDlg::initParam()
{
	char drive[512], dir[512];
	_splitpath(_pgmptr, drive, dir, 0, 0);
	strcat(drive, dir);
	strcat(drive, "log/");

	//日志目录不存在，就创建它
	if (0 != _access(drive, 0))
	{
		if (0 != _mkdir(drive))
		{
			return MessageBox("创建日志目录失败！", "错误信息", MB_OK);
		}
	}

	/*****开启日志系统*****/
		SetupLogFiles(drive, true);

	//*****读取配置文件*****//
	m_confReader = CreateConfigReader();
	char path[MAX_PATH] = { 0 };
	_getcwd(path, MAX_PATH);
	strcat(path, "/CaptureConfig.xml");
	bool ret = m_confReader->OpenFile(path);
	if (!ret)
	{
		//LOG(LogError, "配置文件读取失败\n");
		return MessageBox("配置文件读取失败！", "错误信息", MB_OK);
	}

	g_cameraWidth = m_confReader->GetInt("root.CaptureParameter.Width");
	g_cameraHeight = m_confReader->GetInt("root.CaptureParameter.Height");
	mBitRate = m_confReader->GetInt("root.CaptureParameter.BitRate");
	mFPS = m_confReader->GetInt("root.CaptureParameter.FPS");

	m_confReader->GetStr("root.CenterServer.ip", mCenterServerIP);
	m_confReader->GetStr("root.CenterServer.port", mCenterServerPort);

	return 0;
}


void CHXCameraViewerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CHXCameraViewerDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CRect rc;
	GetClientRect(&rc);
	
	if (IsIconic())
	{
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		if (m_camera) 
		{
			m_camera->UpdateFromCamera(m_camera);
			//m_camera->MakeGreenMatting();
			if (m_buffers.size() == 0)
				m_buffers.resize(m_camera->ImageSize().X*m_camera->ImageSize().Y * 3);
			char* result = &m_buffers.at(0);
			//if (m_camera->GetPixels((color4*)result, m_camera->ImageSize().X, m_camera->ImageSize().Y)) {
			if (m_camera->GetPixels((void*)result, m_camera->ImageSize().X, m_camera->ImageSize().Y)) {
				static int index = 1;
				double dTime = GetTimer()->GetTickTimer();
				m_encoder->EncodeVideo(result, m_buffers.size(), false, index++, GetTimer()->GetTickTimer());
				double dDelay = GetTimer()->GetTickTimer() - dTime;
				int nPitch = m_image.GetPitch();
				for (int y = 0; y < m_image.GetHeight(); y++) {
					char* dst = (char*)m_image.GetBits() + y*nPitch;
					memcpy(dst, result + y*(int)m_camera->ImageSize().X * 3, m_image.GetWidth() * 3);
				}

				//count the average encoding delay time
				m_dDelayTime[m_nNumDelayTime % 100] = dDelay;
				m_nNumDelayTime++;
				double dDelayTime = 0;
				int nDelayTime = 0;
				for (int i = 0; i < m_nNumDelayTime; i++) {
					if (i >= 100)
						break;
					dDelayTime += m_dDelayTime[i];
					nDelayTime++;
				}

				//get the statistics data from image server
				Statistics& s = m_client->GetStatistics();

				//get the handle of device context for CImange
				HDC dcImage = m_image.GetDC();

				FillRect(dcImage, &CRect(0, 0, 500, 100), CBrush(RGB(0, 0, 0)));
				SetTextColor(dcImage, RGB(0, 255, 0));

				HGDIOBJ oldFont = SelectObject(dcImage, GetFont()->m_hObject);
				char temp[128];
				sprintf(temp, "网络延迟(ms):%.02f", m_client->GetNetDelay() * 1000);
				DrawText(dcImage, temp, -1, &CRect(0, 0, 500, 25), DT_TOP | DT_LEFT);
				sprintf(temp, "编码延迟(ms)单帧/平均:%.02f/%.02f", dDelay * 1000, dDelayTime / (double)nDelayTime * 1000);
				DrawText(dcImage, temp, -1, &CRect(0, 26, 500, 50), DT_TOP | DT_LEFT);
				sprintf(temp, "每秒发送包数:%d/", s.sendPackagesPerSecond);
				DrawText(dcImage, temp, -1, &CRect(0, 51, 500, 75), DT_TOP | DT_LEFT);
				sprintf(temp, "每秒发送字节(Mbps):%.02f", s.sendBytesPerSecond / (1024.0*1024.0));
				DrawText(dcImage, temp, -1, &CRect(0, 76, 500, 100), DT_TOP | DT_LEFT);
				if (oldFont)
					SelectObject(dcImage, oldFont);
				
				//release the handle of device context for CImange
				m_image.ReleaseDC();

			}
		}
		
		if (!m_image.IsNull())
		{
			HDC dcImage = m_image.GetDC();
			SetStretchBltMode(dc.m_hDC, COLORONCOLOR);
			SetStretchBltMode(dcImage, COLORONCOLOR);
			SetBkMode(dcImage, TRANSPARENT);

			StretchBlt(dc.m_hDC, 0, 0, rc.Width(), rc.Height(), dcImage, 0, 0, m_image.GetWidth(), m_image.GetHeight(), SRCCOPY);
			m_image.ReleaseDC();
			
		}

	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CHXCameraViewerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


BOOL CHXCameraViewerDlg::OnEraseBkgnd(CDC * pDC)
{
	return true;
}

void CHXCameraViewerDlg::OnTimer(UINT_PTR nIDEvent)
{
	static bool bIn = false;
	if (!bIn) {
		bIn = true;
		if (m_client) {
			HXMessageMap& mmap = m_client->SwapQueue();
			for (int i = 0; i < mmap.GetCount(); i++) {
				const HXMessagePackage* pack = mmap.GetPackage(i);
				switch (pack->header()->id)
				{
				case HXMessagePackage::Header::ID_Connect:
				{
					g_bCanSending = true;
					break;
				}
				case HXMessagePackage::Header::ID_ConnectFailure:
				{
					MessageBox("找不到图像传输服务器");
					break;
				}
				case id(ImageHeader):
				{
					ImageHeader* header = (ImageHeader*)pack->body();
				
					//videoFrameSlicePkt vfsp;
					//vfsp.data = (char*)header + sizeof(ImageHeader);
					//vfsp.slice_idx = header->sliceIndex;
					//vfsp.size = header->size;
					//vfsp.timestamp = header->timestamp;

					VideoPackage vp;
					vp.dTimer = header->dTimer;
					vp.data = (char*)header + sizeof(ImageHeader);
					vp.index = 0;
					vp.size = header->size;
					vp.timestamp = header->timestamp;

					if (m_image.IsNull() && !m_decoder) {
						m_image.Create(header->width, header->height, 24);
						CRect rc;
						GetWindowRect(&rc);
						rc.right = rc.left + header->width;
						rc.bottom = rc.top + header->height;
						MoveWindow(&rc);
						//m_decoder = pAVFastCodecService->CreateH264Decoder(MyDecodeVideo, header->width, header->height);
						m_decoder = pAVFastCodecService->CreateH265Decoder(MyDecodeVideo, header->width, header->height);

					}
					
					m_updateFPS.Add(1.0);

					//计算解码延迟时间
					double decStart = GetTimer()->GetTickTimer();
					//m_decoder->DecodeSliceVideo(&vfsp);
					m_decoder->DecodeVideo(&vp);
					m_decodeDelayTime = GetTimer()->GetTickTimer() - decStart;

					m_decodeDelayTimeArr[m_decodeDelayCount % 100] = m_decodeDelayTime;
					m_decodeDelayCount++;
					break;
				}
				}
			}
		}
		bIn = false;
	}
	if (nIDEvent == 0) {
		this->Invalidate();
	}
	CDialogEx::OnTimer(nIDEvent);
}


void CHXCameraViewerDlg::OnDestroy()
{
	//if (m_camera)
	//	GetSharedFaceMorph()->DeleteImage(m_camera);

	if (m_VideoCapturer)
	{
		delete m_VideoCapturer;
	}

	if (m_encoder)
		delete m_encoder;
	if (m_decoder)
		delete m_decoder;
	if (m_confReader)
		delete m_confReader;

	if (m_client)
	{
		m_client->Stop();
		delete m_client;
	}



	CDialogEx::OnDestroy();
}



void CHXCameraViewerDlg::OnBnClickedSendButton()
{
	if (m_camera)return;
	
	//m_camera = GetSharedFaceMorph()->OpenCamera(g_cameraWidth, g_cameraHeight);
	//g_cameraWidth = m_camera->VideoImageSize().X;
	//g_cameraHeight = m_camera->VideoImageSize().Y;

	m_VideoCapturer = CreateVideoCaptureService();
	m_camera = m_VideoCapturer->OpenCamera(g_cameraWidth, g_cameraHeight);
	g_cameraWidth = m_camera->VideoImageSize().X;
	g_cameraHeight = m_camera->VideoImageSize().Y;



	m_image.Create(g_cameraWidth, g_cameraHeight, 24);

	m_client = CreateNewClient();
	m_client->Start(mCenterServerIP, mCenterServerPort);

	CRect rc;
	GetWindowRect(&rc);
	rc.right = rc.left + g_cameraWidth;
	rc.bottom = rc.top + g_cameraHeight;
	MoveWindow(&rc);

	//m_encoder = pAVFastCodecService->CreateH264Encoder(MyEncodeVideo, g_cameraWidth, g_cameraHeight, mFPS, mBitRate);
	m_encoder = pAVFastCodecService->CreateH265Encoder(MyEncodeVideo, g_cameraWidth, g_cameraHeight, mFPS, mBitRate);

	SetTimer(0, 1, 0);
	m_btSender.ShowWindow(SW_HIDE);
	m_btReceiver.ShowWindow(SW_HIDE);
}


void CHXCameraViewerDlg::OnBnClickedRecvButton()
{
	if (m_decoder || m_client)
		return;
	m_client = CreateNewClient();
	m_client->Start(mCenterServerIP, mCenterServerPort);
	SetTimer(0, 1, 0);
	m_btSender.ShowWindow(SW_HIDE);
	m_btReceiver.ShowWindow(SW_HIDE);
}




void CHXCameraViewerDlg::OnDecodeVideo(VideoPackage * pkt)
{

	int pitch = m_image.GetPitch();
	int width = m_image.GetWidth();
	int height = m_image.GetHeight();
	for (int y = 0; y < height; y++) {
		memcpy((char*)m_image.GetBits() + y*pitch, (char*)pkt->data + y*width * 3, width * 3);
	}
	double dTimer = GetTimer()->GetTickTimer();
	m_nNumDelayTime++;

	
	//计算最近100帧的总延迟时间之和
	m_dDelayTime[m_nNumDelayTime % 100] = dTimer - pkt->dTimer;
	double dDelayTime = 0;
	int nDelayTime = 0;
	for (int i = 0; i < m_nNumDelayTime; i++) {
		if (i >= 100)
			break;
		dDelayTime += m_dDelayTime[i];
		nDelayTime++;
	}


	//计算最近100帧的解码延迟时间之和
	double dTotalDecTime = 0;
	int count = 0;
	for (int i = 0; i < m_decodeDelayCount; i++) {
		if (i >= 100)
			break;
		dTotalDecTime += m_decodeDelayTimeArr[i];
		count++;
	}


	Statistics& s = m_client->GetStatistics();

	HDC imageDC = m_image.GetDC();

	FillRect(imageDC, &CRect(0, 0, 500, 125), CBrush(RGB(0, 0, 0)));
	SetTextColor(imageDC, RGB(0, 255, 0));
	HGDIOBJ oldFont = SelectObject(imageDC, GetFont()->m_hObject);

	char temp[128];
	sprintf(temp, "网络延迟(ms):%.02f", m_client->GetNetDelay()* 1000);
	DrawText(imageDC, temp, -1, &CRect(0, 0, 500, 25), DT_TOP | DT_LEFT);

	//总延迟时间：从编码开始计时，经过网络传输、本地接收直至解码完成的计时时间
	sprintf(temp, "总延迟(ms)单帧/平均:%4.2f/%4.2f  FPS:%.0f", (dTimer - pkt->dTimer) *1000, dDelayTime / (double)nDelayTime * 1000, m_updateFPS.GetFPS());
	DrawText(imageDC, temp, -1, &CRect(0, 26, 500, 50), DT_TOP | DT_LEFT);
	sprintf(temp, "每秒接收包数:%d", s.recvPackagesPerSecond);
	DrawText(imageDC, temp, -1, &CRect(0, 51, 500, 75), DT_TOP | DT_LEFT);
	sprintf(temp, "每秒接收字节(Mbps):%.02f", s.recvBytesPerSecond / (1024.0*1024.0));
	DrawText(imageDC, temp, -1, &CRect(0, 76, 500, 100), DT_TOP | DT_LEFT);
	sprintf(temp, "解码延迟(ms)单帧/平均:%4.2f/%4.2f", m_decodeDelayTime * 1000, dTotalDecTime / (double)count * 1000);
	DrawText(imageDC, temp, -1, &CRect(0, 101, 500, 125), DT_TOP | DT_LEFT);
	
	if (oldFont)
		SelectObject(imageDC, oldFont);
	
	m_image.ReleaseDC();
	
	Invalidate();

}

