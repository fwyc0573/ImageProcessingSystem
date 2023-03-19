
// MFCpictryDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MFCpictry.h"
#include "MFCpictryDlg.h"
#include "afxdialogex.h"
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <windows.h>
using namespace cv;
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

Mat org ;
Mat Current_pic;//正在处理的图像
Mat Temp_pic;//上一步处理的情况

Mat samll_pic;//小框预处理图像
Mat imagedst; //适应框中尺寸的图像
Mat result ;//改变参数以后的处理效果（未点击叠加时候的临时图像）
int can_show;
bool can_back;//限制回调函数的使用

bool  can_setup;//摄像头开关
Mat frem;//摄像头画面
Mat srcImage;
Vec3b pixel_use;
bool can_detect;
int can_draw;
int line_thickness;


class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// CMFCpictryDlg 对话框

CMFCpictryDlg::CMFCpictryDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMFCpictryDlg::IDD, pParent)
	, m_cover(0)
	, x_value(0)
	, y_value(0)
	, size_value(1.0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCpictryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PICTURE, m_PictureControl);
	DDX_Control(pDX, IDC_SLIDER1, x_control);
	DDX_Slider(pDX, IDC_SLIDER1, x_value);
	DDX_Control(pDX, IDC_SLIDER2, y_control);
	DDX_Slider(pDX, IDC_SLIDER2, y_value);
	DDX_Text(pDX, IDC_EDIT1, size_value);
	DDX_Control(pDX, IDC_EDIT1, size_control);
	DDX_Control(pDX, IDC_SLIDER3, angle_control);
}

BEGIN_MESSAGE_MAP(CMFCpictryDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CMFCpictryDlg::OnBnClickedButton1)
	ON_COMMAND(ID_32771, &CMFCpictryDlg::On32771)
	ON_BN_CLICKED(IDC_BUTTON2, &CMFCpictryDlg::OnMeanShiftFilter_binary)
	ON_BN_CLICKED(IDC_BUTTON3, &CMFCpictryDlg::OnBack)
	ON_BN_CLICKED(IDC_BUTTON4, &CMFCpictryDlg::OnBinarization)
	ON_BN_CLICKED(IDC_BUTTON5, &CMFCpictryDlg::OnDiedaithresh_Cut)
	ON_BN_CLICKED(IDC_BUTTON6, &CMFCpictryDlg::OnGetGrayObj)
	ON_COMMAND(ID_32773, &CMFCpictryDlg::OnSavepic)
	ON_COMMAND(ID_32772, &CMFCpictryDlg::OnAverageFilter)
	ON_COMMAND(ID_32774, &CMFCpictryDlg::OnMiddleFilter)
	ON_BN_CLICKED(IDC_BUTTON7, &CMFCpictryDlg::OnCoverPic)
	ON_BN_CLICKED(IDC_BUTTON8, &CMFCpictryDlg::OnWaitTosolve)
	ON_COMMAND(ID_32775, &CMFCpictryDlg::OnCamSetup)
	ON_COMMAND(ID_32776, &CMFCpictryDlg::OnCamCrenncut)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_CHECK1, &CMFCpictryDlg::OnBnClickedCheck1)
	ON_COMMAND(ID_32779, &CMFCpictryDlg::OnDetectPixel)
	ON_COMMAND(ID_32780, &CMFCpictryDlg::OnCloseDetect)
	ON_BN_CLICKED(IDC_CHECK2, &CMFCpictryDlg::OnBnClickedCheck2)
END_MESSAGE_MAP()


// CMFCpictryDlg 消息处理程序

