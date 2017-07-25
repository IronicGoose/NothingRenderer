#pragma once
#include"ObjectInfo.h"
#include"camera.h"
#include"Vector.h"
#include"Buffer.h"
#include<thread>
#include"BMPManager.h"
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
	BMPManager m_BMPManager;
	float m_width, m_height;
	float m_lightIntense;
	
public : 
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
		for (int i = 0; i < h - 1; i++) {
			for (int j = 0; j < w - 1; j++) {
				m_buffer.WriteToColorBuffer(j, i, m_BMPManager.GetBMPColor(j/w, i/h));
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
		return abs((float)((x0 *y1 - x1 * y0)/ 2));
	}   
	void SampleColor(VECTOR4* A , VECTOR4 * B ,  VECTOR4* C,  float Px ,float Py , float Pz, VECTOR3* res) {
		float a, b, c, total;
		float ax, ay, bx, by, cx, cy;
		ax = A->x /A->w;
		ay = A->y /A->w;
		bx = B->x /B->w;
		by = B->y /B->w;
		cx = C->x /C->w;
		cy = C->y /C->w;
		Px = Px  /Pz;
		Py = Py /Pz;
		c = TriangleArea(ax - Px, ay - Py, bx - Px, by - Py);
		b = TriangleArea(ax - Px,ay - Py, cx - Px, cy - Py);
		a = TriangleArea(bx - Px, by - Py, cx - Px, cy - Py);
		total = a + b + c;
		a = a / total;
		b = b / total;
		c = c / total;
		res->x = a;
		res->y = b;
		res->z = c; 
	}
	float SampleDepth(VECTOR4* A  ,VECTOR4 * B,    VECTOR4* C ,int Px,int Py) {
		float a, b, c, total;
		c = TriangleArea(A->x - Px, A->y - Py, B->x - Px, B->y - Py);
		b = TriangleArea(A->x - Px, A->y - Py, C->x - Px, C->y - Py); 
		a = TriangleArea(B->x - Px, B->y - Py, C->x - Px, C->y - Py);
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
	struct DrawTrianglePoint {
		float x, y, tarX, tarY;
		VECTOR2 dir; 
		void Move() {
			x += dir.x; y += dir.y;
		}
	};
	void Swap(DrawTrianglePoint& A, DrawTrianglePoint& B) {
		DrawTrianglePoint& C = A;
		A = B;
		B = C;
	}
	void DrawPoint(DrawTrianglePoint a) {
		VECTOR4 col(1, 1, 1, 0);
		if (ValidScreamPos(a.x, a.y))
			m_buffer.WriteToColorBuffer(a.x, a.y, &col); 
	}
	void RasterLine(DrawTrianglePoint a,DrawTrianglePoint b) { 
		int xl = a.x, xr = b.x;
		VECTOR4 col(1, 1, 1, 0);
		if (xl > xr) swap(xl, xr);
		for (int i = 0; xl + i <= xr; i++) {
			if(ValidScreamPos(xl+i,a.y))
				m_buffer.WriteToColorBuffer(xl + i, a.y, &col);
		}
		
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
		if (A->uv->y == B->uv->y && A->uv->y == C->uv->y)
			return;
		VECTOR2 dir;
		dir.x = B->uv->x - A->uv->x;
		dir.y = B->uv->y - A->uv->y;
		normalized(dir); 
		DrawTrianglePoint ab; ab.dir.x = dir.x, ab.dir.y = dir.y;
		ab.x = A->uv->x; ab.y = A->uv->y;
		ab.tarX = B->uv->x; ab.tarY = B->uv->y;

		dir.x = C->uv->x - A->uv->x;
		dir.y = C->uv->y - A->uv->y;
		normalized(dir);

		DrawTrianglePoint ac; ac.dir.x = dir.x, ac.dir.y = dir.y;
		ac.x = A->uv->x; ac.y = A->uv->y;
		ac.tarX = C->uv->x; ac.tarY = C->uv->y;
		
		float disAB = distance2D(*(A->uv), *(B->uv)), disAC = distance2D(*(A->uv), *(C->uv));
		int countab = 0, countac = 0, y = A->uv->y;
		for (; countab < disAB && countac < disAC;) { 
			while ((int)ab.y != y )
			{
				countab++;	ab.x += ab.dir.x; ab.y += ab.dir.y; DrawPoint(ab);
				if (countab > disAB )
					break;
			}
			while ((int)ac.y != y)
			{
				countac++;  ac.Move(); DrawPoint(ac);
				if (countac > disAC )
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
		if (countac >= disAC) {
			la.x = C->uv->x; la.y = C->uv->y;
			dir.x = B->uv->x - C->uv->x; dir.y = B->uv->y - C->uv->y;
			normalized(dir); la.dir.x = dir.x; la.dir.y = dir.y; 
		}
		if (countab >= disAB) { 
			la.x = B->uv->x; la.y = B->uv->y;
			dir.x = C->uv->x - B->uv->x; dir.y = C->uv->y - B->uv->y;
			normalized(dir); la.dir.x = dir.x; la.dir.y = dir.y;

			countab = countac;	Swap(ab, ac); disAB = disAC;
		}
		int countbc = 0;
		cout << y << " " << ab.y << " " << la.y << endl;
		for (; countab < disAB && countbc < disbc;) {  
			while ((int)ab.y != y)
			{
				countab++;	ab.Move(); DrawPoint(ab);
			}
			while ((int)la.y != y)
			{
				countbc++;  la.Move(); DrawPoint(la);
			}
			RasterLine(ab, la); 
			y++;
		}


	}

};  
void PipelineController::RenderAll() {
	m_buffer.ClearColBuffer(m_penCol);
	
	for (int i = 0; i < targetLength; i++) {
		RenderTarget(*m_renderTargets[i]);
	}
	ShowBMPPicture();
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
	m_cam.GetCamCoordinateTransformVert(&object);  /*
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
	}  */
	m_cam.GetClipSpaceTransfromVert(&object); 
	VECTOR2 va, vb, vc;
	VECTOR4 col(0.1, 0.1, 0.1, 1); 
	int i = 0 ;  
	VECTOR3 ab, bc, normal;
	minusV3(&viewDir, m_cam.position, object.position);
	for (; i < object.prefab->faceCount; i++) {
		VERT* A,*B, *C;
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
		if (dotV3(normal, viewDir) < 0)
			continue;    
		DrawTriangle(A, B, C);
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
	m_threads = 5;
} 

bool PipelineController::RemoveRenderTarget(Object ob) { 
	return false;
} 