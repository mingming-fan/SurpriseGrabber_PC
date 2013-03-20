
#define STRICT
#define DIRECTINPUT_VERSION 0x0800
#include <fstream>
#include <tchar.h>
#include "myd3d.h"
//#include <commctrl.h>
#include <basetsd.h>
#include <dinput.h>
#include <stdio.h>
//////////////////////////////mouse^////////////////////////////////////////////
#include <WinUser.h>
#include <Windows.h>
#include <mmsystem.h>
#include <math.h>
#include <d3dx9.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include "Server.h"

#define UserNumber 5

static char m_serviceNameForServer[5][BT_MAX_SERVICE_NAME_LEN + 1] = {"WIDCOMM DK COM Serial Port","WIDCOMM DK COM Serial Port 2",
"WIDCOMM DK COM Serial Port 3","WIDCOMM DK COM Serial Port 4","WIDCOMM DK COM Serial Port 5"};
static Server **blueServer;


int GrabbingNum[UserNumber][2];//抓取次数，抓取成功次数

int serverCount;

int GrasperNo = 0; 

int ScreenWidth = 800;
int ScreenHeight = 600;

int player1=-1,player2=-1;


int MobileLife[UserNumber];  //记录某个手机是否连接：0为未连接；1为连接

//#include "Server.h"
#pragma warning( disable : 4996 ) // disable deprecated warning 
#include <strsafe.h>
#pragma warning( default : 4996 )
using namespace std; 

#define NumGift 5
#define NumUser 2

#define gametime 300

#define Round0 6
#define Round1 4
#define Round2 2

int round[3]={6,4,2};
d3dXFileData bgbox;
d3dXFileData r02;
d3dXFileData r1;

d3dTexture leftui;
d3dTexture rightui;
d3dTexture empty;
d3dTexture icon[NumGift];

d3dTexture NumPic[10];

//grabber
d3dXFileData HandLeft[UserNumber];
d3dXFileData HandRight[UserNumber];
d3dXFileData HandMid[UserNumber];
d3dXFileData TopHolder[UserNumber];
d3dXFileData Chain[UserNumber];
//d3dXFileData Shadow;

D3DXMATRIXA16 hl[UserNumber];
D3DXMATRIXA16 hr[UserNumber];
D3DXMATRIXA16 hm[UserNumber];
D3DXMATRIXA16 th[UserNumber];
D3DXMATRIXA16 ch[UserNumber];


// 
// D3DXMATRIXA16 gitran1;
// D3DXMATRIXA16 girot1;
// D3DXMATRIXA16 gi1;

D3DXMATRIXA16 hlrot[UserNumber];
D3DXMATRIXA16 hrrot[UserNumber];

D3DXMATRIXA16 GrabbedMat[UserNumber];


int GrabberGift[UserNumber]={-1,-1,-1,-1,-1};


const float Edge=41.0;
const float maxleth=10.0;

FLOAT GrabberLocationX[UserNumber]={0.0f,0.0f,0.0f,0.0f,0.0f},GrabberLocationZ[UserNumber]={0.0f,0.0f,0.0f,0.0f,0.0f};

int state[UserNumber]={1,1,1,1,1};//1移动，2下降，3抓物，4上升，5复位，6松开。

float ChainLenth[UserNumber]={
	1.0,1.0,1.0,1.0,1.0
};
//
d3dXFileData gift[NumGift];//gift

FLOAT RGift[NumGift]={22.0,18.0,20.0,25.0,16.0};


int ValueGift[NumGift]={10,30,20,10,50};//礼物价值

int UserScore[UserNumber];

int StartTime=0;

int GameFlag=0;

D3DXVECTOR3 gLocation[NumGift];//the location of gift
FLOAT gAngle1[Round0]={15.0f,45.0f,75.0f,105.0f,135.0f,165.0f};//the angle of gift head从x轴负半轴，沿着z轴右手方向
FLOAT gAngle2[Round1]={20.0f,65.0f,110.0f,155.0f};
FLOAT gAngle3[Round2]={45.0f,135.0f};
FLOAT gAngle[3][6];
int SnRound[3][Round0]={0,1,2,3,4,0,1,2,3,4,0,1};
int GrabbedGift[UserNumber][2]={-1,-1,-1,-1};// the sn of grabbed gift

int ActiveGift[3][Round0]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};//激活的状态的渲染小车

FLOAT AngleStep=2.0f;
FLOAT GrabberX,GrabberY,DeltaX=0.0,DeltaY=0.0;

int CurrenTime;


char keystate[UserNumber],keyvalue[UserNumber];  //记录手机按键状态及键值

int tt = 0, ttime = 0;

float sx[UserNumber],sy[UserNumber],sz[UserNumber];
//分别记录每个用户按下时的x,y,z信息

VOID ShowTime()
{
	int w=NumPic[0].m_Width,h=NumPic[0].m_Height,t=20,l,sn,CurrenT,test;
	
	l=ScreenWidth/2-w-w/2;

	CurrenT=(int(timeGetTime())-StartTime)/1000;
	CurrenT=gametime-CurrenT;


	NumPic[CurrenT/100].BltFast(l,t,0);

	l=ScreenWidth/2-w/2;

	NumPic[(CurrenT%100)/10].BltFast(l,t,0);

	l=ScreenWidth/2+w/2;

	NumPic[CurrenT%10].BltFast(l,t,0);

	//if (CurrenT>=100)
	//	{
	//		sn=1;
	//	}
	//	else
	//	{	sn=0;}


	//NumPic[sn].BltFast(l,t,0);

	//test=CurrenT-sn*100;
	//sn=test/10;

	//l=ScreenWidth/2-w/2;

	//NumPic[sn].BltFast(l,t,0);
	//
	//l=ScreenWidth/2+w/2;

	//sn=test%10;

	//NumPic[sn].BltFast(l,t,0);

}

