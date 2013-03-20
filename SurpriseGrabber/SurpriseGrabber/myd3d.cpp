/*////////////////////////////////////////////////






  Direct Graphice





/*////////////////////////////////////////////////
#include "windows.h"

#include "myd3d.h"

/*//////////////////////////////////////////////

  �t��

/*//////////////////////////////////////////////
//
//LPDIRECT3D9			g_pD3D ;
//LPDIRECT3DDEVICE9	g_pd3dDevice ;
LPDIRECT3D9         g_pD3D ;//= NULL; // Used to create the D3DDevice
LPDIRECT3DDEVICE9   g_pd3dDevice;// = NULL; // Our rendering device
/*//////////////////////////////////////////////

  ��l��

/*//////////////////////////////////////////////
// void d3dInit()
// {
// 	g_pD3D = NULL ;
// 	g_pd3dDevice = NULL ;
// }

/*//////////////////////////////////////////////

  ����

/*//////////////////////////////////////////////
void d3dRelease()
{
	if( g_pD3D ) g_pD3D->Release();
	if( g_pd3dDevice ) g_pd3dDevice->Release();
}

/*//////////////////////////////////////////////

  �إ�
  1.�վ�ù��l�ΰϤj�p
  2.�إ�D3d �� Device 
  3.ø�ϭ��M��

/*//////////////////////////////////////////////
BOOL d3dCreate( HWND hWnd , int width , int height , BOOL IsWindow )
{
	//�վ�ù��l�ΰϤj�p
	if( !d3dSetDisplaySize( hWnd , width , height ))
		return false ;
	//�إ�D3D �˸m
	if( !d3dDeviceCreate( hWnd , IsWindow ))
		return false ;
	//ø�ϭ��M��
	d3dClear();
	//����
	return true ;
}


/*//////////////////////////////////////////////

  �վ�ù��j�p

/*//////////////////////////////////////////////
BOOL d3dSetDisplaySize( HWND hWnd , int width , int height )
{
	//�վ�ù��j�p
	RECT rect ;
	HDC hdc ;
	//���o�l�ΰϦ�m
	hdc = GetDC( hWnd );
	rect.left = (GetDeviceCaps( hdc , HORZRES ) - width )/2;
	rect.top = (GetDeviceCaps( hdc , VERTRES ) - height )/2;
	ReleaseDC( hWnd , hdc );
	rect.right = rect.left + width ;
	rect.bottom = rect.top + height ;
	//�վ�l�ΰ�
	AdjustWindowRectEx( &rect , GetWindowLong( hWnd , GWL_STYLE ) , (GetMenu( hWnd)!=NULL) , 
		GetWindowLong( hWnd , GWL_EXSTYLE) );
	//���ʩl�ΰ�
	MoveWindow( hWnd , rect.left , rect.top , rect.right - rect.left , rect.bottom - rect.top , true );
	return true ;
}
/*//////////////////////////////////////////////

  �إ�D3d �� Device 

/*//////////////////////////////////////////////
BOOL d3dDeviceCreate( HWND hWnd , BOOL IsWindow  )
{
	//�إ�d3d�D�n�˸m
	g_pD3D =  Direct3DCreate9( D3D_SDK_VERSION );
	if( !g_pD3D )
		return false ;
	//���o�ثe�ୱ��ܼҦ�
	D3DDISPLAYMODE d3ddm ;
	if( g_pD3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT , &d3ddm ) != D3D_OK )
		return false ;
	//�إ�ø�ϸ˸m
	D3DPRESENT_PARAMETERS d3dpp ;
	memset( &d3dpp , 0 , sizeof( d3dpp ));
	d3dpp.Windowed = IsWindow ;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD ;
	d3dpp.BackBufferFormat = d3ddm.Format ;
	d3dpp.hDeviceWindow = hWnd ;
	d3dpp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER ;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    d3dpp.EnableAutoDepthStencil = TRUE;

	if( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT , 
		D3DDEVTYPE_HAL , hWnd , D3DCREATE_SOFTWARE_VERTEXPROCESSING , 
		&d3dpp , &g_pd3dDevice ) != D3D_OK )
		return false ;
	return true ;
}
/*//////////////////////////////////////////////

  ø�ϭ��M��

/*//////////////////////////////////////////////
void d3dClear( UINT color )
{
	g_pd3dDevice->Clear( 0 , 0 , D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER , color , 1.0f , 0 );
}
/*//////////////////////////////////////////////





  Direct Graphice Hdc Object
  class d3dHdc 





/*//////////////////////////////////////////////
d3dHdc::d3dHdc()
{
	m_hdc = 0 ;
	m_Surface = 0 ;
	//���o�˸m
	if( !g_pd3dDevice )
		return ;
	if( g_pd3dDevice->GetBackBuffer( 0 , 0 , D3DBACKBUFFER_TYPE_MONO , &m_Surface ) != D3D_OK )
		return ;

	m_Surface->GetDC( &m_hdc );

}

