#pragma once
#include"ObjectInfo.h"
#include"camera.h"
#include"Vector.h"
#include"Buffer.h"
 
using namespace VECTOR;

class PipelineController { 
	Object* m_renderTargets [100];
	int targetLength = 0;
	Buffer m_buffer;
	VECTOR4 m_penCol;
	Camera m_cam;
	ObjectHolder m_holder;
	float m_width, m_height;
	
public :
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
	void SampleColor(VECTOR4* A, VECTOR4 * B, VECTOR4* C, VECTOR4 * P, VECTOR4* res) {
		float a, b, c, total;
		AffineFunction(*A);
		AffineFunction(*B);
		AffineFunction(*C);
		AffineFunction(*P);
		c = TriangleArea(A->x - P->x, A->y - P->y, B->x - P->x, B->y - P->y);
		b = TriangleArea(A->x - P->x, A->y - P->y, C->x - P->x, C->y - P->y);
		a = TriangleArea(B->x - P->x, B->y - P->y, C->x - P->x, C->y - P->y);
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
		ReAffineFunction(*P);
	}
	float SampleDepth(VECTOR4* A, VECTOR4 * B, VECTOR4* C, VECTOR4 * P) {
		float a, b, c, total;
		c = TriangleArea(A->x - P->x, A->y - P->y, B->x - P->x, B->y - P->y);
		b = TriangleArea(A->x - P->x, A->y - P->y, C->x - P->x, C->y - P->y);
		a = TriangleArea(B->x - P->x, B->y - P->y, C->x - P->x, C->y - P->y);
		total = a + b + c;
		a = a / total;
		b = b / total;
		c = c / total;
		return A->z * a + B->z * b + C->z * c;
	}
	void DrawTriangle3(VERT*A , VERT*B , VERT * C, bool recurse = true);

	void DrawLine2(int x1, int  y1, int x2, int y2) {
		int tokenx = 1;
		int tokeny = 1;
		if (x1 > x2)
			tokenx = -1;
		if (y1 > y2)
			tokeny = -1;
		VECTOR4 col(0, 0, 0,1);

		int deltax = x1 - x2;
		int deltay = y1 - y2;
		bool movey = false;
		if (deltax * tokenx > deltay *tokeny)
			movey = true;
		float error = 0;
		float deltaerr = (float) deltay / deltax;
		if (movey) {
			int x = x1;
			for (int y = y1; y < y2; y+=tokeny) {
				//draw point  
				m_buffer.WriteToColorBuffer(x, y, &col);
				error = error + deltaerr;
				while (abs(error) >= 0.5) {
					x += tokenx;
					error -= tokenx;
					m_buffer.WriteToColorBuffer(x, y, &col);
				}
			}
		}
		else {
			int y = y1;
			for (int x = x1; abs(x - x2)> 0.5; x+=tokenx) {
				m_buffer.WriteToColorBuffer(x, y, &col);
				//draw point  
				error = error + deltaerr;
				if (abs(error) >= 0.5) {
					y += tokeny;
					error -= tokeny;
				}
			}
		}
	}
	void VertexShader(VECTOR4& viewDir,VERT& A ) {

	//	cout << A.normal->x << " " << A.normal->y << " " << A.normal->z << endl;
		VECTOR::minusV4(&viewDir,  m_cam.position, A.position);
		normalizedVector3(&viewDir, &viewDir);
		normalizedVector3(A.normal, A.normal);
		
		VECTOR4 r;
		VECTOR::dot(&r,A.normal, 2);

		add(&r, &r,& lightDir);

		normalizedVector3(&r,&r);

		float  sth = dot3x3(&r, &viewDir);
		sth = sth *0.5 + 0.5; 
	//	cout << " " << sth <<A.position->x << endl;
		
		dot(A.color, A.color, sth*1.2 );

	}
	void DrawPoint(float & depth,VERT*A,VERT* B ,VERT* C , VECTOR4& P, VECTOR4 & res ,VECTOR4& Col) {


		depth = SampleDepth(A->position, B->position, C->position, &P);
		P.z = depth;
		SampleColor(A->position, B->position, C->position, &P, &res);
		if (  ValidScreamPos(P.x, P.y) && depth > m_buffer.GetZBufferValue(P.x, P.y)) {
			m_buffer.WriteToZBuffer(depth, P.x, P.y);
			Col.x = A->color->x * res.x + B->color->x * res.y + C->color->x * res.z;
			Col.y = A->color->y * res.x + B->color->y * res.y + C->color->y * res.z;
			Col.z = A->color->z * res.x + B->color->z * res.y + C->color->z * res.z;

			m_buffer.WriteToColorBuffer(P.x, P.y, &Col);
		}
	}
};

void PipelineController::RenderAll() {
	m_buffer.ClearColBuffer(m_penCol);
	DrawLine2(1, 1,20, 200);
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
		DrawTriangle3(&(object.verts[a]), &(object.verts[b]), &(object.verts[c]));
	}
}
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

		float step = 0.3, dis = 0.3;
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
		VECTOR4 res, P, Col;
		float depth = -99;

		 

		while (sqdistance2D(&drawPointAB, B->position) > dis)
		{ 
			
			int i = drawPointAB.x, j = drawPointAB.y;
			P.x = i; P.y = j;
			DrawPoint(depth, A, B, C, P, res, Col);
			drawPointAB.x += delAB.x;
			drawPointAB.y += delAB.y;
			i = drawPointAB.x, j = drawPointAB.y;
			P.x = i; P.y = j;
			DrawPoint(depth, A, B, C, P, res, Col);
			if (j != y) {
				//draw point   
				continue;
			}
			while (sqdistance2D(&drawPointAC, C->position) > dis)
			{
				int k = drawPointAC.x, w = drawPointAC.y;
				P.x = k; P.y = w;
				DrawPoint(depth, A, B, C, P, res, Col);
				drawPointAC.x += delAC.x;
				drawPointAC.y += delAC.y;
				k = drawPointAC.x, w = drawPointAC.y;
				P.x = k; P.y = w;
				DrawPoint(depth, A, B, C, P, res, Col);
				if (w != y)
				{
					//draw point   
					continue;
				}
				int x = min(i, k), xm = max(i, k);
				P.y = y;
				for (int ii = 0; x + ii < xm; ii++) {
					P.x = x + ii;  
					DrawPoint(depth, A, B, C, P, res, Col);
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
}
//void PipelineController::RenderAll() {

//}
bool PipelineController::RemoveRenderTarget(Object ob) { 
	return false;
} 