#pragma once
//////////////////////////////////////////////////////////////////////////////////////
const int MTIME = 52;            //控制期长
const int MRESVR = 30,			    //最多水库数
MTRIB = 5,				//河流最多支流数
MPLNT = 4,				//最多电厂数
MSUBPLNT = 2,			//一个水库最多下属电厂数
MUTYPE = 13,			//最多机组型号数
MUNIT = 50,				//最多机组个数
MSUBUNIT = 10,			//一个电厂最多下属机组数
MUZONE = 2,				//机组最大运行区间数
MPZONE = 12,			//电厂最大运行区间数
MV =80,				    //库容离散数
MRELEASE =80,			//尾水位/下泄流量离散数
MH = 30,				//水头离散数
HOUR = 24,
N = 2;      //负荷图离散区间数;
float     sparCapBound[MTIME];          //系统备用容量需求
float     maxLoad[MTIME];               //日内最大负荷
const string fileName = "D:\\Fengsz\\PROJECT1\\";
///////////////////////////////////////////////////////////////////////////////////////  
//基本数据定义
typedef struct _TBsc
{
	bool	IsMW;				//是否采用兆瓦单位？1-YES:MW; 0-NO: 万千瓦
	int	  resvrN;				//选中水库个数
	int	  plntN;				//选中电厂个数
	int	  unitN;				//选中机组个数
	int    utypeN;				//机型种类个数
	float  cofP;			    //电力转换系数
	float  Load[MTIME];			//负荷需求

	//--------------------------------------
}TBsc;
///////////////////////////////////////////////////////////////////////////////////////
//水库数据定义
typedef struct _TResvr
{
	int	  iniIndx;				//水库序号
	int	  Linkto;				//下游链接水库序号		
	bool  IsZend;				//是否控制末水位？
	bool  IsZini;				//是否已设定初水位？1-是; 0-否，需要估算出初水位；

	int   Nqz;					//水库尾水位曲线离散数
	int   Nvz;					//水库库容曲线离散数
	float Zini;					//水库初水位
	float Zend;					//水库末水位

	float Z00;					//当日0:00水位						|用于估算
	float t00;					//距离控制期初时刻的时间长(hrs)		|初水位

	float Qdn[MRELEASE];		//水库尾水位曲线流量离散值
	float Zdn[MRELEASE];		//水库尾水位曲线水位离散值
	float Vup[MV];				//水库库容曲线库容离散值
	float Zup[MV];				//水库库容曲线水位离散值

	float Qloc[MTIME];			//水库区间入库流量
	float Qout_min;		//水库出库流量下限
	float Qout_max;		//水库出库流量上限
	float Zmin;			//水库上游水位下限
	float Zmax;			//水库上游水位下限
	//结果：
	float  P[MTIME];			//水库出力         （-显）
	float  Z[MTIME];			//水库水位         （-显）
	float  H[MTIME];			//水库水头         （-显）
	float  Qcome[MTIME];		//水库优化入库流量 （-显）
	float  Qout[MTIME];			//水库出库流量     （-显）
	float  Qgen[MTIME];			//水库发电流量     （-显）
	float  Qspl[MTIME];			//水库弃水流量     （-显）

	//中间变量
	int	   Nlinkfrom;			//上游链接的水库个数
	int    Linkfrom[MTRIB];		//上游链接的水库编号
	float  Vini;				//水库初蓄水
	float  Vend;				//水库末蓄水
	float  V[MTIME];			//水库蓄水
	float  Vmin;			//水库蓄水下限
	float  Vmax;			//水库蓄水上限
	float  GH;              //最大发电流量

	int	   plntN;				//水库下属电厂个数
	int	   plntIndx[MSUBPLNT];	//水库下属电厂编号

}TResvr;
/////////////////////////////////////////////////////////////////////////////////////////
//电厂数据定义
typedef struct _TPlnt
{
	int	  resvrIndx;			//电厂所属水库编号
	float WrateAVG;				//平均耗水率
	float instCap;              //电厂装机容量


	//约束
	float Pmin;			        //电厂出力下限
	float Pmax;			        //电厂出力上限
	//输出结果
	float Pplnt[MTIME];			//电厂出力         （-显）
	float Qplnt[MTIME];			//电厂发电流量     （-显）

	//中间变量----------------------
	int   Nh;					//电厂耗水率曲线离散数
	float Head[MH];				//电厂水头离散值	
	float Level[MH];			//水库蓄水位
	float Wrate[MH];			//电厂耗水率离散值

	int   Nzone;				//电厂运行区间数
	float Lower[MPZONE];		//电厂运行区间下限
	float Upper[MPZONE];		//电厂运行区间上限

	int   unitN;				//电厂下属机组个数
	int	  unitIndx[MSUBUNIT];	//电厂下属机组编号

	float wrateTm[MTIME];		//耗水率过程

}TPlnt;
/////////////////////////////////////////////////////////////////////////////////////////
//机组类型数据定义
typedef struct _TUType
{
	int	  plntIndx;				//该型号所属电厂编号

	int   Nh;					//机组耗水率曲线离散数
	int   MD;                   //每年检修天数
	float Head[MH];				//机组水头离散值	
	float Level[MH];			//水库蓄水位
	float Wrate[MH];			//机组耗水率离散值
	int   Nzone;				//机组运行区间数
	float Lower[MH][MUZONE];	//机组运行区间下限
	float Upper[MH][MUZONE];	//机组运行区间上限
}TUType;
/////////////////////////////////////////////////////////////////////////////////////////
//机组数据定义
typedef struct _TUnit
{
	int   iniIndx;				//机组序号
	int	  plntIndx;				//机组所属电厂编号
	int	  typeIndx;				//机组型号编号
	bool  IsX;					//是否不可用？1-YES; 0-NO
}TUnit;


/////////////////////////////////////////////////////////////////////////////////////////

//装机容量组成
float XM[MRESVR][MTIME];  //检修容量
float XD[MRESVR][MTIME];  //受阻容量
float XR[MRESVR][MTIME];  //备用容量
float XS[MRESVR][MTIME];  //闲置容量
float XW[MRESVR][MTIME];  //工作容量

//日负荷过程（场景）
float D[MTIME];            //负荷待优化电站的负荷需求
float Y[MTIME];            //云南省内的负荷需求
float F[MTIME];            //云南大工厂（硅、铝等耗能大厂）负荷需求
float S[MTIME];            //太阳能发电功率
float W[MTIME];            //风能发电功率
float HS[MTIME];           //小水电发电功率

float Gd[MRESVR][MTIME];   //外送广东电量

bool u[MRESVR][MPLNT][MTIME];     //是否检修
bool s[MRESVR][MPLNT][MTIME];     //是否结束检修
bool v[MRESVR][N][MTIME];         //是否位于d日负荷图的第k个负荷水平
float x[N+1][MTIME];              //d日k位置处的负荷水平



//定义全局变量
TBsc    Bsc;			//基本数据
TResvr  Resvr[MRESVR];  //水库数据
TPlnt   Plnt[MPLNT];    //电厂数据
TUType  UType[MUTYPE];	//机型数据
TUnit	Unit[MUNIT];	//机组数据
//TBus    Bus[MBUS];		//母线数据
/////////////////////////////////////////////////////////////////////////////////////////