#pragma once
#include"ObjectInfo.h"
#include"camera.h"
#include"Vector.h"
#include"Buffer.h"
#include<thread>
using namespace std;
 
using namespace VECTOR;

struct MulThreadFACE {
	VERT *A;
	VERT *B;
	VERT *C;
};
class PipelineController { 
	Object* m_renderTargets [100];
	Object* m_TranparentTargets[100];
	int targetLength = 0;
	Buffer m_buffer;
	VECTOR4 m_penCol;
	Camera m_cam;
	ObjectHolder m_holder;
	float m_width, m_height;
	float m_lightIntense;
	
public :
	int m_maxThread;
	VECTOR4 lightDir;
	PipelineController();
	void CreateBuffer(int w, int h) {
		m_buffer.SetWidthHeight(w, h); 
		m_buffer.CreateBuffer(&m_penCol); 
		m_cam.SetWidthHeight(w, h);
		m_width = w; m_height = h;
	/*	int i;//do nothing
		cin >> i;
		*/
	} 
	const VECTOR4& GetBufferColor(int x, int y) {
		return m_buffer.GetColorBufferValue(x, y);
	}
	void SetPenCol(VECTOR4 * col) {
		copy(col, &m_penCol);
	}
	const Buffer * GetBuffer() {
		return &m_buffer;
	}
	ObjectHolder* GetObjectHolder() {
		return &m_holder;
	}
	void RenderAll();
	void AddRenderTarget(Object* ob); 
	bool RemoveRenderTarget(Object ob); 
	void RenderTarget(Object &  object);
	void CreateObject(string objName, string type, const VECTOR4* pos) {
		Object * output =  m_holder.CreateObject(objName, type,pos); 
		AddRenderTarget(output);
	}
	bool ValidScreamPos(int x, int y) {
		if (x >= 0 && x < m_width && y >= 0 && y < m_height)
			return true;
		return false;
	}
	Object * GetObject(string name );
	void GetVertsWorldSpace(Object* obj, MATRIX4x4* rotateMatrix, bool normalConvert = false) {
		MATRIX4x4* ma = new MATRIX4x4();
		GenerateTransformMatrix(obj->position, ma);
		for (int i = 0; i < obj->prefab->normalCount; i++) {

			matrixdot(obj->verts[i].normal, obj->prefab->n[i], rotateMatrix); 
		}
		for (int i = 0; i < obj->prefab->vertCount; i++) {
			matrixdot(obj->verts[i].position, obj->prefab->v[i], rotateMatrix);
			//normal convert
		//	cout << "Rot" << obj->verts[i].position->x << " " << obj->verts[i].position->y << " " << obj->verts[i].position->z << endl;


			matrixdot(obj->verts[i].position, obj->verts[i].position, ma);

	//		cout << obj->verts[i].position->x << " " << obj->verts[i].position->y << " " << obj->verts[i].position->z << endl;
			if (i % 3 == 0) {
				obj->verts[i].color->x = 1; 
			}
			else if (i % 3 == 1) {
				obj->verts[i].color->y = 1; 
			}
			else
				obj->verts[i].color->z = 1;
			obj->verts[i].color->x = 1;
			obj->verts[i].color->y = 1;
			obj->verts[i].color->z = 1;
		}
	} 
	float TriangleArea(int x0, int y0, int x1, int y1) {
		return abs(((float)(x0 *y1 - x1 * y0)) / 2);
	}
	void AffineFunction(VERT* A) {
		A->position->x = A->position->x / A->position->z;
		A->position->y = A->position->y / A->position->z;
	}
	void ReAffineFunction(VERT* A) {

		A->position->x = A->position->x * A->position->z;
		A->position->y = A->position->y * A->position->z;
	}
	void AffineFunction(VECTOR4& A) {
		A.x = A.x / A.z;
		A.y = A.y / A.z;
	}
	void ReAffineFunction(VECTOR4& A) {
		A.x = A.x * A.z;
		A.y = A.y * A.z;
	}
	void ReAffineFunction(int& Px, int& Py , float& Pz) {
		Px =   Px * Pz;
		Py = Py * Pz;
	}
	void AffineFunction(int& Px, int& Py, float& Pz) {
		Px = Px / Pz;
		Py = Py / Pz;
	}
	void SampleColor(VECTOR4* A, VECTOR4 * B, VECTOR4* C, int Px ,int Py , float Pz, VECTOR4* res) {
		float a, b, c, total;
		AffineFunction(*A);
		AffineFunction(*B);
		AffineFunction(*C);
		AffineFunction(Px,Py,Pz);
		c = TriangleArea(A->x - Px, A->y - Py, B->x - Px, B->y - Py);
		b = TriangleArea(A->x - Px, A->y - Py, C->x - Px, C->y - Py);
		a = TriangleArea(B->x - Px, B->y - Py, C->x - Px, C->y - Py);
		total = a + b + c;
		a = a / total;
		b = b / total;
		c = c / total;
		res->x = a;
		res->y = b;
		res->z = c;
		ReAffineFunction(*A);
		ReAffineFunction(*B);
		ReAffineFunction(*C); 
	}
	float SampleDepth(VECTOR4* A, VECTOR4 * B, VECTOR4* C, int Px,int Py) {
		float a, b, c, total;
		c = TriangleArea(A->x - Px, A->y - Py, B->x - Px, B->y - Py);
		b = TriangleArea(A->x - Px, A->y - Py, C->x - Px, C->y - Py);
		a = TriangleArea(B->x - Px, B->y - Py, C->x - Px, C->y - Py);
		total = a + b + c;
		a = a / total;
		b = b / total;
		c = c / total;
		return A->z * a + B->z * b + C->z * c;
	}
	void DrawTriangle3(VERT*A , VERT*B , VERT * C, bool recurse = true);
	//void DrawTriangle4(VERT*A, VERT*B, VERT * C, bool recurse = true);
	/*
	void DrawLine2(int x1, int  y1, int x2, int y2,VECTOR2* point ) { 
		cout << x1 << " " << x2 << " " << y1 << " " << " " << y2 << endl;
		int tokenx = 1;
		int tokeny = 1;
		if (x1 > x2)
			tokenx = -1;
		if (y1 > y2)
			tokeny = -1;
		int count = 0;
		if (x1 == x2) {
			for (int i = 0; abs(tokeny * i + y1 - y2 ) >= 1; i++) { 
				point[count].x = x1; point[count++].y = tokeny * i + y1;
			}
			return;
		}
		if (y1 == y2) {
			for (int i = 0; abs( tokenx * i + x1 - x2 ) >= 1; i++) { 
				point[count].y = y1; point[count++].x = tokenx * i + x1;
			}
			return;
		}
		VECTOR4 col(0, 0, 0,1);

		int deltax = x1 - x2;
		int deltay = y1 - y2;
		bool movey = false;
		if (deltax * tokenx > deltay *tokeny)
			movey = true;
		float error = 0;
		float deltaerr;
		if (movey) {
			int x = x1;
			for (int y = y1; abs (y- y2 ) > 0.5; y+=tokeny) {
				//draw point  
				//m_buffer.WriteToColorBuffer(x, y, &col);
				point[count].x = x; point[count++].y = y;
				deltaerr = (float)  deltax/deltay;
				error = error + deltaerr;
				if (abs(error) >= 0.5) {
					x += tokenx;
					error -= tokenx; 
				}
			}
		}
		else {
			int y = y1;
			for (int x = x1; abs(x - x2)> 0.5; x+=tokenx) {
			//	m_buffer.WriteToColorBuffer(x, y, &col);
				//draw point  
				deltaerr = (float)deltay / deltax;
				point[count].x = x; point[count++].y = y;
				error = error + deltaerr;
				if (abs(error) >= 0.5) {
					y += tokeny;
					error -= tokeny;
				}
			}
		}
	}
	*/