//-----------------------------------------------------------------------------
// Name: InitD3D()
// Desc: Initializes Direct3D
//-----------------------------------------------------------------------------
HRESULT InitD3D( HWND hWnd )
{
	// Create the D3D object.
	if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
		return E_FAIL;

	// Set up the structure used to create the D3DDevice. Since we are now
	// using more complex geometry, we will create a device with a zbuffer.
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory( &d3dpp, sizeof( d3dpp ) );
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

	// Create the D3DDevice
	if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp, &g_pd3dDevice ) ) )
	{
		return E_FAIL;
	}
	//alphat
	g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE);
	g_pd3dDevice->SetRenderState( D3DRS_SRCBLEND , D3DBLEND_SRCALPHA);
	g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND , D3DBLEND_INVSRCALPHA);

	// Turn on the zbuffer
	g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );

	// Turn on ambient lighting 
	g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0xffffffff );

	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: InitGeometry()
// Desc: Load the mesh and build the material and texture arrays
//-----------------------------------------------------------------------------
HRESULT InitGeometry()
{

	r02.Create(L"round02.x");
	r1.Create(L"round1.x");


	NumPic[0].Create(L"num0.PNG");
	NumPic[1].Create(L"num1.PNG");
	NumPic[2].Create(L"num2.PNG");
	NumPic[3].Create(L"num3.PNG");
	NumPic[4].Create(L"num4.PNG");
	NumPic[5].Create(L"num5.PNG");
	NumPic[6].Create(L"num6.PNG");
	NumPic[7].Create(L"num7.PNG");
	NumPic[8].Create(L"num8.PNG");
	NumPic[9].Create(L"num9.PNG");


	gift[0].Create(L"elephant.X");
	gift[1].Create(L"shoes.X");
	gift[2].Create(L"basketball.X");

	gift[3].Create(L"bear.X");

	gift[4].Create(L"3car.X");


	leftui.Create(L"leftui.png");
	rightui.Create(L"rightui.png");
	bgbox.Create(L"bgmachine.X");


	empty.Create(L"empty.png");
	icon[0].Create(L"icon0.png");
	icon[1].Create(L"icon1.png");
	icon[2].Create(L"icon2.png");
	icon[3].Create(L"icon3.png");
	icon[4].Create(L"icon4.png");

	HandLeft[0].Create(L"HandLeft.x");
	HandRight[0].Create(L"HandRight.x");
	HandMid[0].Create(L"HandMid.x");
	TopHolder[0].Create(L"TopHolder.x");
	Chain[0].Create(L"Chain2.X");

	for (int i=1;i<UserNumber;i++)
	{

		HandLeft[i].Create(L"HandLeft-1.x");
		HandRight[i].Create(L"HandRight-1.x");
		HandMid[i].Create(L"HandMid-1.x");
		TopHolder[i].Create(L"TopHolder.x");
		Chain[i].Create(L"Chain2.X");
	}
	//	Shadow.Create(L"shadow.X");
	return S_OK;
}



//-----------------------------------------------------------------------------
// Name: Cleanup()
// Desc: Releases all previously initialized objects
//-----------------------------------------------------------------------------
VOID Cleanup()
{

	if( g_pd3dDevice != NULL )
		g_pd3dDevice->Release();

	if( g_pD3D != NULL )
		g_pD3D->Release();
}

VOID Setlight(int i, int lx,int lz)
{
	g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_XRGB( 255 , 255 , 255));

	D3DLIGHT9 light ;
	memset( &light , 0 , sizeof( light ));
	light.Type = D3DLIGHT_POINT ;
	light.Diffuse.r = 1 ; 
	light.Diffuse.g = 1 ;
	light.Diffuse.b = 1 ; 
	light.Position.x =lx ; 
	light.Position.y =4; 
	light.Position.z =lz ; 
	D3DXVec3Normalize( (D3DXVECTOR3*)&light.Direction, (D3DXVECTOR3*)&light.Position  );
	light.Range = 5 ;
	light.Attenuation1 = 0.01f ;
	g_pd3dDevice->SetLight( i , &light );
	g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
	g_pd3dDevice->LightEnable( i , true );
}


VOID environlight()
{
	g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_XRGB( 155 , 155 , 155 ));

	D3DLIGHT9 light ;
	memset( &light , 0 , sizeof( light ));
	light.Type = D3DLIGHT_POINT ;
	light.Diffuse.r = 1 ; 
	light.Diffuse.g = 1 ;
	light.Diffuse.b = 1 ; 
	light.Position.x =0 ; 
	light.Position.y =60; 
	light.Position.z =-110 ; 
	D3DXVec3Normalize( (D3DXVECTOR3*)&light.Direction, (D3DXVECTOR3*)&light.Position  );
	light.Range =200 ;
	light.Attenuation1 = 0.5f ;
	g_pd3dDevice->SetLight( 5 , &light );
	g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
	g_pd3dDevice->LightEnable( 5 , true );
}

//-----------------------------------------------------------------------------
// Name: SetupMatrices()
// Desc: Sets up the world, view, and projection transform matrices.
//-----------------------------------------------------------------------------
VOID SetupMatrices()
{
	// Set up world matrix
	D3DXMATRIXA16 matWorld;
	//     D3DXMatrixRotationY( &matWorld, timeGetTime() / 1000.0f );
	D3DXMatrixIdentity(&matWorld);
	g_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

	// Set up our view matrix. A view matrix can be defined given an eye point,
	// a point to lookat, and a direction for which way is up. Here, we set the
	// eye five units back along the z-axis and up three units, look at the 
	// origin, and define "up" to be in the y-direction.

	D3DXVECTOR3 vEyePt( 0.0f, 60.0f, -110.0f );
	D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
	//D3DXVECTOR3 vEyePt( 8.0f, 15.0f, -25.0f );
	//D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
	//D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f ); 
	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
	g_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

	// For the projection matrix, we set up a perspective transform (which
	// transforms geometry from 3D view space to 2D viewport space, with
	// a perspective divide making objects smaller in the distance). To build
	// a perpsective transform, we need the field of view (1/4 pi is common),
	// the aspect ratio, and the near and far clipping planes (which define at
	// what distances geometry should be no longer be rendered).
	D3DXMATRIXA16 matProj;
	D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI / 4, 1.333f, 1.0f, 1000.0f );
	g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
	environlight();
}

