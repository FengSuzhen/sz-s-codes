#pragma once
//////////////////////////////////////////////////////////////////////////////////////
const int MTIME = 52;            //�����ڳ�
const int MRESVR = 30,			    //���ˮ����
MTRIB = 5,				//�������֧����
MPLNT = 4,				//���糧��
MSUBPLNT = 2,			//һ��ˮ����������糧��
MUTYPE = 13,			//�������ͺ���
MUNIT = 50,				//���������
MSUBUNIT = 10,			//һ���糧�������������
MUZONE = 2,				//�����������������
MPZONE = 12,			//�糧�������������
MV =80,				    //������ɢ��
MRELEASE =80,			//βˮλ/��й������ɢ��
MH = 30,				//ˮͷ��ɢ��
HOUR = 24,
N = 2;      //����ͼ��ɢ������;
float     sparCapBound[MTIME];          //ϵͳ������������
float     maxLoad[MTIME];               //������󸺺�
const string fileName = "D:\\Fengsz\\PROJECT1\\";
///////////////////////////////////////////////////////////////////////////////////////  
//�������ݶ���
typedef struct _TBsc
{
	bool	IsMW;				//�Ƿ�������ߵ�λ��1-YES:MW; 0-NO: ��ǧ��
	int	  resvrN;				//ѡ��ˮ�����
	int	  plntN;				//ѡ�е糧����
	int	  unitN;				//ѡ�л������
	int    utypeN;				//�����������
	float  cofP;			    //����ת��ϵ��
	float  Load[MTIME];			//��������

	//--------------------------------------
}TBsc;
///////////////////////////////////////////////////////////////////////////////////////
//ˮ�����ݶ���
typedef struct _TResvr
{
	int	  iniIndx;				//ˮ�����
	int	  Linkto;				//��������ˮ�����		
	bool  IsZend;				//�Ƿ����ĩˮλ��
	bool  IsZini;				//�Ƿ����趨��ˮλ��1-��; 0-����Ҫ�������ˮλ��

	int   Nqz;					//ˮ��βˮλ������ɢ��
	int   Nvz;					//ˮ�����������ɢ��
	float Zini;					//ˮ���ˮλ
	float Zend;					//ˮ��ĩˮλ

	float Z00;					//����0:00ˮλ						|���ڹ���
	float t00;					//��������ڳ�ʱ�̵�ʱ�䳤(hrs)		|��ˮλ

	float Qdn[MRELEASE];		//ˮ��βˮλ����������ɢֵ
	float Zdn[MRELEASE];		//ˮ��βˮλ����ˮλ��ɢֵ
	float Vup[MV];				//ˮ��������߿�����ɢֵ
	float Zup[MV];				//ˮ���������ˮλ��ɢֵ

	float Qloc[MTIME];			//ˮ�������������
	float Qout_min;		//ˮ�������������
	float Qout_max;		//ˮ�������������
	float Zmin;			//ˮ������ˮλ����
	float Zmax;			//ˮ������ˮλ����
	//�����
	float  P[MTIME];			//ˮ�����         ��-�ԣ�
	float  Z[MTIME];			//ˮ��ˮλ         ��-�ԣ�
	float  H[MTIME];			//ˮ��ˮͷ         ��-�ԣ�
	float  Qcome[MTIME];		//ˮ���Ż�������� ��-�ԣ�
	float  Qout[MTIME];			//ˮ���������     ��-�ԣ�
	float  Qgen[MTIME];			//ˮ�ⷢ������     ��-�ԣ�
	float  Qspl[MTIME];			//ˮ����ˮ����     ��-�ԣ�

	//�м����
	int	   Nlinkfrom;			//�������ӵ�ˮ�����
	int    Linkfrom[MTRIB];		//�������ӵ�ˮ����
	float  Vini;				//ˮ�����ˮ
	float  Vend;				//ˮ��ĩ��ˮ
	float  V[MTIME];			//ˮ����ˮ
	float  Vmin;			//ˮ����ˮ����
	float  Vmax;			//ˮ����ˮ����
	float  GH;              //��󷢵�����

	int	   plntN;				//ˮ�������糧����
	int	   plntIndx[MSUBPLNT];	//ˮ�������糧���

}TResvr;
/////////////////////////////////////////////////////////////////////////////////////////
//�糧���ݶ���
typedef struct _TPlnt
{
	int	  resvrIndx;			//�糧����ˮ����
	float WrateAVG;				//ƽ����ˮ��
	float instCap;              //�糧װ������


	//Լ��
	float Pmin;			        //�糧��������
	float Pmax;			        //�糧��������
	//������
	float Pplnt[MTIME];			//�糧����         ��-�ԣ�
	float Qplnt[MTIME];			//�糧��������     ��-�ԣ�

	//�м����----------------------
	int   Nh;					//�糧��ˮ��������ɢ��
	float Head[MH];				//�糧ˮͷ��ɢֵ	
	float Level[MH];			//ˮ����ˮλ
	float Wrate[MH];			//�糧��ˮ����ɢֵ

	int   Nzone;				//�糧����������
	float Lower[MPZONE];		//�糧������������
	float Upper[MPZONE];		//�糧������������

	int   unitN;				//�糧�����������
	int	  unitIndx[MSUBUNIT];	//�糧����������

	float wrateTm[MTIME];		//��ˮ�ʹ���

}TPlnt;
/////////////////////////////////////////////////////////////////////////////////////////
//�����������ݶ���
typedef struct _TUType
{
	int	  plntIndx;				//���ͺ������糧���

	int   Nh;					//�����ˮ��������ɢ��
	int   MD;                   //ÿ���������
	float Head[MH];				//����ˮͷ��ɢֵ	
	float Level[MH];			//ˮ����ˮλ
	float Wrate[MH];			//�����ˮ����ɢֵ
	int   Nzone;				//��������������
	float Lower[MH][MUZONE];	//����������������
	float Upper[MH][MUZONE];	//����������������
}TUType;
/////////////////////////////////////////////////////////////////////////////////////////
//�������ݶ���
typedef struct _TUnit
{
	int   iniIndx;				//�������
	int	  plntIndx;				//���������糧���
	int	  typeIndx;				//�����ͺű��
	bool  IsX;					//�Ƿ񲻿��ã�1-YES; 0-NO
}TUnit;


