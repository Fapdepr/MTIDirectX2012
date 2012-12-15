#include "stdafx.h"
#include "cube.h"

struct vertex {
	D3DXVECTOR3 Position;
	D3DCOLOR color;
};

int fillVertexBuffer (vertex* pData, int index, int face, int sizeSquare)
{
	for (int i = -floor((float)sizeSquare/2); i <= floor((float)sizeSquare/2); i++)
	{
		for (int j = -floor((float)sizeSquare/2); j <= floor((float)sizeSquare/2); j++)
		{
			if (face == 1) // plan X Z -Y
				pData[index].Position = D3DXVECTOR3((float)i, -(sizeSquare/2), (float)j);
			if (face == 2) // plan X Y +Z 
				pData[index].Position = D3DXVECTOR3((float)i, (float)j, sizeSquare/2);
			if (face == 3) // plan Y Z -X
				pData[index].Position = D3DXVECTOR3(-(sizeSquare/2), (float)i, (float)j);
			if (face == 4) // plan Y Z +X
				pData[index].Position = D3DXVECTOR3((sizeSquare/2), (float)i, (float)j);
			if (face == 5) // plan X Z +Y
				pData[index].Position = D3DXVECTOR3((float)i, (sizeSquare/2), (float)j);
			if (face == 6) // plan X Y -Z
				pData[index].Position = D3DXVECTOR3((float)i, (float)j, -sizeSquare/2);
			pData[index].color = D3DCOLOR_ARGB(255, 150,150,150);

			++index;
		}
	}
	return index;
}

int fillIndexBuffer(int* pDataIndex, int offset, int k, int s1, int s2, int s3, int s4, int sizeSquare)
{
	for (int i = (sizeSquare)*offset - offset; i < (sizeSquare)*(offset+1) - (offset+1); i++)
	{
		for (int j = sizeSquare*offset; j < (sizeSquare)*(offset+1) - 1; j++)
		{
			pDataIndex[k++] = sizeSquare*i + j; 
			pDataIndex[k++] = sizeSquare*i + j + s1;
			pDataIndex[k++] = sizeSquare*i + j + s2; 

			pDataIndex[k++] = sizeSquare*i + j; 
			pDataIndex[k++] = sizeSquare*i + j + s3;
			pDataIndex[k++] = sizeSquare*i + j + s4; 
		}
	}

	return k;
}