	void DrawLine2(int x0, int y0 , int x1 , int y1,VECTOR2 * point) {   
		bool steep = abs(y1 - y0) > abs(x1 - x0);
		if (steep) {
			swap(x0, y0);
			swap(x1, y1);

		}
		if (x0 > x1) {
			swap(x0, x1);
			swap(y0, y1);
		}

		int deltax = x1 - x0;
		int deltay = abs(y1 - y0);
		int error = deltax / 2;
		int ystep;
		int  y = y0;

		if (y0 < y1) {
			ystep = 1;
		}
		else
			ystep = -1;
		int count = 0;
		for (int i = 0; x0 + i < x1; i++) {
			if (steep) {
				point[count].x = y;
				point[count].y = x0 + i; 
			}
			else {
				point[count].x = x0 + i;
				point[count].y = y;
			} 
			error = error - deltay;
			if (error < 0)
			{
				y = y + ystep;
				error = error + deltax;
			}
		}

	}
	void VertexShader(VECTOR4& viewDir,VERT& A ) {

	//	cout << A.normal->x << " " << A.normal->y << " " << A.normal->z << endl;
		VECTOR::minusV4(&viewDir,  m_cam.position, A.position);
		normalizedVector3(&viewDir, &viewDir);
		normalizedVector3(A.normal, A.normal);
		float amb = dot3x3(&lightDir, A.normal);
		
		VECTOR4 r;
		VECTOR::dot(&r,A.normal, 2 * dot3x3 (A.normal,&lightDir));

		minusV3(&r, &r,& lightDir);

		normalizedVector3(&r,&r);
		float  sth = dot3x3(&r, &viewDir);
		sth = sth *0.5 + 0.5; 
		sth = sth * sth *sth;
		amb = amb * 0.5 + 0.5;

	//	cout << " " << sth <<A.position->x << endl;
		
		dot(A.color, A.color, (sth+amb) * m_lightIntense );

	}
	void DrawPoint(float & depth,VERT*A,VERT* B ,VERT* C , int Px,int Py, VECTOR4 & res ,VECTOR4& Col) { 
		//fragment shader apply in here

		depth = SampleDepth(A->position, B->position, C->position, Px,Py);
		float Pz = depth;
		VECTOR4 normal;
		SampleColor(A->position, B->position, C->position, Px,Py,Pz, &res);
		if (  ValidScreamPos(Px, Py) && depth > m_buffer.GetZBufferValue(Px, Py)) {
			m_buffer.WriteToZBuffer(depth, Px, Py);
			Col.x = A->color->x * res.x + B->color->x * res.y + C->color->x * res.z;
			Col.y = A->color->y * res.x + B->color->y * res.y + C->color->y * res.z;
			Col.z = A->color->z * res.x + B->color->z * res.y + C->color->z * res.z; 


			m_buffer.WriteToColorBuffer(Px, Py, &Col);
		}
	}
};
void MultyFaceDrawCall(MulThreadFACE fa) {

//	pipeline.DrawTriangle3(fa.A, fa.B, fa.C); 
//	pipeline.m_maxThread += 1;
}
void PipelineController::RenderAll() {
	m_buffer.ClearColBuffer(m_penCol);
	VECTOR2 aa[2000];
	DrawLine2(821,755,812, 727,aa);
	for (int i = 0; i < targetLength; i++) {
		RenderTarget(*m_renderTargets[i]);
	}
	m_buffer.SwapBuffer();
}

