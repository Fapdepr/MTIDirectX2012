#include "stdafx.h"
#include "3DTP.h"
#include <d3d9.h>
#include <d3dx9.h>

int fillVertexBuffer (vertex* pData, int index, int face, int sizeSquare);
int fillIndexBuffer(int* pDataIndex, int offset, int k, int s1, int s2, int s3, int s4, int sizeSquare);
