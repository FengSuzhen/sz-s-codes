#pragma once
//////////////////////////////////////////////////////////////////////////////////////
const int MTIME = 52;        
const int MRESVR = 30,			 
MTRIB = 5,				
MPLNT = 4,			
MSUBPLNT = 2,			
MUTYPE = 13,			
MUNIT = 50,			
MSUBUNIT = 10,			
MUZONE = 2,				
MPZONE = 12,			
MV =80,				   
MRELEASE =80,		
MH = 30,			
HOUR = 24,
N = 2;    
float     sparCapBound[MTIME];      
float     maxLoad[MTIME];            
const string fileName = "D:\\Fengsz\\PROJECT1\\";
///////////////////////////////////////////////////////////////////////////////////////  

typedef struct _TBsc
{
	bool	IsMW;				
	int	  resvrN;			
	int	  plntN;				
	int	  unitN;			
	int    utypeN;			
	float  cofP;			    
	float  Load[MTIME];		

	//--------------------------------------
}TBsc;
///////////////////////////////////////////////////////////////////////////////////////

typedef struct _TResvr
{
	int	  iniIndx;			
	int	  Linkto;			
	bool  IsZend;		
	bool  IsZini;				

	int   Nqz;				
	int   Nvz;				
	float Zini;					
	float Zend;				

	float Z00;					
	float t00;				

	float Qdn[MRELEASE];		
	float Zdn[MRELEASE];		
	float Vup[MV];				
	float Zup[MV];			

	float Qloc[MTIME];		
	float Qout_min;		
	float Qout_max;		
	float Zmin;			
	float Zmax;			

	float  P[MTIME];			
	float  Z[MTIME];			
	float  H[MTIME];			
	float  Qcome[MTIME];		
	float  Qout[MTIME];			
	float  Qgen[MTIME];		
	float  Qspl[MTIME];		


	int	   Nlinkfrom;			
	int    Linkfrom[MTRIB];	
	float  Vini;			
	float  Vend;			
	float  V[MTIME];		
	float  Vmin;		
	float  Vmax;			
	float  GH;        

	int	   plntN;				
	int	   plntIndx[MSUBPLNT];

}TResvr;
/////////////////////////////////////////////////////////////////////////////////////////

typedef struct _TPlnt
{
	int	  resvrIndx;			
	float WrateAVG;				
	float instCap;             



	float Pmin;			      
	float Pmax;			      

	float Pplnt[MTIME];			
	float Qplnt[MTIME];			


	int   Nh;				
	float Head[MH];				
	float Level[MH];			
	float Wrate[MH];		

	int   Nzone;			
	float Lower[MPZONE];		
	float Upper[MPZONE];	

	int   unitN;				
	int	  unitIndx[MSUBUNIT];	

	float wrateTm[MTIME];		

}TPlnt;
/////////////////////////////////////////////////////////////////////////////////////////

typedef struct _TUType
{
	int	  plntIndx;			

	int   Nh;					
	int   MD;                  
	float Head[MH];				
	float Level[MH];			
	float Wrate[MH];			
	int   Nzone;				
	float Lower[MH][MUZONE];	
	float Upper[MH][MUZONE];	
/////////////////////////////////////////////////////////////////////////////////////////

typedef struct _TUnit
{
	int   iniIndx;			
	int	  plntIndx;			
	int	  typeIndx;				
	bool  IsX;				
}TUnit;


/////////////////////////////////////////////////////////////////////////////////////////

float XM[MRESVR][MTIME];  
float XD[MRESVR][MTIME];  
float XR[MRESVR][MTIME]; 
float XS[MRESVR][MTIME];  
float XW[MRESVR][MTIME];  

float D[MTIME];            
float Y[MTIME];           
float F[MTIME];           
float S[MTIME];            
float W[MTIME];           
float HS[MTIME];       

float Gd[MRESVR][MTIME];  

bool u[MRESVR][MPLNT][MTIME];  
bool s[MRESVR][MPLNT][MTIME];
bool v[MRESVR][N][MTIME];        
float x[N+1][MTIME];        




TBsc    Bsc;		
TResvr  Resvr[MRESVR]; 
TPlnt   Plnt[MPLNT];   
TUType  UType[MUTYPE];	
TUnit	Unit[MUNIT];	
//TBus    Bus[MBUS];	

/////////////////////////////////////////////////////////////////////////////////////////
