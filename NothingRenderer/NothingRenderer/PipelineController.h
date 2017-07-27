#pragma once
#include"ObjectInfo.h"
#include"camera.h"
#include"Vector.h"
#include"Buffer.h"
#include<thread>
#include<mutex>	
#include"BMPManager.h"
#include<algorithm>
using namespace std;
 
using namespace VECTOR;

struct MulThreadFACE {
	VERT *A;
	VERT *B;
	VERT *C;
};

struct DrawTrianglePoint {
	float x, y, tarX, tarY;
	VERT* FROM, *TO;
	int count;
	int dis;
	VECTOR2 dir;
	void Move() {
		x += dir.x; y += dir.y;
	}
};
class PipelineController { 
	Object* m_renderTargets [100];
	Object* m_TranparentTargets[100];
	int targetLength = 0;
	Buffer m_buffer;
	VECTOR4 m_penCol;
	Camera m_cam;
	ObjectHolder m_holder;
	BMPManager m_BMPManager;
	float m_width, m_height;
	float m_lightIntense;
	mutex m_locker;
public :
	int done = 0;
	int m_threads;
	VECTOR4 lightDir;
	PipelineController();
	void CreateBuffer(int w, int h) {
		m_buffer.SetWidthHeight(w, h); 
		m_buffer.CreateBuffer(&m_penCol); 
		m_cam.SetWidthHeight(w, h);
		m_width = w; m_height = h; 
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
	void LoadPicture() {
		m_BMPManager.BMPPath = "./Objects/cube.bmp";
		m_BMPManager.LoadBMPPicture(); 
	}
	void ShowBMPPicture() { 
		int w = m_BMPManager.width, h = m_BMPManager.height;
		for (int i = 1; i <= h; i++) {
			for (int j = 1; j <= w ; j++) {
				m_buffer.WriteToColorBuffer(j, i, m_BMPManager.GetBMPColor(j, i));
			}
		}
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
	void DrawLine(int x0, int x1, int y0, int y1) {
		VECTOR2 dir;
		dir.x = x1 - x0;
		dir.y = y1 - y0;
		int dis = sqrt(dir.x * dir.x + dir.y * dir.y);
		normalized(dir);
		DrawTrianglePoint p;
		p.x = x0; p.y = y0;
		p.dir.x = dir.x; p.dir.y = dir.y;

		for (int i = 0; i < dis; i++) {
			DrawPoint(p);
			p.Move();
		}

	}
	void GetVertsWorldSpace(Object* obj, MATRIX4x4* rotateMatrix, bool normalConvert = false) {
		MATRIX4x4* ma = new MATRIX4x4();
		GenerateTransformMatrix(obj->position, ma);
		for (int i = 0; i < obj->prefab->normalCount; i++) {

			matrixdot(obj->verts[i].normal, obj->prefab->n[i], rotateMatrix); 
		}
		for (int i = 0; i < obj->prefab->vertCount; i++) {
			matrixdot(obj->verts[i].position, obj->prefab->v[i], rotateMatrix); 
			matrixdot(obj->verts[i].position, obj->verts[i].position, ma); 
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
	float TriangleArea(float x0, float y0, float x1, float y1) {
		return abs( ( x0 *y1 -  x1 * y0)/ 2.0f);
	}  
	float TriangleArea(float x0, float y0, float x1, float y1,float x2 , float y2) {
		return abs(  (x0 * (y1 - y2) + x1 * (y2 - y0) + x2* (y0 - y1))/2.0 );
	}
	void SampleColor(VECTOR4* A , VECTOR4 * B ,  VECTOR4* C,  float Px ,float Py , float pw, VECTOR3* res) {
		float a, b, c, total;
		int x1, y1, x2, y2, x3, y3;
		x1 = A->x /A->w;
		y1 = A->y /A->w;
		x2 = B->x /B->w;
		y2 = B->y /B->w;
		x3 = C->x /C->w;
		y3 = C->y /C->w;
		Px = Px  /pw;
		Py = Py /pw;
		c = ((y1 - y2) * Px + (x2 - x1) * Py + x1 * y2 - x2 * y1) /  (float) ((y1 - y2) * x3 + (x2 - x1) * y3 + x1 * y2 - x2 * y1)  ; 
		b = ((y1 - y3) * Px + (x3 - x1) * Py + x1 * y3 - x3 * y1) /(float) ((y1 - y3) * x2 + (x3 - x1) * y2 + x1 * y3 - x3 * y1) ; 
		a = 1 - b - c;  
		res->x = a;
		res->y = b;
		res->z = c; 
	} 
	float SampleDepth(VECTOR4* A  ,VECTOR4 * B,    VECTOR4* C ,int Px,int Py) {
		float a, b, c, total;
		/*
		c = TriangleArea(A->x - Px, A->y - Py, B->x - Px, B->y - Py);
		b = TriangleArea(A->x - Px, A->y - Py, C->x - Px, C->y - Py); 
		a = TriangleArea(B->x - Px, B->y - Py, C->x - Px, C->y - Py);*/
		c = TriangleArea(A->x, A->y, B->x, B->y, Px, Py);
		b = TriangleArea(A->x, A->y, C->x, C->y, Px, Py);
		a = TriangleArea(C->x, C->y, B->x, B->y, Px, Py);
		total = a + b + c;
		a = a / total;
		b = b / total;
		c = c / total;  
		return A->w * a + B->w * b + C->w * c;
	} 
	void VertexShader(VECTOR4& viewDir,VERT& A ) {
		 
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
		
		dot(A.color, A.color, (sth+amb) * m_lightIntense );
	}  
	
	void Swap(DrawTrianglePoint& a, DrawTrianglePoint& b) {
		DrawTrianglePoint c;
		c.FROM = a.FROM;
		a.FROM = b.FROM; b.FROM = c.FROM;

		c.TO = a.TO;
		a.TO = b.TO; b.TO = c.TO;

		c.count = a.count; 
		a.count = b.count; b.count = c.count;

		c.dis = a.dis; 
		a.dis = b.dis; b.dis = c.dis;

		c.dir.x = a.dir.x; c.dir.y = a.dir.y;
		a.dir.x = b.dir.x; a.dir.y = b.dir.y;
		b.dir.x = c.dir.x; b.dir.y = c.dir.y;

		c.x = a.x; c.y = a.y;
		a.x = b.x; a.y = b.y; b.x = c.x; b.y = c.y;


	}
	void DrawPoint(DrawTrianglePoint a) {
		if (!ValidScreamPos(a.x, a.y))
			return;
		VECTOR4 col(1, 1, 1, 0); 
		float alpha = 1 -(float)a.count / a.dis;
		float depth = a.FROM->zValue * alpha + a.TO->zValue * (1 - alpha);
		if (depth > m_buffer.GetZBufferValue(a.x, a.y)) {
			m_buffer.WriteToZBuffer(depth, a.x, a.y);
		}
		else
			return;
	//	col.x = a.FROM->color->x * alpha + a.TO->color->x * (1- alpha);
	//	col.y = a.FROM->color->y * alpha + a.TO->color->y * (1 - alpha);
	//	col.z = a.FROM->color->z * alpha + a.TO->color->z * (1 - alpha);  
		m_buffer.WriteToColorBuffer(a.x, a.y, &col); 
	}
	void RasterLine(DrawTrianglePoint a,DrawTrianglePoint b) { 
		int xl = a.x, xr = b.x; 
		int errorPoint = 0, normalPoint = 0;
		if (xl == xr) {
			DrawPoint(a);
			DrawPoint(b);
			cout << "triangle same " << endl;
			return;
		}

		VECTOR4 colL, colR(1,1,1,0); 
		float alpha = 1 - (float)a.count / a.dis;
		float beta = 1- (float)b.count / b.dis; 
		float dl = a.FROM->zValue * alpha + a.TO->zValue * (1- alpha), dr = b.FROM->zValue * beta + b.TO->zValue * (1- beta);
		float wl = a.FROM->position->w * alpha + a.TO->position->w  * (1 - alpha), wr = b.FROM->position->w  * beta + b.TO->position->w  * (1 - beta);
		float gamma = 0,depth;
		VECTOR4 col(1, 1, 1, 0); 
		bool lr = true;
		if (xl > xr) {
			swap(xl, xr); 
			lr = false;
		}  
		for (int i = 0; xl + i <= xr; i++) {
			if (!ValidScreamPos(xl + i, a.y)) { 
	//			errorPoint++;
				continue;
			}
			if (lr)
				gamma = 1 - i / (float)(xr - xl);
			else
				gamma = (float)i / (float)(xr - xl);
			depth = dl * gamma + dr * (1 - gamma);
			if (depth > m_buffer.GetZBufferValue(xl + i, a.y)) { 
				m_buffer.WriteToZBuffer(depth, xl + i, a.y); 
			}
			else {
		//		errorPoint++;
				continue;
			}
			VECTOR3 res;
			float pw = wl * gamma + wr * (1 - gamma);  
			float u, v;
			if (a.FROM == b.FROM) {  
				SampleColor(a.FROM->position, a.TO->position, b.TO->position, xl + i, a.y, pw, &res);
				u = a.FROM->tv->x * res.x + a.TO->tv->x * res.y + b.TO->tv->x * res.z;
				v = a.FROM->tv->y * res.x + a.TO->tv->y * res.y + b.TO->tv->y * res.z;
				if (res.x < 0 || res.x > 1 || res.y < 0 || res.y > 1 || res.z < 0 || res.z > 1) {
					errorPoint++;
					continue;
				}
			}
			else {
				SampleColor(a.FROM->position, a.TO->position, b.FROM->position, xl + i, a.y, pw, &res);
				u = a.FROM->tv->x * res.x + a.TO->tv->x * res.y + b.FROM->tv->x * res.z;
				v = a.FROM->tv->y * res.x + a.TO->tv->y * res.y + b.FROM->tv->y * res.z;
				if (res.x < 0 || res.x > 1 || res.y < 0 || res.y > 1 || res.z < 0 || res.z > 1) {
					errorPoint++;
					continue;
				}
			} 
			normalPoint++;
			m_buffer.WriteToColorBuffer(xl + i, a.y,m_BMPManager.GetBMPColor(u,v));
		} 
		cout << "error point " << errorPoint << "   " << "normal poinr " << " " << normalPoint << endl;
	}  
	void DrawTriangle(VERT* A, VERT* B, VERT* C) {
		int yDir = 1;
		if (B->uv->y < A->uv->y) {
			VERT* temp = A;A = B; B = temp;
		}
		if (C->uv->y < A->uv->y) {
			VERT* temp = A; A = C; C = temp; 
		}
		if (C->uv->y < B->uv->y) {
			VERT* temp = B; B =C; C = temp;
		}
		if (B->uv->y == A->uv->y) {
			VERT* temp = A; A = C;C = temp; 
		}
		if (C->uv->y == A->uv->y) {
			VERT* temp = A; A = B; B = temp; 
		}
		if (A->uv->y > B->uv->y)
			yDir = -1;
		float disAB = distance2D(*(A->uv), *(B->uv)), disAC = distance2D(*(A->uv), *(C->uv));
		VECTOR2 dir;
		dir.x = B->uv->x - A->uv->x;
		dir.y = B->uv->y - A->uv->y;
		normalized(dir); 
		DrawTrianglePoint ab; ab.dir.x = dir.x, ab.dir.y = dir.y;
		ab.x = A->uv->x; ab.y = A->uv->y;
		ab.tarX = B->uv->x; ab.tarY = B->uv->y;
		ab.dis = disAB; ab.count = 0;
		dir.x = C->uv->x - A->uv->x;
		dir.y = C->uv->y - A->uv->y;
		normalized(dir); ab.FROM = A; ab.TO = B;

		DrawTrianglePoint ac; ac.dir.x = dir.x, ac.dir.y = dir.y;
		ac.x = A->uv->x; ac.y = A->uv->y;
		ac.tarX = C->uv->x; ac.tarY = C->uv->y;
		ac.dis = disAC; ac.count = 0;
		ac.FROM = A; ac.TO = C;
		int y = A->uv->y;

		if (A->uv->y == B->uv->y && A->uv->y == C->uv->y)
		{ 
			return;
		}
		for (; ab.count < disAB && ac.count < disAC;) {  

			while ((int)ab.y != y )
			{
				ab.count++;	ab.x += ab.dir.x; ab.y += ab.dir.y; DrawPoint(ab);
				if (ab.count > disAB )
					break;
			}
			while ((int)ac.y != y)
			{
				ac.count++;  ac.Move(); DrawPoint(ac);
				if (ac.count > disAC )
					break;
			} 
			RasterLine(ab, ac);
			y += yDir;
		}
		if (B->uv->y == C->uv->y) {
			return;
		}
		DrawTrianglePoint la;

		float disbc = distance2D(*(B->uv), *(C->uv));
		la.count = 0; la.dis = disbc;
		if (ac.count >= disAC) {
			la.x = C->uv->x; la.y = C->uv->y;
			dir.x = B->uv->x - C->uv->x; dir.y = B->uv->y - C->uv->y;
			normalized(dir); la.dir.x = dir.x; la.dir.y = dir.y; 
			la.FROM = C; la.TO = B;
		}
		if (ab.count >= disAB) { 
			la.x = B->uv->x; la.y = B->uv->y;
			dir.x = C->uv->x - B->uv->x; dir.y = C->uv->y - B->uv->y;
			normalized(dir); la.dir.x = dir.x; la.dir.y = dir.y;
			la.FROM = B; la.TO = C;
			ab.count = ac.count;	Swap(ab, ac); disAB = disAC;
		}  
		for (; ab.count < disAB && la.count < disbc;) {   
			while ((int)ab.y != y)
			{
				ab.count++;	ab.Move(); DrawPoint(ab);
			}
			while ((int)la.y != y)
			{
				la.count++;  la.Move(); DrawPoint(la);
			}
			RasterLine(ab, la); 
			y++;
		} 
		cout << "do nothing" << endl;
	}
	void ThreadDone() {
		m_locker.lock(); 
		done++;
		m_locker.unlock();
	}
};  
void MultiFaceDraw(Object & object , int from ,int to,VECTOR4* viewDir,PipelineController* pipeline) {
	
	VECTOR3 ab, bc, normal;
	int a, b, c; 
	VERT* A, *B, *C;
	VECTOR2 va, vb, vc;
	for (int i = from; i < to; i++) {
		cout << " draw face " << i << endl;
		a = object.prefab->f[i]->a.x - 1;
		b = object.prefab->f[i]->b.x - 1;
		c = object.prefab->f[i]->c.x - 1;
		A = &(object.verts[a]); B = &(object.verts[b]); C = &(object.verts[c]);
		a = object.prefab->f[i]->a.y - 1;
		b = object.prefab->f[i]->b.y - 1;
		c = object.prefab->f[i]->c.y - 1;
		copy(object.prefab->t[a], A->tv);
		copy(object.prefab->t[b], B->tv);
		copy(object.prefab->t[c], C->tv);
		ab.x = B->uv->x - A->uv->x;
		ab.y = B->uv->y - A->uv->y;
		ab.z = B->zValue - A->zValue;
		bc.x = C->uv->x - B->uv->x;
		bc.y = C->uv->y - B->uv->y;
		bc.z = C->zValue - B->zValue;
		crossV3(normal, ab, bc);
		normalized(normal);
		if (dotV3(normal, *viewDir) < 0)
			continue;
		A->color->x = (float)i / 8; A->color->y = (float)i / 8; A->color->z = (float)i / 8;
		B->color->x = (float)i / 8; B->color->y = (float)i / 8; B->color->z = (float)i / 8;
		C->color->x = (float)i / 8; C->color->y = (float)i / 8; C->color->z = (float)i / 8;
		pipeline->DrawTriangle(A, B, C);
	}

	pipeline->ThreadDone();

}
void PipelineController::RenderAll() {
	m_buffer.ClearColBuffer(m_penCol);

	for (int i = 0; i < targetLength; i++) {
		RenderTarget(*m_renderTargets[i]);
	}
	ShowBMPPicture();
	m_buffer.SwapBuffer();
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
	m_threads = 5;
}

bool PipelineController::RemoveRenderTarget(Object ob) {
	return false;
}
void PipelineController::RenderTarget(Object &  object) {

	VECTOR4 viewDir(0, 0, 0, 0);
	int a, b, c;
	int na, nb, nc;
	VECTOR2 va, vb, vc;
	MATRIX4x4 rot, ma;
	GenerateRotateMatrix(*object.rotation, &rot);
	GenerateTransformMatrix(object.position, &ma);
	GetVertsWorldSpace(&object, &rot, true);
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
	//	VertexShader(viewDir, A);
	//	VertexShader(viewDir, B);
	//	VertexShader(viewDir, C);
	}
	m_cam.GetClipSpaceTransfromVert(&object);
	VECTOR4 col(0.1, 0.1, 0.1, 1);
	VECTOR3 ab, bc, normal;
	minusV3(&viewDir, m_cam.position, object.position);
	
	

	int i = 7;
	
	/*
	int threadCount = 5;
	int threadMissionCount = object.prefab->faceCount / threadCount;
	int startCount = 0,endCount = threadMissionCount;
	
	thread t1(MultiFaceDraw, ref(object), startCount, endCount, &viewDir, this);
	startCount = endCount; endCount += threadMissionCount;
	thread t2(MultiFaceDraw, ref(object), startCount, endCount, &viewDir, this);
	startCount = endCount; endCount += threadMissionCount;
	thread t3(MultiFaceDraw, ref(object), startCount, endCount, &viewDir, this);
	startCount = endCount; endCount += threadMissionCount;
	thread t4(MultiFaceDraw, ref(object), startCount, endCount, &viewDir, this);
	startCount = endCount; endCount += threadMissionCount;
	thread t5(MultiFaceDraw, ref(object), startCount, object.prefab->faceCount, &viewDir, this);
	t1.join(); t2.join(); t3.join(); t4.join();
	t5.join();

	while (done != 5)
	{

	}
	done = 0;*/
	 
	//for (; i < object.prefab->faceCount; i++) {
		VERT* A, *B, *C;
		a = object.prefab->f[i]->a.x - 1;
		b = object.prefab->f[i]->b.x - 1;
		c = object.prefab->f[i]->c.x - 1;
		A = &(object.verts[a]); B = &(object.verts[b]); C = &(object.verts[c]);
		a = object.prefab->f[i]->a.y - 1;
		b = object.prefab->f[i]->b.y - 1;
		c = object.prefab->f[i]->c.y - 1;
		copy(object.prefab->t[a], A->tv);
		copy(object.prefab->t[b], B->tv);
		copy(object.prefab->t[c], C->tv);
		ab.x = B->uv->x - A->uv->x;
		ab.y = B->uv->y - A->uv->y;
		ab.z = B->zValue - A->zValue;
		bc.x = C->uv->x - B->uv->x;
		bc.y = C->uv->y - B->uv->y;
		bc.z = C->zValue - B->zValue;
		crossV3(normal, ab, bc);
		normalized(normal);
	//	if (VECTOR::dotV3(normal, viewDir) < 0)
	//		continue;    
		A->color->x = (float)i / 8; A->color->y = (float)i / 8; A->color->z = (float)i / 8;
		B->color->x = (float)i / 8; B->color->y = (float)i / 8; B->color->z = (float)i / 8;
		C->color->x = (float)i / 8; C->color->y = (float)i / 8; C->color->z = (float)i / 8;  
		DrawTriangle(A, B, C);
//	}  
	//VERT* A, *B, *C;
	//a = object.prefab->f[0]->a.x - 1;
	//b = object.prefab->f[0]->b.x - 1;
	//c = object.prefab->f[0]->c.x - 1;
	//A = &(object.verts[a]); B = &(object.verts[b]); C = &(object.verts[c]);
	////	a = object.prefab->f[i]->a.y - 1;
	////	b = object.prefab->f[i]->b.y - 1;
	////	c = object.prefab->f[i]->c.y - 1;
	////	copy(object.prefab->t[a], A->tv);
	////	copy(object.prefab->t[b], B->tv);
	////	copy(object.prefab->t[c], C->tv);
	//ab.x = B->uv->x - A->uv->x;
	//ab.y = B->uv->y - A->uv->y;
	//ab.z = B->zValue - A->zValue;
	//bc.x = C->uv->x - B->uv->x;
	//bc.y = C->uv->y - B->uv->y;
	//bc.z = C->zValue - B->zValue;
	//crossV3(normal, ab, bc);
	//normalized(normal); 
	//A->color->x = (float)i / 8; A->color->y = (float)i / 8; A->color->z = (float)i / 8;
	//B->color->x = (float)i / 8; B->color->y = (float)i / 8; B->color->z = (float)i / 8;
	//C->color->x = (float)i / 8; C->color->y = (float)i / 8; C->color->z = (float)i / 8;
	//DrawTriangle(A, B, C);
}