VOID GiftMove()
{
	for (int i=0;i<Round0;i++)
	{
		//if ((1!=GrabbedGift[0][0])&&(i!=GrabbedGift[0][1])&&(1!=GrabbedGift[1][0])&&(i!=GrabbedGift[1][1]))//激活状态，且未被抓住
		//{
		gAngle1[i]+=AngleStep;
		if (gAngle1[i]>180)
		{
			gAngle1[i]-=180;
			SnRound[0][i]=rand()%(NumGift+1);/**12*/;
			//SnRound[0][i]--;
			if (SnRound[0][i]>NumGift-1)
			{
				SnRound[0][i]=NumGift-1;
			}
			if (SnRound[0][i]<0)
			{
				SnRound[0][i]=0;
			}
			ActiveGift[0][i]=1;
			//	}
		}
		gAngle[0][i]=gAngle1[i];
	}

	for (int i=0;i<Round1;i++)
	{
		//		if ((ActiveGift[i]!=0)&&(GrabbedGift[0]!=i)&&(GrabbedGift[1]!=i))//激活状态，且未被抓住
		//		{
		gAngle2[i]-=AngleStep;
		if (gAngle2[i]<0)
		{
			gAngle2[i]+=180;
			SnRound[1][i]=rand()%(NumGift+1);/**12*/;
			//SnRound[1][i]--;
			if (SnRound[1][i]>NumGift-1)
			{
				SnRound[1][i]=NumGift-1;
			}
			if (SnRound[1][i]<0)
			{
				SnRound[1][i]=0;
			}
			ActiveGift[1][i]=1;
		}
		gAngle[1][i]=gAngle2[i];

		//		}
	}

	for (int i=0;i<Round2;i++)
	{
		//		if ((ActiveGift[i]!=0)&&(GrabbedGift[0]!=i)&&(GrabbedGift[1]!=i))//激活状态，且未被抓住
		//		{
		gAngle3[i]+=AngleStep;
		if (gAngle3[i]>180)
		{
			gAngle3[i]-=180;

			SnRound[2][i]=rand()%(NumGift+1);/**12*/;
			//SnRound[2][i]--;
			if (SnRound[2][i]>NumGift-1)
			{
				SnRound[2][i]=NumGift-1;
			}
			if (SnRound[2][i]<0)
			{
				SnRound[2][i]=0;
			}
			ActiveGift[2][i]=1;

		}
		gAngle[2][i]=gAngle3[i];

		//		}
	}
}


VOID Rendergift()
{

	D3DXMATRIXA16 trans,roty,matword,scale,smallscale,carsmall,bgscale;
	//	D3DXMatrixTranslation(&trans,0.0f,0.0f,1.0f);
	D3DXMatrixScaling(&scale,12.0f,12.0f,12.0f);
	D3DXMatrixScaling(&smallscale,0.20f,0.20f,0.20f);
	D3DXMatrixScaling(&bgscale,0.5,0.5,0.5);

	D3DXMatrixScaling(&carsmall,1.5f,1.5f,1.5f);

	D3DXMatrixRotationY(&roty,-(gAngle1[0]/FLOAT(180))*D3DX_PI);
	roty=bgscale*roty;
	g_pd3dDevice->SetTransform( D3DTS_WORLD, &roty );
	r02.Draw();

	D3DXMatrixRotationY(&roty,-(gAngle2[0]/FLOAT(180))*D3DX_PI);
	roty=bgscale*roty;
	g_pd3dDevice->SetTransform( D3DTS_WORLD, &roty );
	r1.Draw();

	for (int i=0;i<Round0;i++)//半径30
	{
		if (ActiveGift[0][i]==1)//((0!=GrabbedGift[0][0])||(i!=GrabbedGift[0][1]))&&((0!=GrabbedGift[1][0])||(i!=GrabbedGift[1][1])))//激活的物品
		{
			//D3DXMatrixTranslation(&trans,gLocation->x,gLocation->y,gLocation->z);
			D3DXMatrixTranslation(&trans,-30.0f,0.0f,0.0f);
			D3DXMatrixRotationY(&roty,-(gAngle1[i]/FLOAT(180))*D3DX_PI);
			matword=scale*trans*roty;

			if ((SnRound[0][i])==1)
			{
				matword=smallscale*trans*roty;
			}
			if ((SnRound[0][i])==4)
			{
				matword=carsmall*trans*roty;
			}
			g_pd3dDevice->SetTransform( D3DTS_WORLD, &matword );
			gift[SnRound[0][i]].Draw();
		}
	}

	for (int i=0;i<Round1;i++)//半径20
	{
		if (ActiveGift[1][i]==1)//((1!=GrabbedGift[0][0])||(i!=GrabbedGift[0][1]))&&((1!=GrabbedGift[1][0])||(i!=GrabbedGift[1][1])))//激活的物品
		{
			//D3DXMatrixTranslation(&trans,gLocation->x,gLocation->y,gLocation->z);
			D3DXMatrixTranslation(&trans,-20.0f,0.0f,0.0f);
			D3DXMatrixRotationY(&roty,-(gAngle2[i]/FLOAT(180))*D3DX_PI);
			matword=scale*trans*roty;

			if ((SnRound[1][i])==1)
			{
				matword=smallscale*trans*roty;
			}
			if ((SnRound[1][i])==4)
			{
				matword=carsmall*trans*roty;
			}
			g_pd3dDevice->SetTransform( D3DTS_WORLD, &matword );
			gift[SnRound[1][i]].Draw();
		}
	}

	for (int i=0;i<Round2;i++)//半径10
	{
		if (ActiveGift[2][i]==1)//((2!=GrabbedGift[0][0])||(i!=GrabbedGift[0][1]))&&((2!=GrabbedGift[1][0])||(i!=GrabbedGift[1][1])))//激活的物品
		{
			//D3DXMatrixTranslation(&trans,gLocation->x,gLocation->y,gLocation->z);
			D3DXMatrixTranslation(&trans,-10.0f,0.0f,0.0f);
			D3DXMatrixRotationY(&roty,-(gAngle3[i]/FLOAT(180))*D3DX_PI);
			matword=scale*trans*roty;

			if ((SnRound[2][i])==1)
			{
				matword=smallscale*trans*roty;
			}
			if ((SnRound[2][i])==4)
			{
				matword=carsmall*trans*roty;
			}
			g_pd3dDevice->SetTransform( D3DTS_WORLD, &matword );
			gift[SnRound[2][i]].Draw();
		}
	}
}