/////////////////////////////////////////////////////////////////////////////////////////

//װ���������
float XM[MRESVR][MTIME];  //��������
float XD[MRESVR][MTIME];  //��������
float XR[MRESVR][MTIME];  //��������
float XS[MRESVR][MTIME];  //��������
float XW[MRESVR][MTIME];  //��������

//�ո��ɹ��̣�������
float D[MTIME];            //���ɴ��Ż���վ�ĸ�������
float Y[MTIME];            //����ʡ�ڵĸ�������
float F[MTIME];            //���ϴ󹤳����衢���Ⱥ��ܴ󳧣���������
float S[MTIME];            //̫���ܷ��繦��
float W[MTIME];            //���ܷ��繦��
float HS[MTIME];           //Сˮ�緢�繦��

float Gd[MRESVR][MTIME];   //���͹㶫����

bool u[MRESVR][MPLNT][MTIME];     //�Ƿ����
bool s[MRESVR][MPLNT][MTIME];     //�Ƿ��������
bool v[MRESVR][N][MTIME];         //�Ƿ�λ��d�ո���ͼ�ĵ�k������ˮƽ
float x[N+1][MTIME];              //d��kλ�ô��ĸ���ˮƽ



//����ȫ�ֱ���
TBsc    Bsc;			//��������
TResvr  Resvr[MRESVR];  //ˮ������
TPlnt   Plnt[MPLNT];    //�糧����
TUType  UType[MUTYPE];	//��������
TUnit	Unit[MUNIT];	//��������
//TBus    Bus[MBUS];		//ĸ������
/////////////////////////////////////////////////////////////////////////////////////////