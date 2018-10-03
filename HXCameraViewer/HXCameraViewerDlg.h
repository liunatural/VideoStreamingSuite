
// HXCameraViewerDlg.h : header file
//

#pragma once
//#include "HXLibFaceMorph.h"
#include "AVFastCodecLib.h"
#include "HXLibNetwork.h"
#include <atlimage.h>
#include <vector>
#include "afxwin.h"

#include "VideoCapturerLib.h"



// CHXCameraViewerDlg dialog
class CHXCameraViewerDlg : public CDialogEx
{
// Construction
public:
	CHXCameraViewerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HXCAMERAVIEWER_DIALOG };
#endif

	void	OnDecodeVideo(VideoPackage* pkt);
	HXLibClient*	GetNetClient() { return m_client; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedSendButton();
	afx_msg void OnBnClickedRecvButton();

private:

	int initParam();


protected:
	HICON m_hIcon;
private:
	CButton m_btSender;
	CButton m_btReceiver;
	//HXLibImage*				m_camera;

	VideoCapturer*		m_VideoCapturer;
	ImageManager*		m_camera;


	CImage					m_image;

	AVFastCodecService *pAVFastCodecService;
	Encoder*		m_encoder;
	Decoder*		m_decoder;

	std::vector<char>		m_buffers;
	HXLibClient*	m_client;
	HXLibConfigService* m_confReader;
	HXFPSTimer				m_updateFPS;
	double					m_dDelayTime[100];
	int						m_nNumDelayTime;

	double	m_decodeDelayTime = 0.0;
	double	m_decodeDelayTimeArr[100] = {0.0};
	int			m_decodeDelayCount = 0;


	char mCenterServerIP[50] = {0};
	char mCenterServerPort[50] = {0};
	int mBitRate = 0;
	int mFPS = 0;

};