void PipelineController::RenderTarget(Object &  object) {

	VECTOR4 viewDir(0, 0, 0, 0);
	int a, b, c;
	int na, nb, nc;
	MATRIX4x4 rot,ma;
	GenerateRotateMatrix(*object.rotation, &rot);  
	GenerateTransformMatrix(object.position, &ma);
	GetVertsWorldSpace(&object, &rot,true);
	m_cam.GetCamCoordinateTransformVert(&object);
	for (int i = 0; i < object.prefab->faceCount; i++) {
		a = object.prefab->f[i]->a.x - 1;
		b = object.prefab->f[i]->b.x - 1;
		c = object.prefab->f[i]->c.x - 1;
		na = object.prefab->f[i]->a.z - 1;
		nb = object.prefab->f[i]->b.z - 1;
		nc = object.prefab->f[i]->c.z - 1;
		VERT& A = object.verts[a], &B = object.verts[b], &C = object.verts[c]; 

		copy(object.prefab->n[na], A.normal);
		copy(object.prefab->n[nb], B.normal);
		copy(object.prefab->n[nc], C.normal);
		matrixdot(A.normal, A.normal, &rot);
		matrixdot(B.normal, B.normal, &rot);
		matrixdot(C.normal, C.normal, &rot);
		VertexShader(viewDir, A);
		VertexShader(viewDir, B);
		VertexShader(viewDir, C);
	}
	
	m_cam.GetClipSpaceTransfromVert(&object);
	m_cam.GetUV(&object);
	VECTOR2 va, vb, vc;
	VECTOR4 col(0.1, 0.1, 0.1, 1); 
	for (int i = 0; i < object.prefab->faceCount; i++) {
		a = object.prefab->f[i]->a.x - 1;
		b = object.prefab->f[i]->b.x - 1;
		c = object.prefab->f[i]->c.x - 1;
		VECTOR2 vsss[1920];
		DrawLine2(object.verts[a].position->x, object.verts[a].position->y, object.verts[b].position->x,object.verts[b].position->y, vsss);
		DrawLine2(object.verts[a].position->x, object.verts[a].position->y, object.verts[c].position->x, object.verts[c].position->y, vsss);
		DrawLine2(object.verts[c].position->x, object.verts[c].position->y, object.verts[b].position->x, object.verts[b].position->y, vsss);
		DrawTriangle3(&(object.verts[a]), &(object.verts[b]), &(object.verts[c])); 
		if (m_maxThread > 0) {
			MulThreadFACE fa;
			m_maxThread -= 1; 
		}
	} 
} 
/*
void PipelineController::DrawTriangle4(VERT* A, VERT* B, VERT * C, bool recurse) {
	{

		if (recurse) {
			if ((A->position->y == B->position->y || A->position->y == C->position->y)) {
				if (A->position->y == B->position->y) {
					DrawTriangle3(C, B, A, false);
					return;
				}
				if (A->position->y == C->position->y) {
					DrawTriangle3(B, C, A, false);
					return;
				}
			} 
		}
		 
		int sizeAB = 0,sizeAC = 0;
		if (abs(A->position->y - B->position->y) > abs(A->position->x - B->position->x)) {
			sizeAB =abs( A->position->y - B->position->y ) ;
		}
		else {
			sizeAB = abs(A->position->x - B->position->x);
		}
		VECTOR2 pointAB [1920] , pointAC[1920];
		if (abs(A->position->y -C->position->y) > abs(A->position->x -C->position->x)) {
			sizeAC = abs(A->position->y - C->position->y);
		}
		else {
			sizeAC = abs(A->position->x - C->position->x);
		}
		DrawLine2(A->position->x, A->position->y, B->position->x, B->position->y,pointAB);
		DrawLine2(A->position->x, A->position->y, C->position->x, C->position->y, pointAC);

		int sizeMin = 0;
		if (abs(A->position->y - B->position->y) > abs(A->position->y - C->position->y)) {
			sizeMin = sizeAC;
		}
		else
			sizeMin = sizeAB;
		VECTOR4 P, res, Col;
		float depth;
		for (int i = 0; i < sizeMin; i++)  {
			for (int p = pointAB[i].x; p <= pointAC[i].x; p++) {
				//draw point p , y  here 
				P.x = p; P.y = pointAB[i].y;
				DrawPoint(depth, A, B, C, P, res, Col);
			} 
		}
		if (pointAB[sizeAB - 1].y != pointAC[sizeAC - 1].y) { 
			VECTOR2 posOne, posTwo,posC; 
			posOne = pointAB[sizeAB - 1];
			posTwo = pointAC[sizeAC - 1];
			int i;
			bool ac = false;
			if (abs(A->position->y - posOne.y) > abs(A->position->y - posTwo.y)) {
				
				for (i = 0; i < sizeAB; i++) {
					if (pointAB[i].y == posTwo.y) {
						posC = pointAB[i];
						break;
					}
				}
			}
			else {
				ac = true;
				for (i = 0; i < sizeAC; i++) {
					if (pointAC[i].y == posOne.y) {
						posC = pointAC[i];
						break;
					}
				} 
			}
			int sizeOT;
			if (abs(posOne.y - posTwo.y) > abs(posOne.x - posTwo.x)) {
				sizeOT = abs(posOne.y - posTwo.y);
			}
			else
				sizeOT = abs(posOne.x - posTwo.x);
			VECTOR2 pointOT[1920];
			DrawLine2(posOne.x, posOne.y, posTwo.x, posTwo.y,pointOT);
			int maxsize;
			if (ac)
				maxsize = sizeAC;
			else
				maxsize = sizeAB;
			for (int j = 0; j < sizeOT && i + j < maxsize; ) {
				j++; 
				//raster here
				if (ac) { 
					for (int p = pointOT[j].x; p <= pointAC[i + j].x; p++) {
						//draw point p , y  here 
						P.x = p; P.y = pointOT[j].y;
						DrawPoint(depth, A, B, C, P, res, Col); 
					}
				}
			}


		}
	}
}*/
void PipelineController::DrawTriangle3(VERT* A, VERT* B, VERT * C, bool recurse ) {
	{
		if (recurse == true) {

			if ((A->position->y == B->position->y || A->position->y == C->position->y)) {
				if (A->position->y == B->position->y) {
					DrawTriangle3(C, B, A, false);
					return;
				}
				if (A->position->y == C->position->y) {
					DrawTriangle3(B, C, A, false);
					return;
				}
			}
		}
		if (B->position->y == C->position->y) 
			recurse = false;
		if (recurse) {
			recurse = false;
			DrawTriangle3(B, C, A, false);
			DrawTriangle3(C, B, A, false);
		}

		float step =1, dis = 0.3;
		VECTOR2 delAB, delAC;
		VECTOR4	drawPointAB, drawPointAC;
		delAB.x = B->position->x - A->position->x;
		delAB.y = B->position->y - A->position->y;

		delAC.x = C->position->x - A->position->x;
		delAC.y = C->position->y - A->position->y;


		drawPointAB.x = A->position->x; drawPointAB.y = A->position->y;
		drawPointAC.x = A->position->x; drawPointAC.y = A->position->y;

		normalized(&delAB);
		normalized(&delAC);

		delAB.x *= step;
		delAB.y *= step;
		delAC.x *= step;
		delAC.y *= step;
		int y = A->position->y;
		int dir = 0;
		if (A->position->y - B->position->y > 0) {
			dir = -1;
		}
		else
			dir = 1;
		y += dir;
		VECTOR4 res,  Col;
		float depth = -99;
		int Px, Py;
		 

		while (sqdistance2D(&drawPointAB, B->position) > dis)
		{  
			int i = drawPointAB.x, j = drawPointAB.y;
			Px = i; Py = j;
			DrawPoint(depth, A, B, C, Px,Py, res, Col);
			drawPointAB.x += delAB.x;
			drawPointAB.y += delAB.y;
			i = drawPointAB.x, j = drawPointAB.y;
			Px = i; Py = j;
			DrawPoint(depth, A, B, C, Px,Py, res, Col);
			if (j != y) {
				//draw point   
				continue;
			}
			while (sqdistance2D(&drawPointAC, C->position) > dis)
			{
				int k = drawPointAC.x, w = drawPointAC.y;
				Px = k; Py = w;
				DrawPoint(depth, A, B, C, Px,Py, res, Col);
				drawPointAC.x += delAC.x;
				drawPointAC.y += delAC.y;
				k = drawPointAC.x, w = drawPointAC.y;
				Px = k; Py = w;
				DrawPoint(depth, A, B, C, Px,Py, res, Col);
				if (w != y)
				{
					//draw point   
					continue;
				}
				int x = min(i, k), xm = max(i, k);
				//cout << i <<"  " <<j<< " " << k << " " << w<< endl;
				for (int ii = 0; x + ii < xm; ii++) {
					Px = x + ii;  
					Py = y;
					DrawPoint(depth, A, B, C, Px,Py, res, Col);
				}
				y += dir; 
				break;
			} 

		}
	}
}
Object* PipelineController::GetObject(string name) {

	for (int i = 0; i < targetLength; i++) {
		if (m_renderTargets[i]->objectName == name) {
			return m_renderTargets[i];
		}
	}
}
void PipelineController::AddRenderTarget(Object* ob) {
	for (int i = 0; i < targetLength; i++) {
		if (m_renderTargets[i]->objectName == ob->objectName) {
			return;
		}
	}
	m_renderTargets[targetLength++] = ob;
}
PipelineController::PipelineController() {
	VECTOR4 col(0.953, 0.447, 0.8156, 1);
	copy(&col, &m_penCol); 
	lightDir.x = 0; lightDir.y = 0; lightDir.z = -1; lightDir.w = 0;
	normalizedVector3(&lightDir, &lightDir);
	m_lightIntense = 1;
	m_maxThread = 5;
}
//void PipelineController::RenderAll() {

//}
bool PipelineController::RemoveRenderTarget(Object ob) { 
	return false;
} 