VOID SendScore(bool catched,int score,int GrabberNum){
	//将分数发给第GrabberNum个手机
	//char index[]="0123456789";
	char index[] = {'0','1','2','3','4','5','6','7','8','9'};
	char *buffer = new char[4];
	*buffer = index[(score/100)%10];
	buffer[1] = index[(score/10)%10];
	buffer[2] = index[score%10];
	buffer[3] = '0';

	if(catched){
		buffer[3] = '2';
	}
	else{
		buffer[3] = '1';
	}
	
	bool succ = blueServer[GrabberNum]->serialComm.WriteData(buffer,4);
	
}

VOID State1Move(int i)
{
		//ifstream caminfo("D:\\NaviScene\\Release\\out.txt");
		//char tag,key;
		//float movex,movez,test;
		//caminfo>>tag;
		//caminfo>>key;
		//caminfo>>movex;
		//caminfo>>movez;
		//caminfo>>test;
		//caminfo>>test;
  //      caminfo.close();

		if (keystate[i] == 'D'&& keyvalue[i] == 'O'&& state[i] == 1)
		{
			state[i] = 2;

		}
		float movex,movez,accX,accY;
		movex = sx[i];
		movez = sy[i];
		//accX = ix_raw;
		//accY = iy_raw;

		//movex/=10;
		//movez/=10;

		GrabbedGift[i][0]=-1;
		GrabbedGift[i][1]=-1;

		D3DXMATRIXA16 MoveHand,TopTest;

// 		if (i==4)
// 		{
// 			if (		GrabberX != sx[i]||	GrabberY != sy[i])
// 			{
// 
// 
// 				GrabberX = sx[i];
// 				GrabberY = sy[i];
// 
// 			}
// 			else
// 			{
// 
// 				GrabberX = 0;
// 				GrabberY = 0;
// 			}
// 
// 		}
// 		else
// 		{

		
		GrabberX = sx[i];
		GrabberY = sy[i];
		//}
		// 		if (GrabberLocationX!=FLOAT(GrabberX)+DeltaX||GrabberLocationZ!=FLOAT(GrabberY)+DeltaY)
		// 		{
		GrabberLocationX[i]=FLOAT(GrabberX)+DeltaX;
		GrabberLocationZ[i]=FLOAT(GrabberY)+DeltaY;

		D3DXMatrixTranslation(&MoveHand,GrabberLocationX[i],0.0f,GrabberLocationZ[i]);
		// 		}
		// 		else
		// 		{
		// 			D3DXMatrixIdentity(&MoveHand);
		// 		}




		//if (GrabberLocationX<0.001&&GrabberLocationZ<0.001)
		//{
		//	D3DXMatrixIdentity(&MoveHand);
		//}

		D3DXMatrixRotationZ(&hlrot[i],-(90.0/180.0)*D3DX_PI);
		D3DXMatrixRotationZ(&hrrot[i],(90.0/180.0)*D3DX_PI);

		TopTest=th[i]*MoveHand;

		if( (TopTest._41> -Edge)&&(TopTest._41< Edge)&&(TopTest._43> -Edge)&&(TopTest._43< 0))
		{
			hl[i]=hl[i]*MoveHand;
			hr[i]=hr[i]*MoveHand;
			hm[i]=hm[i]*MoveHand;
			th[i]=th[i]*MoveHand;
			ch[i]=ch[i]*MoveHand;
		}


		sx[i]=0;
		sy[i]=0;
}


