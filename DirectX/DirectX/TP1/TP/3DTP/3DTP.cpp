#include "stdafx.h"
#include "3DTP.h"
#include "cube.h"
#include <d3d9.h>
#include <d3dx9.h>
#include <iostream>
#include <sstream>

// Global Variables:
HINSTANCE	hInst;			// current instance
HWND		hWnd;			// windows handle used in DirectX initialization

/*
@TODO
variables global (interface avec directx et le device en autres
*/

// Forward declarations
bool				CreateWindows(HINSTANCE, int);
bool				CreateDevice();
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LPDIRECT3D9 pD3D = Direct3DCreate9(D3D_SDK_VERSION);
IDirect3DDevice9 *device;

D3DVERTEXELEMENT9 dwDecl3[] = {
	{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},	
	{0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
	D3DDECL_END()
};

struct vertex {
	D3DXVECTOR3 Position;
	D3DCOLOR color;
};

int countParticle = 1;

int sizeSquare = 55; // taille d'un cote, tout est fait en fonction de cette valeur, mettre une valeur impaire
int rayon = floor((float)sizeSquare/2);

float lookatPtX = 0; // la camera regarde toujours vers l'orgine
float lookatPtY = 0;  
float lookatPtZ = 0;
float eyePtX = 0;
float eyePtY = 0;
float eyePtZ = -rayon; // place la camera proche d'un bord


int initEnv (IDirect3DVertexDeclaration9** pDecl,
	IDirect3DVertexBuffer9** bufferBox,
	IDirect3DIndexBuffer9** indexBufferBox,
	IDirect3DVertexBuffer9** bufferParticles,
	IDirect3DIndexBuffer9** indexBufferParticles)
{
	//device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME); // Fait le rendu sans remplir le cube

	device->CreateVertexDeclaration(dwDecl3, &(*pDecl));	

	// Creation particles
	//device->CreateVertexBuffer(countParticle * sizeof(vertex), 0, 0, D3DPOOL_DEFAULT, &bufferParticles, NULL);
	//vertex* pDataParticle;
	//bufferParticles->Lock(0, countParticle * sizeof(vertex), (void**) &pDataParticle, 0);
	//pDataParticle[0].Position = D3DXVECTOR3(sizeSquare / 2, 50, sizeSquare / 2);
	//pDataParticle[0].color = D3DCOLOR_ARGB(255, 255, 255, 255);

	device->CreateVertexBuffer(6 *(sizeSquare * sizeSquare) * sizeof(vertex), 0, 0, D3DPOOL_DEFAULT, &(*bufferBox), NULL);
	vertex* pData;
	(*bufferBox)->Lock(0, 6 * (sizeSquare * sizeSquare) * sizeof(vertex), (void**) &pData, 0); // il y 6 faces
	
	int index = 0;
	// Rempli le vertex buffer de tout les points necessaires pour creer le cube
	for (int i = 1; i <= 6; i++)
	{
		index = fillVertexBuffer (pData, index, i, sizeSquare);
	}
	(*bufferBox)->Unlock();

	device->CreateIndexBuffer((6 * 6 * (sizeSquare - 1) * (sizeSquare - 1) * sizeof(int)), 0, D3DFMT_INDEX32, D3DPOOL_DEFAULT, &(*indexBufferBox), NULL);
	int* pDataIndex;
	(*indexBufferBox)->Lock(0, 6 * 6 * (sizeSquare - 1) * (sizeSquare - 1)* sizeof(int), (void**) &pDataIndex, 0);
	
	int k = 0; 
	int a = 1;
	int b = sizeSquare + 1;
	int c = sizeSquare;

	// Cree l'integralite des 6 faces
	k = fillIndexBuffer(pDataIndex, 0, k, a, c, c, b, sizeSquare);
	k = fillIndexBuffer(pDataIndex, 1, k, a, c, c, b, sizeSquare);
	k = fillIndexBuffer(pDataIndex, 2, k, c, a, b, c, sizeSquare);
	k = fillIndexBuffer(pDataIndex, 3, k, c, a, b, c, sizeSquare);
	k = fillIndexBuffer(pDataIndex, 4, k, c, a, b, c, sizeSquare);
	k = fillIndexBuffer(pDataIndex, 5, k, c, a, b, c, sizeSquare);
	(*indexBufferBox)->Unlock();

	return true;
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	IDirect3DVertexBuffer9* bufferBox;
	IDirect3DIndexBuffer9* indexBufferBox;

	IDirect3DVertexBuffer9* bufferParticles;
	IDirect3DIndexBuffer9* indexBufferParticles;

	MSG oMsg;
	    if (!CreateWindows (hInstance, nCmdShow))
    {
        MessageBox(NULL, "Erreur lors de la création de la fenêtre", "Error", 0);
        return false;
    }
	
	CoInitialize (NULL);

	if (!CreateDevice())
    {
        MessageBox(NULL, "Erreur lors de la création du device DirectX 9", "Error", 0);
        return false;
    }

	IDirect3DVertexDeclaration9 *pDecl; 

	// Cree le cube autour du moteur a particules ainsi que le tableau de particules
	initEnv(&pDecl, &bufferBox, &indexBufferBox, &bufferParticles, &indexBufferParticles);

	LPD3DXEFFECT pEffect;
	LPD3DXBUFFER compilationErrors;
	if (D3DXCreateEffectFromFile(device, "shader.vsh",NULL,NULL, 0, NULL, &pEffect, &compilationErrors) != D3D_OK)
	{
		MessageBoxA (NULL, (char *)compilationErrors->GetBufferPointer(), "Error", 0);
	}

	D3DXHANDLE hWorldViewProj = pEffect->GetParameterByName (NULL, "WorldViewProj");
	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(&proj, 0.78*1.5, (float)1280/(float)800, 0.01, 1000);


    PeekMessage( &oMsg, NULL, 0, 0, PM_NOREMOVE );
    while ( oMsg.message != WM_QUIT )
    {
        if (PeekMessage( &oMsg, NULL, 0, 0, PM_REMOVE )) 
        {
            TranslateMessage( &oMsg );
            DispatchMessage( &oMsg );
        }
        else
        {

			D3DCOLOR color = D3DCOLOR_ARGB (255, 0, 0, 0);
			device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, color, 1.0f, 0);
			device->BeginScene();

			// Initialise our vectors
			D3DXVECTOR3 vEyePt( eyePtX, eyePtY, eyePtZ );
			D3DXVECTOR3 vLookatPt(lookatPtX, lookatPtY, lookatPtZ);
			D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );

			// Get D3DX to fill in the matrix values
			D3DXMATRIX matView;
			D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );

			D3DXMATRIX world;
			D3DXMatrixIdentity(&world);	

			D3DXMATRIXA16 WorldViewProj = world * matView * proj;
			pEffect->SetMatrix(hWorldViewProj , &WorldViewProj);

			unsigned int cPasses, iPass;
			pEffect->Begin(&cPasses, 0);
			for (iPass= 0; iPass< cPasses; ++iPass)
			{
				pEffect->BeginPass(iPass);
				pEffect->CommitChanges(); // que si on a changé des états après le BeginPass
				
				device->SetVertexDeclaration(pDecl);
				device->SetStreamSource(0, bufferBox, 0, sizeof(vertex));

				device->SetIndices(indexBufferBox);
				device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0, 6 * (sizeSquare * sizeSquare),0, (((sizeSquare-1) * (sizeSquare-1)) * 2) * 6);

				pEffect->EndPass();
			}
			pEffect->End();

			device->EndScene();
			device->Present(0, 0, 0, 0);
        }
    }
    CoUninitialize ();
    return (int) oMsg.wParam;
}