BOOL CMFCpictryDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
     can_back = false;
	 can_show = 0;
	 can_detect = false;
	 can_show = 0;
	// 将“关于...”菜单项添加到系统菜单中。
    m_Menu.LoadMenu(IDR_MENU1);
	SetMenu(&m_Menu);
	pixel_use = 0;
	line_thickness = 1;
	x_control.SetRange(-200,200);//设置滑动范围
	x_control.SetTicFreq(2);//每2个单位画一刻度
	y_control.SetRange(-200,200);//设置滑动范围
	y_control.SetTicFreq(2);//每2个单位画一刻度
	angle_control.SetRange(-180,180);//设置滑动范围
	angle_control.SetTicFreq(2);//每2个单位画一刻度

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	namedWindow("view",WINDOW_AUTOSIZE);
	namedWindow("view_src",WINDOW_AUTOSIZE);
	HWND hWnd = (HWND) cvGetWindowHandle("view");
	HWND hWnd_src = (HWND) cvGetWindowHandle("view_src");
	HWND hParent = ::GetParent(hWnd);
	HWND hParent_src = ::GetParent(hWnd_src);
	::SetParent(hWnd, GetDlgItem(IDC_PICTURE)->m_hWnd);
	::SetParent(hWnd_src, GetDlgItem(IDC_PICTURE_src)->m_hWnd);
	::ShowWindow(hParent, SW_HIDE);
	::ShowWindow(hParent_src, SW_HIDE);

	((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(TRUE);//选上
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMFCpictryDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMFCpictryDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

HCURSOR CMFCpictryDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void on_mouse(int event, int x, int y, int flags, void *ustc)//event鼠标事件，x,y是鼠标坐标，flags是拖拽和键盘操作    
{
	if(can_back)
	{
	static vector<Point> vctPoint;
	static vector<vector<Point>> vctvctPoint;
    static Point ptStart = (-1, -1); //初始化起点
    static Point cur_pt = (-1, -1);  //初始化临时节点
    char temp[16];
	Mat dst, maskImage;
	if (event == CV_EVENT_LBUTTONDOWN)
	{
		ptStart = Point(x, y);
		vctPoint.push_back(ptStart);
        //circle(org, ptStart, 1, cv::Scalar(0, 0, 0), CV_FILLED, CV_AA, 0);
		//imshow("view", org);
		//cv::putText(tmp, temp, ptStart, cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0, 0), 1, 8);
	}
	else if (event == CV_EVENT_MOUSEMOVE && (flags & CV_EVENT_FLAG_LBUTTON))
	{
		cur_pt = Point(x, y);
		line(org, vctPoint.back(), cur_pt, Scalar(0, 0, 0, 0), 2, 8, 0);
		//circle(org, cur_pt, 1, Scalar(0, 0, 0), CV_FILLED, CV_AA, 0);
		imshow("view", org);
		vctPoint.push_back(cur_pt);
		//cv::putText(tmp, temp, cur_pt, cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0, 0));
	}
	else if (event == CV_EVENT_LBUTTONUP)
	{	
		cur_pt = Point(x, y);
		line(org, ptStart, cur_pt, Scalar(0, 0, 0, 0), 2, 8, 0);
		//circle(org, cur_pt, 1, Scalar(0, 0, 0), CV_FILLED, CV_AA, 0);
		imshow("view", org);
		vctPoint.push_back(cur_pt);
		vctvctPoint.push_back(vctPoint);   
		//把点构成任意多边形进行填充
		const Point * ppt[1] = { &vctPoint[0] };//取数组的首地址
		int len = vctPoint.size();
		int npt[] = { len };
	//	cv::polylines(org, ppt, npt, 1, 1, cv::Scalar(0,0, 0, 0), 1, 8, 0);	    
		org.copyTo(maskImage);
		maskImage.setTo(cv::Scalar(0, 0, 0, 0));
		fillPoly(maskImage, ppt, npt, 1, Scalar(255, 255, 255));	
		org.copyTo(dst ,maskImage);
		imshow("view",dst);
		Current_pic = dst ;
		vctPoint.clear();
		vctvctPoint.clear();
	 	len = 0;
		can_back = false;
	 }
   }
}

void MouseEvent(int event, int x, int y, int flags, void* data)//用于检测像素点的相关信息
{
 if(can_detect){
    char charText[30];
    Mat tempImage, hsvImage;
    tempImage = srcImage.clone();
    // 左键按下移动显示RGB信息
    if (event == CV_EVENT_LBUTTONDOWN)
    {
        Vec3b p = tempImage.at<Vec3b>(y,x);
        sprintf(charText, "R=%d, G=%d, B=%d", p[2], p[1], p[0]);
        // 写RGB文本信息到图像
	    putText(tempImage, charText, Point(8,20),
	    5, 1, CV_RGB(100,100,100));
    }
    // 右键按下移动显示HSV信息
    else if (event == CV_EVENT_RBUTTONDOWN)
    {
        // 转换成HSV图像
        cvtColor(tempImage, hsvImage, CV_BGR2HSV);
        Vec3b p = hsvImage.at<Vec3b>(y,x);
        // 获取相应的通道值
        sprintf(charText, "H=%d, S=%d, V=%d", p[0], p[1], p[2]);
        // 写HSV文本信息到图像
    	putText(tempImage, charText, Point(8,20),
        5, 1, CV_RGB(100,100,100));

    }
	else if(event == CV_EVENT_LBUTTONDBLCLK)
	{
		//防止越界
		if(x<1)
			x = 1;
		if(x>970)
			x = 970;
		if(y<1)
			y = 1;
		if(y > 660 )
			y = 660;
		for(int m = 0 ; m < 3; m++)
		{
			pixel_use[m] = (3*tempImage.at<Vec3b>(y,x)[m] + tempImage.at<Vec3b>(y,x-1)[m] + tempImage.at<Vec3b>(y,x+1)[m] + tempImage.at<Vec3b>(y-1,x)[m]+ tempImage.at<Vec3b>(y+1,x)[m])/7;
		}
		
		AfxMessageBox("采样成功！");

	}
    // 没有按下键显示对应坐标
    else
    {
	    // 移动鼠标显示坐标
	    sprintf(charText, "x=%d, y=%d", x, y);
	    // 写XY坐标文本信息到图像
    	putText(tempImage, charText, Point(8,20),
        5, 1, CV_RGB(100,100,100));

	}
        imshow("view", tempImage);
	}
}

void MouseDraw(int event, int x, int y, int flags, void* data)
{
	if(can_draw)
	{
		 static vector<Point> vctPoint;
	     static vector<vector<Point>> vctvctPoint;
         static Point ptStart = (-1, -1); //初始化起点
         static Point cur_pt = (-1, -1);  //初始化临时节点 
		
	     if(event == CV_EVENT_LBUTTONDOWN)
	    {
		    ptStart = Point(x, y);
		    vctPoint.push_back(ptStart);
     	}
		else if (event == CV_EVENT_MOUSEMOVE && (flags & CV_EVENT_FLAG_LBUTTON))
	    {
		cur_pt = Point(x, y);
		line(org, vctPoint.back(), cur_pt, Scalar(pixel_use[0], pixel_use[1],pixel_use[2]), line_thickness, 8, 0);
		imshow("view", org);
		vctPoint.push_back(cur_pt);
	    }
		else if (event == CV_EVENT_MBUTTONDOWN)//点击滚轮画笔变细
		{
		   if(line_thickness>0)
			    line_thickness--;
		}
		else if (event == CV_EVENT_RBUTTONDBLCLK)//双击右键画笔变粗
		{
		  if(line_thickness<10)
		     	line_thickness++;
		}
	}
}

void CMFCpictryDlg::OnBnClickedButton1() //手动剪裁
{    
	can_back = true;
	Temp_pic =  Current_pic.clone();
	org  = Current_pic.clone();
	imshow("view", org);
	setMouseCallback("view", on_mouse, 0);//调用回调函数    
	cv::imshow("view", org);

}

//加载图像
void CMFCpictryDlg::On32771()
{
	CString picPath;   //定义图片路径变量
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT|OFN_ALLOWMULTISELECT, 
		NULL, this);   //选择文件对话框
	if(dlg.DoModal() == IDOK)
	{
		picPath= dlg.GetPathName();  //获取图片路径
	}
	//CString to string  使用这个方法记得字符集选用“使用多字节字符”，不然会报错
	string picpath=picPath.GetBuffer(0);  
    Mat image_src = imread(picpath);   
	Mat image = image_src.clone();
	

	//以下操作获取图形控件尺寸并以此改变图片尺寸
	CRect rect;
	
	GetDlgItem(IDC_PICTURE)->GetClientRect(&rect);
	Rect dst(rect.left,rect.top,rect.right,rect.bottom);
	resize(image,imagedst,cv::Size(rect.Width(),rect.Height())); 
	imshow("view",imagedst);
	org = imagedst.clone();
	Current_pic = imagedst.clone();
	Temp_pic = Current_pic.clone();
	can_back  = false;
}

//均值漂移滤波（二值化部分省略了）
void CMFCpictryDlg::OnMeanShiftFilter_binary()
{
	can_back = false;
	Temp_pic = Current_pic.clone();

	////均值漂移滤波及二值化
	Mat shifted, gray, binary, src_clone;
	src_clone = Current_pic.clone();
	pyrMeanShiftFiltering(src_clone, shifted, 25, 55);
	//cvtColor(shifted, shifted, CV_BGR2GRAY);
	//threshold(shifted, shifted, 0, 255, THRESH_BINARY | THRESH_OTSU);
	//cvtColor(shifted, shifted, COLOR_GRAY2RGB);
	Current_pic = shifted;
	imshow("view", shifted);

}

void CMFCpictryDlg::OnBack()
{
	Current_pic = Temp_pic;
	imshow("view",Current_pic);
}

Mat Binarization(Mat binary,int thresh)
{
	for(int i = 0;i< binary.rows;i++)
	 {
		for(int j = 0 ;j<binary.cols;j++)
		{
			if(binary.at<uchar>(i,j) > thresh)
			{
					binary.at<uchar>(i,j) = 0;
			}
           else if(binary.at<uchar>(i,j) <= thresh && binary.at<uchar>(i,j)!=0)
			{
					binary.at<uchar>(i,j) = 150;
		    }
	 }
  }
	return binary;
}

void CMFCpictryDlg::OnBinarization()//自定义二值化
{
	can_back = false;
	Temp_pic = Current_pic.clone();
	int thresh = 0;
	Mat binary = Current_pic.clone();
	cvtColor(binary, binary, CV_BGR2GRAY);//转为灰度图
	binary = Binarization(binary,thresh);//返回二值化结果
	cvtColor(binary, binary, COLOR_GRAY2RGB);
	Current_pic = binary;
	imshow("view",Current_pic);

}

bool cmp(int x,int y)
{
    return x>y;
}

Mat IsodataSeg(Mat &src)
{
    int T = 0;
    int threshold = 0;                             //历史阈值
    int Sumrightpart = 0;                      //大于阈值部分
    int nObject = 0;                              //大于部分像素个数
    int Sumleftpart = 0;                       //小于阈值部分
    int nBack = 0;                                //小于部分像素个数
	cvtColor(src, src, CV_BGR2GRAY);//转为灰度图
	int nCol = src.cols * src.channels() ;       //图像每行的像素个数
	float stopcondition = 0.5;//迭代停止条件

	//用于寻找初始中值T
	LONG pixelSum = nCol*src.rows;//总像素个数
	int *pixelStore;
	pixelStore = new int[pixelSum];
	int clock = 0;
   for (int i=0; i<src.rows; ++i)
   {
	   uchar* pData = src.ptr<uchar>(i);
	   for (int j=0; j<nCol; ++j)
	   {
		   if(pData[j]!=0)
			    pixelStore[clock++] = pData[j];//将所有像素值都存入该数组中
	   }
   }
   sort(pixelStore,pixelStore+pixelSum,cmp);//对数据进行降序排序
   T = pixelStore[(int)((clock)/2)];//取出中值

   //再迭代计算最终阈值
    while (abs(threshold - T) > stopcondition)             
    {
        threshold = T;
        for (int i=0; i<src.rows; ++i)
        {
            uchar* pData = src.ptr<uchar>(i);
            for (int j=0; j<nCol; ++j)
            {

                if (pData[j] <= threshold && pData[j]!=0)   //左侧
                {
                    ++nBack;
                    Sumleftpart += pData[j];
                }
                else if (pData[j] > threshold && pData[j]!=0)   //左侧                       //右侧
                {
                    ++nObject;
                    Sumrightpart += pData[j];
                }
            }
        }
        if (nBack == 0 || nObject == 0)//防止除0
            continue;
        T = (Sumleftpart/nBack + Sumrightpart/nObject) / 2;
    }

    //cv::threshold(src, src, T, 255, 0); //根据得出的阈值T进行分割
	src = Binarization(src,T);
	return src;
}

void CMFCpictryDlg::OnDiedaithresh_Cut()//迭代阈值分割
{
	can_back = false;
	Temp_pic = Current_pic.clone();

	Mat roop_imgcut = Current_pic.clone();
    roop_imgcut = IsodataSeg(roop_imgcut);
	cvtColor(roop_imgcut, roop_imgcut, COLOR_GRAY2RGB);
	Current_pic = roop_imgcut;
	imshow("view",Current_pic);
}

void CMFCpictryDlg::OnGetGrayObj()
{
	can_back = false;
	Temp_pic = Current_pic.clone();
	Mat getObj = Current_pic.clone();
	Mat src = 	imagedst.clone();

	for(int i = 0;i< getObj.rows;i++)
	{
		for(int j = 0 ;j<getObj.cols;j++)
		{
			if(getObj.at<Vec3b>(i,j)[0] == 150 && getObj.at<Vec3b>(i,j)[1] == 150 && getObj.at<Vec3b>(i,j)[2] == 150)//要扣的物体为指定颜色
			{
				for(int m =0;m<3;m++)
				{
					getObj.at<Vec3b>(i,j)[m] = src.at<Vec3b>(i,j)[m];
				}
            }
            else
			{
				for(int m =0;m<3;m++)
				{
					getObj.at<Vec3b>(i,j)[m] = 0;
				}
		    }
	  }
  }
	Current_pic = getObj;
	imshow("view",Current_pic);
}

void CMFCpictryDlg::OnSavepic()
{
	imwrite("savefile/horse.bmp",Current_pic);
	AfxMessageBox("保存成功");
}

//均值滤波
Mat averagefiter(Mat src,Mat dst,int model_length,int model_width)
{
	int  sum =0;
	for(long i=(model_width+1)/2-1; i<src.rows-(model_width+1)/2+1; i++) //行
	{
		for(long j=(model_length+1)/2-1; j<src.cols-(model_length+1)/2+1; j++) //列
		{	
			for(int m =0;m<3;m++){
				sum = 0;
				for(int num = -( (model_width+1)/2-1) ;num< (model_width+1)/2;num++)
				{
					for(int num2 = -( (model_length+1)/2-1);num2< (model_length+1)/2;num2++)
					{
						sum +=  src.at<Vec3b>(i+num, j+num2)[m];
					}					
				}
				dst.at<Vec3b>(i, j)[m] = sum/(model_width*model_length);
			}
		}
	}
	return dst;
}

//中值滤波
Mat  centerfiter(Mat src,Mat dst,int model_length,int model_width)
{
	int clock = 0;
	for(long i=(model_width+1)/2-1; i<src.rows-(model_width+1)/2+1; i++) //行
	{
		for(long j=(model_length+1)/2-1; j<src.cols-(model_length+1)/2+1; j++) //列
		{	
			for(int m =0;m<3;m++){
				int a[10000] ={0} ;
				clock = 0;
				for(int num = -( (model_width+1)/2-1) ;num< (model_width+1)/2;num++)
				{
					for(int num2 = -( (model_length+1)/2-1);num2< (model_length+1)/2;num2++)
					{
						 a[clock++] =  src.at<Vec3b>(i+num, j+num2)[m];
					}					
				}
				sort(a,a+clock-1);
				dst.at<Vec3b>(i, j)[m] =  a[(model_width*model_length+1)/2-1]; 
			}
		}
	}
	return dst;
}

void CMFCpictryDlg::OnAverageFilter()
{
	can_back = false;
	Temp_pic = Current_pic.clone();

	Mat src = 	Current_pic.clone();
	Mat dst  = src.clone();
	dst = averagefiter(src,dst,3,3);
	Current_pic = dst ;
    imshow("view",Current_pic);
}

void CMFCpictryDlg::OnMiddleFilter()
{
	can_back = false;
	Temp_pic = Current_pic.clone();

	Mat src = 	Current_pic.clone();
	Mat dst  = src.clone();
	dst = centerfiter(src,dst,3,3);
	Current_pic = dst ;
    imshow("view",Current_pic);
}

Mat  Changeszie(Mat pho_fir, float k)
{
    long height = pho_fir.rows;
	long width = pho_fir.cols;
	long con_height = k*height;
	long con_width = k*width;
	Mat Con(con_height,con_width,CV_8UC3);

	//初始化矩阵
	for(long i=0; i<con_height; i++) //按行进行
	{
		for(long j=0; j<con_width; j++)
		{	
		    Con.at<Vec3b>(i,j) = 0;
		}
	}

	//最近邻插值
	for(long i=0; i<con_height; i++) //按行进行
	{
		for(long j=0; j<con_width; j++)
		{	
			int srcx = i*1/k;
			int srcy = j*1/k;
		    Con.at<Vec3b>(i,j) = pho_fir.at<Vec3b>(srcx,srcy);
		}
	}
	   return Con;
		imshow("缩放图像后",Con);
}

Mat angelRotate(Mat& src, int angle)
{
    // 角度转换
    float alpha = angle * CV_PI / 180;
    // 构造旋转矩阵
    float rotateMat[3][3] = { 
        {cos(alpha), -sin(alpha), 0},
        {sin(alpha), cos(alpha), 0}, 
        {0, 0, 1} };
    int nSrcRows = src.rows;
    int nSrcCols = src.cols;
    // 计算旋转后图像矩阵各个顶点位置
    float a1 = nSrcCols * rotateMat[0][0] ;
    float b1 = nSrcCols * rotateMat[1][0] ;
    float a2 = nSrcCols * rotateMat[0][0] + 
        nSrcRows * rotateMat[0][1];
    float b2 = nSrcCols * rotateMat[1][0] +
        nSrcRows * rotateMat[1][1];
    float a3 = nSrcRows * rotateMat[0][1];
    float b3 = nSrcRows * rotateMat[1][1];
    // 计算出极值点
    float kxMin =  min( min( min(0.0f,a1), a2 ), a3);
    float kxMax =  max( max( max(0.0f,a1), a2 ), a3);
    float kyMin =  min( min( min(0.0f,b1), b2 ), b3);
    float kyMax =  max( max( max(0.0f,b1), b2 ), b3);

    Mat dst(src.rows, src.cols, src.type(),cv::Scalar::all(0)); 
    for( int i = 0; i < src.rows; ++i)
    {       
        for (int  j = 0; j < src.cols; ++j)
        {
            // 旋转坐标转换
            int x =  (j + kxMin) * rotateMat[0][0] -
                (i + kyMin) * rotateMat[0][1] ;
            int y = -(j + kxMin) * rotateMat[1][0] + 
                (i + kyMin) * rotateMat[1][1] ;
            if( (x >= 0) && (x < nSrcCols) && 
                (y >= 0) && (y < nSrcRows) ) 
            {  
                dst.at<cv::Vec3b>(i,j) = 
                    src.at<cv::Vec3b>(y,x);
            }
        }
    }
    return dst;
}

void PicCover(Mat Current_pic,int x_pos,int y_pos, int radio,float size,int angle)
{
	Mat background_pic = Current_pic.clone();
	 //resize(background_pic,background_pic,Size(background_pic.cols/2, background_pic.rows/2));    //调整尺寸

     int be_cover = true;//等待加入物体覆盖背景或被覆盖
	 int x_move = x_pos;//等待加入物体向x轴移动的距离
	 int y_move = y_pos;//等待加入物体向y轴移动的距离

	Mat big_model(background_pic.rows*3,background_pic.cols*3,CV_8UC3) ;//创建一个原来图像9倍大小的模板
	//背景模板初始化
     for(int  i = 0; i < big_model.rows; i++)
	 {
		 for(int j = 0 ; j < big_model.cols;j++)
		 {
			for(int m = 0;m<3;m++)
			{
				big_model.at<Vec3b>(i,j)[m]  = 0;
			}
		 }
	 }

	 //最后截取的叠加的位置
	 int py_start = (int)(background_pic.rows);
	 int px_start = (int)(background_pic.cols);
	 int py_end = (int)(background_pic.rows*2);
	 int px_end = (int)(background_pic.cols*2);
	 //先将背景图片加入模板
	 for(int i = py_start; i < py_end;i++)
	 {
		 for(int j  = px_start; j < px_end;j++ )
		 {
			 for(int m =0;m<3;m++)
			 {
				 big_model.at<Vec3b>(i,j)[m]  = background_pic.at<Vec3b>(i-py_start,j-px_start)[m];
			 }
		 }
	 }

	 //待加入物体,即预处理框中的图像
	 Mat cut_horse= samll_pic.clone();
	 cut_horse = Changeszie(cut_horse, size);
	 cut_horse = angelRotate(cut_horse,angle);
	 //resize(cut_horse,cut_horse,Size(cut_horse.cols*size, cut_horse.rows*size)); //调整尺寸

	 for(int i = 0; i < cut_horse.rows;i++)//指定图中的行
	 {
		 for(int j  = 0; j < cut_horse.cols;j++ )//指定途中的列
		 {
			 if(!radio) //获取radio button控件的值，如果选中被覆盖，则新加入的物体被背景遮挡
			 {
				  if( big_model.at<Vec3b>(i+py_start+y_move,j+px_start+x_move)[0] == 0) //此时物体被背景遮挡  
			      {
				          for(int m =0;m<3;m++)
			              {
				              big_model.at<Vec3b>(i+py_start+y_move,j+px_start+x_move)[m]  = cut_horse.at<Vec3b>(i,j)[m];
			               }
			      }
			 }
			 else
			 {
				  if( cut_horse.at<Vec3b>(i,j)[0]!= 0||cut_horse.at<Vec3b>(i,j)[1]!= 0||cut_horse.at<Vec3b>(i,j)[2]!= 0) //此时背景被新加入的物体遮挡 
			      {
				          for(int m =0;m<3;m++)
			              {
				              big_model.at<Vec3b>(i+py_start+y_move,j+px_start+x_move)[m]  = cut_horse.at<Vec3b>(i,j)[m];
			               }
			      }
			 }
		 }
	 } 
	 //只将中心处图像导入最后图像矩阵中
	 result = background_pic.clone();
	 for(int i = 0; i < result.rows;i++)
	 {
		 for(int j  = 0; j < result.cols;j++ )
		 {
			 for(int m =0;m<3;m++)
			 {
				 result.at<Vec3b>(i,j)[m]  = big_model.at<Vec3b>(i+py_start,j+px_start)[m];
			 }
		 }
	 }

}

void CMFCpictryDlg::OnCoverPic()//叠加功能的实现
{
	 can_back = false;
	 Temp_pic = Current_pic.clone();
	
	 int x_move = x_control.GetPos();//等待加入物体向x轴移动的距离
	 int y_move = y_control.GetPos();//等待加入物体向y轴移动的距离
	 int radio = ((CButton*)GetDlgItem(IDC_RADIO1))->GetCheck();//radio button
	 CString str;//获取物体缩放尺寸
     GetDlgItemText(IDC_EDIT1, str);
	 float size = atof(str);
	 int angle = angle_control.GetPos();//获取物体的旋转角度
	  //resize(cut_horse,cut_horse,Size(cut_horse.cols/2, cut_horse.rows/2));    //调整尺寸

	 PicCover(Current_pic,x_move,y_move,radio,size,angle);//进行叠加
     Current_pic = result;
	 imshow("view",Current_pic);
}

void CMFCpictryDlg::OnWaitTosolve()//小框图像的加载
{
	CString picPath;   //定义图片路径变量
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT|OFN_ALLOWMULTISELECT, 
		NULL, this);   //选择文件对话框
	if(dlg.DoModal() == IDOK)
	{
		picPath= dlg.GetPathName();  //获取图片路径
	}
	//CString to string  使用这个方法记得字符集选用“使用多字节字符”，不然会报错
	string picpath=picPath.GetBuffer(0);  

    samll_pic =  imread(picpath);   //小框原图像
	Mat image = samll_pic.clone();
    CRect rect_src;
	Mat image_smallFrame;//框中的尺寸

    GetDlgItem(IDC_PICTURE_src)->GetClientRect(&rect_src);
    Rect dst_src(rect_src.left,rect_src.top,rect_src.right,rect_src.bottom);
    resize(image,image_smallFrame,cv::Size(rect_src.Width(),rect_src.Height())); 
    imshow("view_src",image_smallFrame);
    can_back  = false;
}

