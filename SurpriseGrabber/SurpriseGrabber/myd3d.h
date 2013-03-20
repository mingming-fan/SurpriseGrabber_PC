/*////////////////////////////////////////////////






  Direct Graphice





/*////////////////////////////////////////////////

#if !defined(__MYD3D_H___)
#define __MYD3D_H___


#include "d3d9.h"
#include "d3dx9tex.h"
#include <atlstr.h>

#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")

/*//////////////////////////////////////////////




  常用頂點資料




/*//////////////////////////////////////////////

/*//////////////////////////////////////////////

  已上色，已轉換

/*//////////////////////////////////////////////
const DWORD D3DFVF_TLVERTEX = (D3DFVF_XYZRHW | D3DFVF_DIFFUSE |  D3DFVF_SPECULAR | D3DFVF_TEX1 );
typedef struct _D3DTLVERTEX 
{
	float x , y , z , rhw ;
	D3DCOLOR  diffuse , specular;
	float tu, tv;
}D3DTLVERTEX ;
/*//////////////////////////////////////////////

  系統

/*//////////////////////////////////////////////
//extern LPDIRECT3D9			g_pD3D ;
//extern LPDIRECT3DDEVICE9	g_pd3dDevice ;

extern LPDIRECT3D9         g_pD3D ;//= NULL; // Used to create the D3DDevice
extern LPDIRECT3DDEVICE9   g_pd3dDevice;// = NULL; // Our rendering device
//初始化
void d3dInit();
//移除
void d3dRelease();
//建立
BOOL d3dCreate( HWND hWnd , int width , int height , BOOL IsWindow  );
//調整螢幕始用區大小
BOOL d3dSetDisplaySize( HWND hWnd , int width , int height );
//建立D3d 及 Device 
BOOL d3dDeviceCreate( HWND hWnd , BOOL IsWindow );
//繪圖頁清空
void d3dClear( UINT color = 0 );

/*//////////////////////////////////////////////





  Direct Graphice Hdc Object
  class d3dHdc 





/*//////////////////////////////////////////////

class d3dHdc 
{
private :
	HDC m_hdc ;
	LPDIRECT3DSURFACE9 m_Surface ;

public :
	void Release();
	inline operator HDC(){ return m_hdc ;};
public :
	d3dHdc();
	~d3dHdc();
};

/*////////////////////////////////////////////////




  3D 材質




/*////////////////////////////////////////////////
class d3dTexture
{
private :

	LPDIRECT3DTEXTURE9	m_Texture ;
public :
	int					m_Width ;
	int					m_Height ;
	void BltFast( int x , int y , float a = 0.0f );
	void BltFast( int l , int t , int r , int b , float a );
public :
	BOOL Create(  LPCTSTR file );//LPDIRECT3DDEVICE9 device,
	void Release();
	inline operator LPDIRECT3DTEXTURE9(){ return m_Texture ;};

public :
	d3dTexture();
	~d3dTexture();
};

/*////////////////////////////////////////////////




  X模型




/*////////////////////////////////////////////////

class d3dXFileData
{
public :
	DWORD			m_Num ; //模型數
	LPD3DXMESH		m_Mesh ;//模型資料
	d3dTexture		*m_Texture ;//材質圖檔
public :
	D3DMATERIAL9	*m_Material ;//表面材質
	void Init();
	void Release();
	void Draw() ;//LPDIRECT3DDEVICE9 Device
	BOOL Create( LPCTSTR file);//LPDIRECT3DDEVICE9 Device
public :
	d3dXFileData(){Init();};
	~d3dXFileData(){Release();};
};

#endif