bool CreateWindows(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable
    //
    WNDCLASSEX wcex;
    memset(&wcex, 0, sizeof(WNDCLASSEX));
    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style					= CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc		= WndProc;
    wcex.hInstance			= hInstance;
    wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszClassName	= "3DTPClassName";

    if ( RegisterClassEx(&wcex) == 0 )
            return false;

   hWnd = CreateWindow("3DTPClassName", "Moteur a particule", WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
      return false;

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return true;
}

bool CreateDevice()
{
//@TODO Creation de l'interface avec DirectX et du device
//Direct3DCreate9
//D3DPRESENT_PARAMETERS
//CreateDevice

	D3DDISPLAYMODE displayMode;
	D3DPRESENT_PARAMETERS pp;
	pD3D->GetAdapterDisplayMode (D3DADAPTER_DEFAULT, &displayMode);
	pp.Windowed = true; //Mode fenêtré ou pas
	pp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	pp.BackBufferWidth = 1280;// Taille en x du Back Buffer
	pp.BackBufferHeight = 720; // Taille en y du Back Buffer
	pp.BackBufferFormat = displayMode.Format; // Format du Back Buffer
	pp.BackBufferCount = 1; // Nombre de Back Buffer
	pp.MultiSampleType = D3DMULTISAMPLE_NONE ; // Nombre de sample pour l’antialiasing
	pp.MultiSampleQuality = 0; // Qualité pour l’antialiasing
	pp.hDeviceWindow = hWnd; //Handle de la fenêtre
	pp.EnableAutoDepthStencil = true; // True si on veut un depth-stencil buffer
	pp.AutoDepthStencilFormat = D3DFMT_D24S8; // Le format du deth-stencil buffer
	pp.Flags = 0; // Voir le man
	pp. FullScreen_RefreshRateInHz = 0; //Voir le man
	pp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT; // Autrement dit 0, voir le man

	pD3D->CreateDevice(0, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &pp, &device);


    return true;
}

//
//  PURPOSE:  Processes messages for the main window.
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static double angle = -(D3DX_PI / 2);

    switch (message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_KEYDOWN:
		{
			switch(wParam)
			{
				case VK_LEFT :
				{					
					angle -= 0.1;
					eyePtX = rayon * cos(angle);
					eyePtZ = rayon * sin(angle);
					break;
				}
				case VK_RIGHT :
				{
					angle += 0.1;
					eyePtX = rayon * cos(angle);
					eyePtZ = rayon * sin(angle);
					break;
				}
				case VK_UP :
				{
					eyePtY += sizeSquare*0.01;
					break;
				}
				case VK_DOWN :
				{
					eyePtY -= sizeSquare*0.01;
					break;
				}

			}
		}
		break;
    case WM_KEYUP:
        {
			switch(wParam)
			{
				case VK_ESCAPE:
				{
					PostQuitMessage(0);
					break;
				}
			}
			break;
		}
	 case WM_CHAR:
		{
			switch(wParam)
			{
				case 'w' :
				{
					rayon -= 1;
					eyePtX = rayon * cos(angle);
					eyePtZ = rayon * sin(angle);
					break;
				}
				case 's' :
				{
					rayon += 1;
					eyePtX = rayon * cos(angle);
					eyePtZ = rayon * sin(angle);
					break;
				}
			}
			break;
		} 
	 default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