void CMFCpictryDlg::OnCamSetup()
{
VideoCapture mv(0);
Mat frem;
can_setup = true;
while (1)
{
     CRect rect;
	 GetDlgItem(IDC_PICTURE)->GetClientRect(&rect);
	 Rect dst(rect.left,rect.top,rect.right,rect.bottom);
     mv >> frem;
	 resize(frem,frem,cv::Size(rect.Width(),rect.Height())); 
     imshow("view", frem);
     char c = (char)waitKey(10);//？？？？？什么神秘力量少了就不行
     if(!can_setup){
          imshow("view", frem);//显示要保存的一帧图像
          imwrite("截图.bmp",frem);//保存一帧图像
          waitKey();
          break;
      }
    }
}

void CMFCpictryDlg::OnCamCrenncut()
{
	can_setup = false;
}

void CMFCpictryDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
	if(can_show)
	{
	 int x_move = x_control.GetPos();//等待加入物体向x轴移动的距离
	 int y_move = y_control.GetPos();//等待加入物体向y轴移动的距离
	 int radio = ((CButton*)GetDlgItem(IDC_RADIO1))->GetCheck();//radio button
	 CString str;
     GetDlgItemText(IDC_EDIT1, str);
	 float size = atof(str);
	 int angle = 30;
	 PicCover(Current_pic,x_move,y_move,radio,size,angle);//进行叠加
	 imshow("view",result);
	}  
}

void CMFCpictryDlg::OnBnClickedCheck1()//开启同步显示
{
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK1);
    can_show = pBtn->GetCheck();
}

void CMFCpictryDlg::OnDetectPixel()
{
	can_back = false;
	can_detect = true;
	Temp_pic = Current_pic.clone();
    srcImage =  Current_pic.clone();
    // 回调事件响应
    setMouseCallback("view", MouseEvent, 0);
    imshow("view", srcImage);
	//重置开启画笔属性框为未选中，否则显示与效果开启上不对应。
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK2);
    pBtn->SetCheck(0);
}

void CMFCpictryDlg::OnCloseDetect()//关闭像素点检测
{
		can_detect = false;
		imshow("view",Current_pic);
}

void CMFCpictryDlg::OnBnClickedCheck2()//开启画笔
{
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK2);
    can_draw = pBtn->GetCheck();
	Temp_pic = Current_pic.clone();
	org = Current_pic.clone();
	setMouseCallback("view", MouseDraw, 0);
	Current_pic = org;
    imshow("view", org);
}