FLOAT WhetherCatch(int GrabberNum)
{

	GrabbingNum[GrabberNum][0]++;

	bool catched = false;
	D3DXMATRIXA16 trans,roty,matword,scale,smallscale,carsmall;

	D3DXMatrixScaling(&scale,12.0f,12.0f,12.0f);
	D3DXMatrixScaling(&smallscale,0.20f,0.20f,0.20f);
	D3DXMatrixScaling(&carsmall,1.5f,1.5f,1.5f);


	FLOAT MinDis=200.0f,test=0.0f;

	int sn;

	int score = 0;// = UserScore[GrabberNum];

	for (int i=0;i<Round0;i++)//半径30
	{
		//if((0!=GrabbedGift[0][0])&&(i!=GrabbedGift[0][1])&&(0!=GrabbedGift[1][0])&&(i!=GrabbedGift[1][1]))//激活的物品
		{

			D3DXMatrixTranslation(&trans,-30.0f,0.0f,0.0f);
			D3DXMatrixRotationY(&roty,-(gAngle1[i]/FLOAT(180))*D3DX_PI);
			matword=scale*trans*roty;
			//D3DXMatrixTranslation(&trans,gLocation->x,gLocation->y,gLocation->z);

			test=(matword._41-th[GrabberNum]._41)*(matword._41-th[GrabberNum]._41)+(matword._43-th[GrabberNum]._43)*(matword._43-th[GrabberNum]._43);
		//	if (test<MinDis)
			//{
				MinDis=test;
				sn=SnRound[0][i];
				if (MinDis<RGift[SnRound[0][i]])
				{
					GrabbedGift[GrabberNum][0]=0;
					GrabbedGift[GrabberNum][1]=i;
					GrabbedMat[GrabberNum]=scale*trans*roty;


					if ((sn)==1)
					{
						GrabbedMat[GrabberNum]=smallscale*trans*roty;
					}
					if ((sn)==4)
					{
						GrabbedMat[GrabberNum]=carsmall*trans*roty;
					}

					ActiveGift[0][i]=0;
					catched = true;
					GrabberGift[GrabberNum]=sn;

					if (GameFlag==1)
					{
						UserScore[GrabberNum]+=ValueGift[SnRound[0][i]];
						GrabbingNum[GrabberNum][1]++;
					}	
					score = UserScore[GrabberNum];
                    
					for(int i=0;i<10;i++){SendScore(catched,score,GrabberNum);}
					return 1;
				}
		//	}
		}
	}

	for (int i=0;i<Round1;i++)//半径20
	{
		//if ((1!=GrabbedGift[0][0])&&(i!=GrabbedGift[0][1])&&(1!=GrabbedGift[1][0])&&(i!=GrabbedGift[1][1]))//激活的物品
		{
			//D3DXMatrixTranslation(&trans,gLocation->x,gLocation->y,gLocation->z);
			D3DXMatrixTranslation(&trans,-20.0f,0.0f,0.0f);
			D3DXMatrixRotationY(&roty,-(gAngle2[i]/FLOAT(180))*D3DX_PI);

			matword=scale*trans*roty;

			test=(matword._41-th[GrabberNum]._41)*(matword._41-th[GrabberNum]._41)+(matword._43-th[GrabberNum]._43)*(matword._43-th[GrabberNum]._43);
		//	if (test<MinDis)
			//{
				MinDis=test;
				sn=SnRound[1][i];
				if (MinDis<RGift[SnRound[1][i]])
				{
					GrabbedGift[GrabberNum][0]=1;
					GrabbedGift[GrabberNum][1]=i;
					GrabbedMat[GrabberNum]=scale*trans*roty;


					if ((sn)==1)
					{
						GrabbedMat[GrabberNum]=smallscale*trans*roty;
					}
					if ((sn)==4)
					{
						GrabbedMat[GrabberNum]=carsmall*trans*roty;
					}
					ActiveGift[1][i]=0;
					catched = true;

					GrabberGift[GrabberNum]=sn;

					if (GameFlag==1)
					{UserScore[GrabberNum]+=ValueGift[SnRound[1][i]];
					GrabbingNum[GrabberNum][1]++;}

					score = UserScore[GrabberNum];
					for(int i=0;i<10;i++){SendScore(catched,score,GrabberNum);}
					return 1;
				}
			//}

		}
	}

	for (int i=0;i<Round2;i++)//半径20
	{
		//if ((2!=GrabbedGift[0][0])&&(i!=GrabbedGift[0][1])&&(2!=GrabbedGift[1][0])&&(i!=GrabbedGift[1][1]))//激活的物品
		{
			//D3DXMatrixTranslation(&trans,gLocation->x,gLocation->y,gLocation->z);
			D3DXMatrixTranslation(&trans,-10.0f,0.0f,0.0f);
			D3DXMatrixRotationY(&roty,-(gAngle3[i]/FLOAT(180))*D3DX_PI);
			matword=scale*trans*roty;


			test=(matword._41-th[GrabberNum]._41)*(matword._41-th[GrabberNum]._41)+(matword._43-th[GrabberNum]._43)*(matword._43-th[GrabberNum]._43);
		//	if (test<MinDis)
			//{
				MinDis=test;
				sn=SnRound[2][i];
				if (MinDis<RGift[SnRound[2][i]])
				{
					GrabbedGift[GrabberNum][0]=2;
					GrabbedGift[GrabberNum][1]=i;
					GrabbedMat[GrabberNum]=scale*trans*roty;


					if ((sn)==1)
					{
						GrabbedMat[GrabberNum]=smallscale*trans*roty;
					}
					if ((sn)==4)
					{
						GrabbedMat[GrabberNum]=carsmall*trans*roty;
					}
					ActiveGift[2][i]=0;
					catched = true;

					GrabberGift[GrabberNum]=sn;

					if (GameFlag==1)
					{
						UserScore[GrabberNum]+=ValueGift[SnRound[2][i]];
						GrabbingNum[GrabberNum][1]++;
					}

					score = UserScore[GrabberNum];
					for(int i=0;i<10;i++){SendScore(catched,score,GrabberNum);}

					return 1;
				}
			//}

		}
	}


	//未抓起时就将上一轮的分数发给手机
	score = UserScore[GrabberNum];
	for(int i=0;i<10;i++){SendScore(catched,score,GrabberNum);}
	

	return 0;

}



VOID State2Down(int i)
{

	FLOAT dis;

	if (ChainLenth[i]<26)
	{
		D3DXMATRIXA16 ChainScale,HolderMove,chrot;
		ChainLenth[i]+=2;
		D3DXMatrixTranslation(&HolderMove,0,-2,0);
		D3DXMatrixScaling(&ChainScale,1.0,ChainLenth[i],1.0);//缩放

		D3DXMatrixRotationYawPitchRoll(&chrot,0.0,90,0.0);

		hl[i]=hl[i]*HolderMove;
		hr[i]=hr[i]*HolderMove;
		hm[i]=hm[i]*HolderMove;
		ch[i]=ChainScale*th[i];

	}
	else
	{
		state[i]=3;
		dis=WhetherCatch(i);//grab
	}
}

float CatchRot=1.0;

VOID State3Catch(int i)
{
	D3DXMATRIXA16 rot;


	if (CatchRot<90)
	{
		D3DXMatrixRotationZ(&hlrot[i],-(((90.0-CatchRot)/180.0)*D3DX_PI));
		D3DXMatrixRotationZ(&hrrot[i],(((90.0-CatchRot)/180.0)*D3DX_PI));
		CatchRot+=10.0;
		if (GrabbedGift[i][0]!=-1)
		{
			GrabbedMat[i]._41=(th[i]._41-GrabbedMat[i]._41)*10/(90-CatchRot)+GrabbedMat[i]._41;
			GrabbedMat[i]._43=(th[i]._43-GrabbedMat[i]._43)*10/(90-CatchRot)+GrabbedMat[i]._43;
		}
	}
	else
	{
		if (GrabbedGift[i][0]!=-1)
		{
			GrabbedMat[i]._41=th[i]._41;
			GrabbedMat[i]._43=th[i]._43;
		}
		state[i]=4;
	}

	if (GrabbedGift[i][0]!=-1)
	{
		g_pd3dDevice->SetTransform( D3DTS_WORLD,&GrabbedMat[i]);
		//gift[SnRound[GrabbedGift[i][0]][GrabbedGift[i][1]]].Draw();
		gift[GrabberGift[i]].Draw();
	}

}