d3dHdc::~d3dHdc()
{
	Release();
}

void d3dHdc::Release()
{
	if( m_Surface )
	{
		if( m_hdc )
			m_Surface->ReleaseDC( m_hdc );
	//	m_Surface->Release();

		m_hdc = NULL ;
		m_Surface = NULL ;
	}

}

/*////////////////////////////////////////////////




  3D ����




/*////////////////////////////////////////////////

d3dTexture::d3dTexture()
{
	m_Texture = NULL ;
}

d3dTexture::~d3dTexture()
{
	Release();
}

void d3dTexture::Release()
{
	if( m_Texture )
		m_Texture->Release();
	m_Texture = NULL ;
}

BOOL d3dTexture::Create(  LPCTSTR file )//LPDIRECT3DDEVICE9 device,
{
	D3DXIMAGE_INFO in ;
	memset( &in , 0 , sizeof( in ));
	//��l��
	Release();
	//���J
	D3DXCreateTextureFromFileEx( g_pd3dDevice ,
		file , D3DX_DEFAULT , D3DX_DEFAULT , 
		0 , 0 , D3DFMT_UNKNOWN , D3DPOOL_MANAGED ,
		D3DX_DEFAULT , 
		D3DX_DEFAULT , 0 , &in , NULL , &m_Texture );
	if( m_Texture == NULL )
		return false ;
	//���o���
	m_Width = in.Width ;
	m_Height = in.Height ;

	return true ;
//	m_Texture->Get
}
/*
    0   1
	3   2
*/
void d3dTexture::BltFast(int x, int y , float a )
{
	BltFast( x , y , x + m_Width , y + m_Height , a );
}

void d3dTexture::BltFast(int l , int t , int r , int b , float a )
{
	D3DTLVERTEX v[4] ;
	//�w�I�����c
	memset( v , 0 , sizeof( v ));
	//�I��m
	//������
	if( a == 0.0f )
	{
		v[0].x = v[3].x = (float)(l) ;
		v[1].x = v[2].x = (float)(r);
		v[0].y = v[1].y = (float)(t);
		v[2].y = v[3].y = (float)(b);
	}else //����
	{
		float ox , oy ;
		float in ;
		float s , c ;
		//������|��
		in = a * 3.1415926f / 180.0f ;
		//���I
		ox = (float)( r + l )/ 2 ;
		oy = (float)( b + t )/ 2 ;
		//�۹��m
		l -= (int)ox ;
		t -= (int)oy ;
		r -= (int)ox ;
		b -= (int)oy ;
		//��s��m
		s = sinf( in );
		c = cosf( in );

		v[0].x =  c * l + s * t + ox ;
		v[0].y = -s * l + c * t + oy ;

		v[1].x =  c * r + s * t + ox ;
		v[1].y = -s * r + c * t + oy ;

		v[2].x =  c * r + s * b + ox ;
		v[2].y = -s * r + c * b + oy ;

		v[3].x =  c * l + s * b + ox ;
		v[3].y = -s * l + c * b + oy ;
	}
	//Z
	v[0].rhw = v[1].rhw = v[2].rhw = v[3].rhw = 
	v[0].z = v[1].z = v[2].z = v[3].z = 0.5f ;
	//�C��
	v[0].diffuse = v[1].diffuse = v[2].diffuse = v[3].diffuse = -1 ;
	//����
	v[1].tu = v[2].tu = 1.0f ;
	v[2].tv = v[3].tv = 1.0f ;
	//�]ø�ϼҦ�

	g_pd3dDevice->SetTexture( 0 , m_Texture );
	g_pd3dDevice->SetFVF( D3DFVF_TLVERTEX  );
	if( g_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN , 2 , (LPVOID)v , sizeof( D3DTLVERTEX )) != D3D_OK )
		int a = 0 ;


}
/*////////////////////////////////////////////////




  X�ҫ�




/*////////////////////////////////////////////////