VOID State4Back(int i)
{
	if (ChainLenth[i]>1)
	{
		D3DXMATRIXA16 ChainScale,HolderMove,chrot;
		ChainLenth[i]-=2;
		D3DXMatrixTranslation(&HolderMove,0,2,0);
		D3DXMatrixScaling(&ChainScale,1.0,ChainLenth[i],1.0);//缩放

		D3DXMatrixRotationYawPitchRoll(&chrot,0.0,90,0.0);

		hl[i]=hl[i]*HolderMove;
		hr[i]=hr[i]*HolderMove;
		hm[i]=hm[i]*HolderMove;
		ch[i]=ChainScale*th[i];
		//if (GiftFlag1==2)
		//{
		//	gitran1=gitran1*HolderMove;
		//}
		if (GrabbedGift[i][0]!=-1)
		{
			GrabbedMat[i]=GrabbedMat[i]*HolderMove;
			g_pd3dDevice->SetTransform( D3DTS_WORLD,&GrabbedMat[i]);
			//gift[SnRound[GrabbedGift[i][0]][GrabbedGift[i][1]]].Draw();
			gift[GrabberGift[i]].Draw();
		}

	}
	else
	{
		if (GrabbedGift[i][0]!=-1)
		{
			state[i]=5;
		}
		else
		{
			state[i]=6;
		}

		ttime=int(timeGetTime());
	}
}

INT State5BacktoTarget(int i)
{
	if (int(timeGetTime())>(ttime+10))
	{
		ttime=int(timeGetTime());
		float hx,hz;
		hx=th[i]._41;
		hz=th[i]._43;
		D3DXMATRIXA16 HolderMove;
		D3DXMatrixIdentity(&HolderMove);
		if (hx>-30)
		{
			//hx+=0.01;
			hx=-5;
			hz=0;

		}
		else if (hz>(-40))
		{
			//hz-=0.01;
			hz=-5;
			hx=0;

		}
		else
		{
			state[i]=6;
			return 0;
		}

		HolderMove._41=hx;
		HolderMove._43=hz;

		hl[i]=hl[i]*HolderMove;
		hr[i]=hr[i]*HolderMove;
		hm[i]=hm[i]*HolderMove;
		th[i]=th[i]*HolderMove;
		ch[i]=ch[i]*HolderMove;


		if (GrabbedGift[i][0]!=-1)
		{
			GrabbedMat[i]=GrabbedMat[i]*HolderMove;
			g_pd3dDevice->SetTransform( D3DTS_WORLD,&GrabbedMat[i]);
			//gift[SnRound[GrabbedGift[i][0]][GrabbedGift[i][1]]].Draw();
			gift[GrabberGift[i]].Draw();
		}
		//if (GiftFlag1==2)
		//{
		//	gitran1=gitran1*HolderMove;
		//}


	}
	return 1;

}


float fallspeed=0;

VOID GiftFallDown(int i)
{
	D3DXMATRIXA16 GiftFall;

	fallspeed+=1.0f;

	D3DXMatrixTranslation(&GiftFall,0,-fallspeed,0);

	GrabbedMat[i]=GrabbedMat[i]*GiftFall;
	g_pd3dDevice->SetTransform( D3DTS_WORLD,&GrabbedMat[i]);
	//gift[SnRound[GrabbedGift[i][0]][GrabbedGift[i][1]]].Draw();
	gift[GrabberGift[i]].Draw();
}

VOID State6Release(int i)
{
	D3DXMATRIXA16 rot;

	if (CatchRot>0)
	{
		D3DXMatrixRotationZ(&hlrot[i],-(((90.0-CatchRot)/180.0)*D3DX_PI));
		D3DXMatrixRotationZ(&hrrot[i],(((90.0-CatchRot)/180.0)*D3DX_PI));
		CatchRot-=10.0;

		if (GrabbedGift[i][0]!=-1)
		{
			GiftFallDown(i);
		}
	}
	else
	{
		state[i]=1;
	}	

}



VOID RenderGrabber()
{

	for (int i = 0; i < UserNumber; i++)
	{

		if (MobileLife[i]==1)
		{
		

	switch (state[i])//state1移动，2下降，3抓物，4上升，5复位，6松开。

	{
	case 1:State1Move(i);break;
	case 2:State2Down(i);break;
	case 3:State3Catch(i);break;
	case 4:State4Back(i);break;
	case 5:State5BacktoTarget(i);break;
	case 6:State6Release(i);break;
	}

	//		BackgroudPic.BltFast(0,0,700,700,0);

	D3DXMATRIXA16 RenderSense;
	D3DXMATRIXA16 HandRotate;

	D3DXMATRIXA16 GiftMoveXy;

	D3DXMATRIXA16 MoveUp,matWorld;

	D3DXMatrixTranslation(&MoveUp,0,30.0,0);

	D3DXMatrixIdentity(&HandRotate);

	//D3DXMatrixRotationYawPitchRoll(&HandRotate,0,0.01,0);

	hl[i]=HandRotate*hl[i];

	RenderSense=hlrot[i]*hl[i];

	matWorld=RenderSense*MoveUp;

	g_pd3dDevice->SetTransform( D3DTS_WORLD,&matWorld);

	HandLeft[i].Draw();

	hr[i]=HandRotate*hr[i];

	RenderSense=hrrot[i]*hr[i];

	matWorld=RenderSense*MoveUp;

	g_pd3dDevice->SetTransform( D3DTS_WORLD,&matWorld);
	HandRight[i].Draw();

	matWorld=hm[i]*MoveUp;
	g_pd3dDevice->SetTransform( D3DTS_WORLD,&matWorld);
	HandMid[i].Draw();

	matWorld=th[i]*MoveUp;
	g_pd3dDevice->SetTransform( D3DTS_WORLD,&matWorld);
	TopHolder[i].Draw();

	matWorld=ch[i]*MoveUp;
	g_pd3dDevice->SetTransform( D3DTS_WORLD,&matWorld);
	Chain[i].Draw();

	Setlight(i,th[i]._41,th[i]._43);

	//gi1=girot1*gitran1;

	//g_pd3dDevice->SetTransform( D3DTS_WORLD,&gi1);
	//Gift1.Draw();

	//D3DXMATRIXA16 testz;

	//D3DXMatrixTranslation(&testz,0,-3,0);

	//testz=testz*th[i];

	//g_pd3dDevice->SetTransform( D3DTS_WORLD,&testz);
	//Shadow.Draw();
	}

	}

}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Draws the scene
//------------------------------------------------------------------------bi-----

VOID GameOver()
{
	GameFlag=0;

	ofstream res("result.txt",ios::app);
	for (int i=0;i<UserNumber;i++)
	{
		res<<i<<" "<<UserScore[i]<<" Grab Num:"<<GrabbingNum[i][0]<<"  sucessfull Num:"<<GrabbingNum[i][1]<<endl;
		
	}
	res<<endl;
	res.close();
}


int LeftUiPosition=0;

int lastgift[UserNumber]={-1,-1,-1,-1,-1};


VOID RenderUI()
{
	int x,y;

	leftui.BltFast(0,LeftUiPosition,leftui.m_Width*ScreenHeight/ScreenWidth,LeftUiPosition+ScreenHeight,0 );
	leftui.BltFast(0,LeftUiPosition+ScreenHeight,leftui.m_Width*ScreenHeight/ScreenWidth,LeftUiPosition+ScreenHeight*2,0 );
	LeftUiPosition-=3;
	if (LeftUiPosition<=-ScreenHeight)
	{
		LeftUiPosition=0;
	}

	rightui.BltFast(ScreenWidth-rightui.m_Width,0,ScreenWidth,ScreenHeight,0);

	x=ScreenWidth-rightui.m_Width;
	y=ScreenHeight/5-empty.m_Height;

	for (int i=0;i<UserNumber;i++)
	{
		if (MobileLife[i]==1)
		{
			if (GrabberGift[i]==-1)
			{
				empty.BltFast(x,y,0);
			}
			else
			{
				icon[GrabberGift[i]].BltFast(x,y,0);
			}
		}
		y=y+ScreenHeight/5;
	}

}


VOID Render()
{

	//,30,50,0);


	D3DXMATRIXA16 bgscale;

	D3DXMatrixScaling(&bgscale,0.5,0.5,0.5);

	int test;
	if ((CurrenTime+50)<int(timeGetTime()))
	//if(1)
	{
		// Clear the backbuffer and the zbuffer
		g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
			D3DCOLOR_XRGB( 0, 0, 0 ), 1.0f, 0 );


		// Begin the scene
		if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
		{
			// Setup the world, view, and projection matrices
			SetupMatrices();

			RenderUI();


			g_pd3dDevice->SetTransform( D3DTS_WORLD,&bgscale);

			bgbox.Draw();//背景


			test=int(timeGetTime())-CurrenTime;
			ofstream testtime("time.txt");
			testtime<<test;
			testtime.close();

			CurrenTime=int(timeGetTime());
			GiftMove();

			test=int(timeGetTime())-CurrenTime;
			ofstream testtime1("time1.txt");
			testtime1<<test;
			testtime1.close();

			CurrenTime=int(timeGetTime());
			Rendergift();

			test=int(timeGetTime())-CurrenTime;
			ofstream testtime2("time2.txt");
			testtime2<<test;
			testtime2.close();

			CurrenTime=int(timeGetTime());

			RenderGrabber();

			if (GameFlag==1)//在游戏进行中
			{

				ShowTime();
				if ((StartTime+gametime*1000)<int(timeGetTime()))//120 second for game
				{
					GameOver();
				}
			}

			// End the scene
			g_pd3dDevice->EndScene();
		}

		// Present the backbuffer contents to the display
		g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
	}
		

}

//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: The window's message handler
//-----------------------------------------------------------------------------

VOID GameStart()
{
	StartTime=int(timeGetTime());
	for (int j=0;j++;j<UserNumber)
	{
		UserScore[j]=0;
		//lastgift[j]=-1;
		GrabberGift[j]=-1;
		GrabbingNum[j][0]=0; //total times
		GrabbingNum[j][1]=0; // sucessful times
	}
	GameFlag=1;
}

int lastx,lasty;
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	int newx,newy;
	switch( msg )
	{
	case WM_DESTROY:
		Cleanup();
		PostQuitMessage( 0 );
		return 0;
 	case WM_MOUSEMOVE:
 
		if (MobileLife[4]==1)
		{
			newx=LOWORD(lParam);
			newy=-HIWORD(lParam);

			if ((lastx!=0)||(lasty!=0))
			{
				sx[4]=FLOAT(newx-lastx);
				sy[4]=FLOAT(newy-lasty);
			}

			lastx=newx;
			lasty=newy;

			DeltaX=0.0f;
			DeltaY=0.0f;
		}
 
 		break;
 	case WM_KEYDOWN:
 		switch (wParam)
 		{
 			case VK_SPACE:
			if (state[4])
 			{
				state[4]=2;
 			}
			break;
			case VK_F3:
				GameStart();
				break;
			case VK_F1:
				MobileLife[4]=1;
				break;
			case VK_F2:
				MobileLife[4]=0;
				break;
 		}
 		break;
	}


	return DefWindowProc( hWnd, msg, wParam, lParam );
}