void d3dXFileData::Init()
{
	m_Num		= 0 ;
	m_Texture	= NULL ;
	m_Material  = NULL ;
	m_Mesh		= NULL ;
}

void d3dXFileData::Release()
{
	if( m_Mesh ) 
		m_Mesh->Release();

	if( m_Texture )
		delete [] m_Texture ;

	if( m_Material )
		delete [] m_Material ;
	Init();
}


BOOL d3dXFileData::Create( LPCTSTR file)//LPDIRECT3DDEVICE9 device,
{
    LPD3DXMESH   pTempMesh;
	DWORD i ;
	LPD3DXBUFFER buffer ;
	D3DXMATERIAL *matl ;

	LPD3DXBUFFER pointmesh;
	ID3DXBuffer *adj;

	//Ū��
	if( D3DXLoadMeshFromX( file , D3DXMESH_SYSTEMMEM , 
		g_pd3dDevice , &pointmesh , 
		&buffer , NULL , &m_Num , &m_Mesh ) != D3D_OK )
		return false ;
	//�t����O����
	m_Texture = new d3dTexture[ m_Num ];
	m_Material = new D3DMATERIAL9[m_Num] ;
	//�����e
	matl = (D3DXMATERIAL*)buffer->GetBufferPointer();

	adj =  (ID3DXBuffer *)pointmesh->GetBufferPointer();
	
	for( i = 0 ; i < m_Num ; i++ )
	{
		m_Material[i] = matl[i].MatD3D ;
		m_Material[i].Ambient = m_Material[i].Diffuse ;

		//m_Material[i].Power = Mpower;//�趨ֵ

		CString cstr(matl[i].pTextureFilename);
		m_Texture[i].Create( (LPCTSTR)cstr);//device,

	}
	//�k�V�q
    if ( !(m_Mesh->GetFVF() & D3DFVF_NORMAL) )
    {
        if( m_Mesh->CloneMeshFVF( (m_Mesh->GetOptions() & D3DXMESH_32BIT )|D3DXMESH_MANAGED, m_Mesh->GetFVF() | D3DFVF_NORMAL, 
                                            g_pd3dDevice, &pTempMesh ) != D3D_OK )
											return 0 ;
        D3DXComputeNormals( pTempMesh, NULL );

        m_Mesh->Release();
        m_Mesh = pTempMesh;
    }
	//����
	buffer->Release();
	return true ; 
}

void d3dXFileData::Draw()//LPDIRECT3DDEVICE9 device
{
	DWORD i ;

	for( i = 0 ; i < m_Num ; i++ )
	{
		g_pd3dDevice->SetMaterial( &m_Material[i] );
		g_pd3dDevice->SetTexture( 0 , m_Texture[i] );

		m_Mesh->DrawSubset( i );

	}
}