//蓝牙信息接收部分
VOID BlueMessage()
{

	int MobileKey[UserNumber]; //手机按键的信息：包括键值和是否按下	
	//没有按键信息返回值：-1；
	//按键按下：按下数字，和左右的返回值——方向左键对应值20，方向右键对应值21
	//按下：0-0，1-1，2-2，3-3，4-4，5-5，6-6，7-7，8-8，9-9，20-左,21-右，22-上，23-下，24-中；
	//按键抬起：
	//抬起：10-0，11-1，12-2，13-3，14-4，15-5，16-6，17-7，18-8，19-9，30-左,31-右，32-上，33-下，34-中


	for (int i=0;i<(UserNumber-1);i++)
	{MobileLife[i]=0;}

    float tx=0, ty=0, sc, rot,z=0;

	bool needMorePort = true;
	for(int i=0;i<5;i++)
	{

			if(blueServer[i]->m_isStarted && !blueServer[i]->m_isConnected)
			{
				needMorePort = false;
			}
			else if(!blueServer[i]->m_isStarted && needMorePort)
			{
				if(i<4){
						blueServer[i+1]->StartServer();
				}
				serverCount++;
				break;
			}
			else
			{

			}

			if(blueServer[i]->m_isConnected)
			{
				MobileLife[i]=1;
				

				int bytes_in;
				char msg_block[1000];

				bytes_in = blueServer[i]->serialComm.ReadData(msg_block, sizeof msg_block );

				if (bytes_in)
				{ 
					if(*msg_block == 'U'){

						if(msg_block[1] == '1')
						{
							MobileKey[i]=11;
						}
						if(msg_block[1] == '3')
						{
							MobileKey[i]=13;
						}
						if(msg_block[1] == 'R')
						{
							MobileKey[i]=31;
						}
						if(msg_block[1] == 'L')
						{
							MobileKey[i]=30;
						}
						if(msg_block[1] == '5')
						{
							MobileKey[i]=15;
						}
						if(msg_block[1] == '4')
						{
							MobileKey[i]=14;
						}
						if(msg_block[1] == '6')
						{
							MobileKey[i]=16;
						}
						if(msg_block[1] == '7')
						{
							MobileKey[i]=17;
						}

						if(msg_block[1] == '9')
						{
							MobileKey[i]=19;
						}

						if(msg_block[1] == 'U')
						{
							MobileKey[i]=32;
						}

						if(msg_block[1] == 'D')
						{
							MobileKey[i]=33;
						}

						if(msg_block[1] == 'O')
						{
							MobileKey[i]=34;
						}
					}
					else if(*msg_block == 'D'){
						if(msg_block[1] == '1')
						{
							MobileKey[i]=1;
						}
						if(msg_block[1] == '3')
						{
							MobileKey[i]=3;
						}
						if(msg_block[1] == 'R')
						{
							MobileKey[i]=21;
						}
						if(msg_block[1] == 'L')
						{
							MobileKey[i]=20;
						}
						if(msg_block[1] == '5')
						{
							MobileKey[i]=5;
						}
						if(msg_block[1] == '4')
						{
							MobileKey[i]=4;
						}
						if(msg_block[1] == '6')
						{
							MobileKey[i]=6;
						}
						if(msg_block[1] == '7')
						{
							MobileKey[i]=7;
						}

						if(msg_block[1] == '8')
						{
							//Cleanup();
							//PostQuitMessage( 0 );
						}

						if(msg_block[1] == '9')
						{
							MobileKey[i]=9;
						}

						if(msg_block[1] == 'U')
						{
							MobileKey[i]=22;
						}

						if(msg_block[1] == 'D')
						{
							MobileKey[i]=23;
						}

						if(msg_block[1] == 'O')
						{
							MobileKey[i]=24;
						}
					}
					sscanf(msg_block, "%c%c%f%f%f%f", &keystate[i],&keyvalue[i],&tx, &ty, &sc, &rot);

					sx[i] = -tx;
					sy[i] = ty;
			}
			else
			{
				MobileLife[i]=0;
			}
	}

   //
	for (int i=0;i<UserNumber;i++)
	{
		if (MobileLife[i]==1)//第i个手机链接
		{
			GrasperNo = i;
		}
	}
 }
}


//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: The application's entry point
//-----------------------------------------------------------------------------
INT WINAPI wWinMain( HINSTANCE hInst, HINSTANCE, LPWSTR, INT )
{

	StartTime=int(timeGetTime());

	CurrenTime=int(timeGetTime());//计时

	for (int i=0;i<NumGift;i++)
	{
	//	RGift[i]=20.0f;

	}//半径


    //初始化蓝牙服务




	serverCount = 0;
	blueServer = new Server*[5];
	for(int i=0;i<5;i++){
		blueServer[i] = new Server;
		memcpy(blueServer[i]->m_serviceName, m_serviceNameForServer[i], BT_MAX_SERVICE_NAME_LEN + 1);
	}


	blueServer[serverCount]->StartServer();
	blueServer[serverCount+1]->StartServer();
	blueServer[serverCount+2]->StartServer();
	blueServer[serverCount+3]->StartServer();
	blueServer[serverCount+4]->StartServer();
	serverCount++;

	//sx = sy = ix_raw = iy_raw = 0;
	for (int i=0;i<UserNumber;i++)
	{
		keystate[i] = 'U';
		keyvalue[i] = '.';
 	}
	// Register the window class
	WNDCLASSEX wc =
	{
		sizeof( WNDCLASSEX ), CS_CLASSDC, MsgProc, 0L, 0L,
		GetModuleHandle( NULL ), NULL, NULL, NULL, NULL,
		L"UCam Application", NULL
	};
	RegisterClassEx( &wc );

	// Create the application's window
	HWND hWnd = CreateWindow( L"UCam Application", L"Surprise Grabber",
		WS_OVERLAPPEDWINDOW, 100, 100, ScreenWidth, ScreenHeight,
		NULL, NULL, wc.hInstance, NULL );

	// Initialize Direct3D
	if( SUCCEEDED( InitD3D( hWnd ) ) )
	{
		// Create the scene geometry
		if( SUCCEEDED( InitGeometry() ) )
		{
			// Show the window
			ShowWindow( hWnd, SW_SHOWDEFAULT );
			UpdateWindow( hWnd );


			D3DXMATRIXA16 test;

			for (int i=0;i<UserNumber;i++)
			{

			D3DXMatrixIdentity(&hl[i]);
			D3DXMatrixIdentity(&hr[i]);
			D3DXMatrixIdentity(&hm[i]);
			D3DXMatrixIdentity(&th[i]);
			D3DXMatrixIdentity(&ch[i]);
// 			D3DXMatrixIdentity(&girot1);
// 			D3DXMatrixIdentity(&gitran1);
// 
// 			D3DXMatrixTranslation(&gitran1,0,-22,0);

			D3DXMatrixTranslation(&hl[i],-1.5,0,0);
			D3DXMatrixTranslation(&hr[i],1.5,0,0);
			//D3DXMatrixTranslation(&hm[i],0,2.8,0);
			D3DXMatrixScaling(&test,2,2,2);
			hm[i]=test*hm[i];
			D3DXMatrixTranslation(&th[i],0,2.8,0);
			D3DXMatrixTranslation(&ch[i],0,2.3,0);


			//D3DXMatrixRotationYawPitchRoll(&test,0,90,0);
			//D3DXMatrixScaling(&ch[i],10,10,480);
			//ch[i]=test*ch[i];
			}
			// Enter the message loop
			MSG msg;
			ZeroMemory( &msg, sizeof( msg ) );
			while( msg.message != WM_QUIT )
			{
				if (tt<int(timeGetTime()/100))
				{
					tt=int(timeGetTime()/100);//每100毫秒采点一次
					BlueMessage();
					Render();
				}

				if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
				{
					TranslateMessage( &msg );
					DispatchMessage( &msg );
				}
				else
					Render();
			}
		}
	}

	UnregisterClass( L"UCam Application", wc.hInstance );
	return 0;
}




