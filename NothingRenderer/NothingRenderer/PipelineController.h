#pragma once
#include"ObjectInfo.h"
#include"camera.h"
#include"Vector.h"
#include"Buffer.h"
#include<thread>
#include<mutex>	
#include"BMPManager.h"
#include<algorithm>
#include"DirectionLight.h"
using namespace std;
 
using namespace VECTOR; 
struct DrawTrianglePoint {
	float x, y ; 
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
	ObjectHolder m_holder;
	BMPManager m_BMPManager;
	float m_width, m_height;
	float m_lightIntense;
	float m_lightDiffuse;
	mutex m_locker;
	MatrixStore store;
public :
	Camera m_cam;
	int done = 0; 
	DirectionLight m_directionLight; 
	bool isShadowMapGen = false;
	bool useTexture,showShadowMap = false;
	VECTOR4 lightDir;
	PipelineController();
	void CreateBuffer(int w, int h) {
		m_buffer.SetWidthHeight(w, h); 
		m_buffer.CreateBuffer(&m_penCol); 
		m_cam.SetWidthHeight(w, h);
		m_directionLight.SetWidthHeight(w, h);
		m_width = w; m_height = h; 
	} 

	//get color , call from main thread to draw buffer with openGL
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
	void GetVertsWorldSpace(Object* obj, MatrixStore& store) { 
		GenerateTransformMatrix(obj->position , &store.loacl2WorldM); 
		for (int i = 0; i < obj->prefab->vertCount; i++) {
			matrixdot(obj->verts[i].position, obj->prefab->v[i], &store.local2WorldR); 
			matrixdot(obj->verts[i].position, obj->verts[i].position,&store.loacl2WorldM);  
		}
	}  
	void GetAffineWeight(VECTOR4* A , VECTOR4 * B ,  VECTOR4* C,  int Px ,int Py , float pw, VECTOR3* res) {
		float a, b, c;
		float x1, y1, x2, y2, x3, y3;
		x1 = A->x /A->w;
		y1 = A->y /A->w;
		x2 = B->x /B->w;
		y2 = B->y /B->w;
		x3 = C->x /C->w;
		y3 = C->y /C->w;
		float px = Px  /pw;
		float py = Py /pw;   
		c = ((y1 - y2) * px + (x2 - x1) * py + x1 * y2 - x2 * y1) /  ((y1 - y2) * x3 + (x2 - x1) * y3 + x1 * y2 - x2 * y1)  ; 
		b = ((y1 - y3) * px + (x3 - x1) * py + x1 * y3 - x3 * y1) /  ((y1 - y3) * x2 + (x3 - x1) * y2 + x1 * y3 - x3 * y1) ; 
		a = 1 - b - c;    
		res->x = a;
		res->y = b;
		res->z = c;  
	} 
	float GetWValue(VECTOR4* A  ,VECTOR4 * B,    VECTOR4* C ,int Px,int  Py) {
		float a, b, c;
		float  x1, y1, x2, y2, x3, y3;
		x1 = A->x  ;
		y1 = A->y  ;
		x2 = B->x  ;
		y2 = B->y ;
		x3 = C->x  ;
		y3 = C->y  ; 
		c = ((y1 - y2) * Px + (x2 - x1) * Py + x1 * y2 - x2 * y1) /  ((y1 - y2) * x3 + (x2 - x1) * y3 + x1 * y2 - x2 * y1);
		b = ((y1 - y3) * Px + (x3 - x1) * Py + x1 * y3 - x3 * y1) /  ((y1 - y3) * x2 + (x3 - x1) * y2 + x1 * y3 - x3 * y1);
		a = 1 - b - c;
		 /*
		a = abs((Px*(y2 - y3) + x2*(y3 - Py) + x3 *(Py - y2)) / 2);
		b = abs((x1*(Py - y3) + Px*(y3 - y1) + x3 *(y1 - Py)) / 2);
		c = abs((x1*(y2 - Py) + x2*(Py - y1) + Px *(y1 - y2)) / 2);
		float s = a + b + c;
		a = a / s; 
		b = b / s; 
		c = c / s;*/

		return A->w * a + B->w * b + C->w * c;
	}
	float GetZValue(VECTOR4* A, VECTOR4 * B, VECTOR4* C, int Px, int  Py) {
		float a, b, c;
		float  x1, y1, x2, y2, x3, y3;
		x1 = A->x;
		y1 = A->y;
		x2 = B->x;
		y2 = B->y;
		x3 = C->x;
		y3 = C->y;
		c = ((y1 - y2) * Px + (x2 - x1) * Py + x1 * y2 - x2 * y1) / ((y1 - y2) * x3 + (x2 - x1) * y3 + x1 * y2 - x2 * y1);
		b = ((y1 - y3) * Px + (x3 - x1) * Py + x1 * y3 - x3 * y1) / ((y1 - y3) * x2 + (x3 - x1) * y2 + x1 * y3 - x3 * y1);
		a = 1 - b - c; 

		return A->z * a + B->z * b + C->z * c;
	}
	bool IsInsideTriangle(VECTOR4* A, VECTOR4 * B, VECTOR4* C, int Px, int  Py) {
		float a, b, c;
		float  x1, y1, x2, y2, x3, y3;
		x1 = A->x;
		y1 = A->y;
		x2 = B->x;
		y2 = B->y;
		x3 = C->x;
		y3 = C->y;

		c = ((y1 - y2) * Px + (x2 - x1) * Py + x1 * y2 - x2 * y1) / ((y1 - y2) * x3 + (x2 - x1) * y3 + x1 * y2 - x2 * y1);
		b = ((y1 - y3) * Px + (x3 - x1) * Py + x1 * y3 - x3 * y1) / ((y1 - y3) * x2 + (x3 - x1) * y2 + x1 * y3 - x3 * y1);
		a = 1 - b - c;
		if (a < 0 || a> 1 || b < 0 || b > 1 || c< 0 || c > 1) {
			return false;
		}
		return true;
	}
	int GetOrder(VShaderInfo i, VERT&  A) {
		if (i.order[0] == A.vertNum)
			return 0;
		else if (i.order[1] == A.vertNum)
			return 1;
		else
			return 2;
	}
	void VertexShader(VERT& A ,VShaderInfo & info  ) {
		int order = GetOrder(info, A);
		VECTOR4 viewDir;
		VECTOR::minusV4(&viewDir, A.position, m_cam.position);
		normalizedVector3(A.normal, A.normal);
/*
		normalizedVector3(&viewDir, &viewDir);
		float amb = dot3x3(&lightDir, A.normal);
		VECTOR4 r;
		VECTOR::dot(&r,A.normal, 2 *amb); 
		minusV3(&r, &r,& lightDir);
		normalizedVector3(&r,&r);
		float  sth = dot3x3(&r, &viewDir);
		sth = sth *0.5 + 0.5; 
		sth = sth * sth *(sth * 0.25);
		amb = amb * 0.5 + 0.5; 
		amb *= m_lightDiffuse;
		info.lightIntense [order] = (sth + amb)	 * m_lightIntense;  
*/
		copy (A.normal, &(info.normal[order] ));
		copy(A.position, &(info.pos[order]));

	}  
	void Drawpoint(VShaderInfo& info, int x, int y) { 
		VECTOR4 col;
		if (!ValidScreamPos(x, y)) {
			return;
		}
		float depth;
		depth = GetWValue(info.A->position, info.B->position, info.C->position, x,  y);
		depth = 1 / depth;
		if (depth > m_buffer.GetZBufferValue(x ,   y) || depth < 0) {
			return;
		}
		if (FragmentProcessShader(x ,  y, info.A, info.B, info.C, info, &col)) { 
			m_buffer.WriteToColorBuffer(x, y, &col);
			m_buffer.WriteToZBuffer(depth, x, y);
		} 
	 }
	void DrawpointS(VShaderInfo& info, int x, int y) {
		VECTOR4 col;
		if (!ValidScreamPos(x, y)) { 
			if (!ValidScreamPos(x, y)) {
				return;
			}
			if (!IsInsideTriangle(info.A->position, info.B->position, info.C->position, x, y))
				return;
			float depth = GetZValue(info.A->position, info.B->position, info.C->position, x, y);
			if (depth > m_directionLight.ReadShadowMap(x, y) || depth < 0) {
				return;
			}
			m_directionLight.WriteToShadowMap(depth, x, y);


			return;
		} 
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
	void DrawLine(DrawTrianglePoint& a, DrawTrianglePoint& b, DrawTrianglePoint& c, VShaderInfo& info ) {  
	}
	bool FragmentProcessShader(int x,int y, VERT* a, VERT*  b, VERT* c, VShaderInfo& info, VECTOR4* outColor) {
		VECTOR3 res;
		
		float u, v;
		float VlightAdjust; 
		float pw = GetWValue(a->position, b->position, c->position, x, y);
		GetAffineWeight(a->position,b->position, c->position, x , y, pw, &res);
		if (res.x < 0 || res.x > 1 || res.y < 0 || res.y > 1 || res.z < 0 || res.z > 1)
			return false; 
		int ordera = GetOrder(info, *a), orderb = GetOrder(info, *b), orderc = GetOrder(info, *c);
		u = a->tv->x * res.x + b->tv->x * res.y + c->tv->x * res.z;
		v = a->tv->y * res.x + b->tv->y * res.y + c->tv->y * res.z;


		VlightAdjust = res.x *info.lightIntense[ordera] + res.y * info.lightIntense[orderb]
			+ res.z *info.lightIntense[orderc];
		
		VECTOR4 fragPosA, fragPosB, fragPosC;
		dot(&fragPosA, &info.pos[ordera], res.x);
		dot(&fragPosB, &info.pos[orderb], res.y);
		dot(&fragPosC, &info.pos[orderc], res.z);
		add(&fragPosA, &fragPosA, &fragPosB);
		add(&fragPosA, &fragPosA, &fragPosC);


		VECTOR4 normalA, normalB, normlaC;
		dot(&normalA, &info.normal[ordera], res.x);
		dot(&normalB, &info.normal[orderb], res.y);
		dot(&normlaC, &info.normal[orderc], res.z);
		add(&normalA, &normalA, &normalB);
		add(&normalA, &normalA, &normlaC);

		VECTOR4 viewDir;
		minusV4(&viewDir, m_cam.position, &fragPosA);

		normalizedVector3(&viewDir, &viewDir);
		normalizedVector3(&normalA, &normalA); 

		float amb = dot3x3(&m_directionLight.lightDir, &normalA);
		VECTOR4 r;
		VECTOR::dot(&r, &normalA, 2 * amb);
		minusV3(&r, &r, &m_directionLight.lightDir);
		normalizedVector3(&r, &r);
		float  sth = dot3x3(&r, &viewDir);
		sth = sth *0.5 + 0.5;
		sth = sth * sth *(sth * 0.25);
		amb = amb * 0.5 + 0.5;
		amb *= m_lightDiffuse;
		VlightAdjust = (sth + amb) * m_lightIntense;

		outColor->x = 1; outColor->y = 1; outColor->z = 1;
		if (useTexture)
			copy(m_BMPManager.GetBMPColor(u, v), outColor); 
		dot(outColor, outColor, VlightAdjust); 

		m_buffer.WriteToColorBuffer(x, y, outColor);
		return true;
	}  
	void ShowShadowMap() {
		VECTOR4 col;
		float dis = m_directionLight.farZ - m_directionLight.nearZ;
		for (int i = 0; i < m_directionLight.width; i++) {
			for (int j = 0; j < m_directionLight.height -1; j++) {
				col.x = m_directionLight.ReadShadowMap(i, j) / (dis);
				col.y = col.x; col.z = col.x;
				m_buffer.WriteToColorBuffer(i, j,&col);
			} 
		} 
	}
	void RasterLineShadow(DrawTrianglePoint a, DrawTrianglePoint b, VShaderInfo info) { 
		float xl = a.x, xr = b.x; 
		if (xl == xr) { 
			return;
		}  
		if (xl > xr) {
			swap(xl, xr);
		} 
		xl -= 3; xr += 3;
		for (int i = 0; xl + i <= xr; i++) {
			if (!ValidScreamPos(xl + i, a.y)) {
				continue;
			} 
			if (!IsInsideTriangle(info.A->position, info.B->position, info.C->position, xl + i, a.y))
				continue;
			float depth = GetZValue(info.A->position, info.B->position, info.C->position, xl + i, a.y);
			if (depth > m_directionLight.ReadShadowMap(xl + i, a.y) || depth < 0) {
				continue;
			}  
			m_directionLight.WriteToShadowMap(depth,xl + i, a.y); 
		} 
	}
	void RasterLine(DrawTrianglePoint a,DrawTrianglePoint b,VShaderInfo info) {  
		float xl = a.x , xr = b.x  ;   
		if (xl == xr) { 
			return;
		}

		VECTOR4 col(1, 1, 1, 0);  

		if (xl > xr) {
			swap(xl, xr);  
		}    

		xl = xl - 3; xr = xr + 3;

		for (int i = 0; xl + i <= xr ; i++) {
			if (!ValidScreamPos(xl + i, a.y)) {  
				continue;
			} 
			float depth;
			depth = GetWValue(info.A->position, info.B->position,info.C->position, xl + i, a.y);
			depth = 1 / depth;
			if (depth > m_buffer.GetZBufferValue(xl + i, a.y) || depth < 0) {
				continue;
			}   
			if (FragmentProcessShader(xl + i, a.y, info.A, info.B, info.C, info, &col)) {

				m_buffer.WriteToColorBuffer(xl + i, a.y, &col);
				m_buffer.WriteToZBuffer(depth, xl + i, a.y);
			}
		} 
	}   
	void RasterizationTriangle(VShaderInfo& info) {
		VERT* A = info.A, *B = info.B, *C = info.C;
		int yDir = 1;
		if (B->position->y < A->position->y) {
			VERT* temp = A;A = B; B = temp;
		}
		if (C->position->y < A->position->y) {
			VERT* temp = A; A = C; C = temp; 
		}
		if (C->position->y < B->position->y) {
			VERT* temp = B; B =C; C = temp;
		}
		if ((int)B->position->y == (int)A->position->y) {
			VERT* temp = A; A = C;C = temp; 
		} 
		if (A->position->y > B->position->y)
			yDir = -1;  

		float disAB = distance2D(*(A->position), *(B->position));
		
		float disAC = distance2D(*(A->position), *(C->position));


		VECTOR2 dir;
		dir.x = B->position->x - A->position->x;
		dir.y = B->position->y - A->position->y;
		normalized(dir); 
		DrawTrianglePoint ab; ab.dir.x = dir.x, ab.dir.y = dir.y;
		ab.x = A->position->x; ab.y = A->position->y; 
		ab.dis = disAB; ab.count = 0; 


		dir.x = C->position->x - A->position->x;
		dir.y = C->position->y - A->position->y;
		normalized(dir); ab.FROM = A; ab.TO = B;
		DrawTrianglePoint ac; ac.dir.x = dir.x, ac.dir.y = dir.y;
		ac.x = A->position->x; ac.y = A->position->y; 
		ac.dis = disAC; ac.count = 0;
		ac.FROM = A; ac.TO = C; 


		DrawTrianglePoint bc; 

		float disbc = distance2D(* (B->position), *(C->position));
		bc.count = 0; bc.dis = disbc;
		bc.x = C->position->x; bc.y = C->position->y;
		dir.x = B->position->x - C->position->x; dir.y = B->position->y - C->position->y;
		normalized(dir); bc.dir.x = dir.x; bc.dir.y = dir.y;
		bc.FROM = C; bc.TO = B; 

		int y = A->position->y; 
		if ((int) A->position->y == (int)B->position->y && (int)A->position->y == (int)C->position->y)
		{  
			float xmin = A->position->x, xmax = A->position->x;
			if (xmin > B->position->x) xmin = B->position->x; if (xmax < B->position->x) xmax = B->position->x;
			if (xmin > C->position->x) xmin = C->position->x; if (xmax < C->position->x) xmax = C->position->x;
			for (int i = xmin; i < xmax; i++) {
				if(!isShadowMapGen)
					Drawpoint(info, i, A->position->y);
				else 
					DrawpointS(info, i, A->position->y);
			}
			return;
		} 
		if (ab.count < 0 || ac.count <0) 
			return;
		disAB += 5; disAC += 5; disbc += 5;

		for (; ab.count < disAB && ac.count < disAC;) {  

			while ((int)ab.y != y )
			{
				ab.count++;	ab.x += ab.dir.x; ab.y += ab.dir.y;
				if (!isShadowMapGen)
					Drawpoint(info,ab.x, ab.y);
				else
					DrawpointS(info, ab.x, ab.y);

				if (ab.count > disAB)
					break;
			}
			while ((int)ac.y != y)
			{
				ac.count++;  ac.Move();
				if (!isShadowMapGen)
					Drawpoint(info, ac.x, ac.y);
				else
					DrawpointS(info, ac.x, ac.y);
				
				if (ac.count > disAC)
					break; 
			} 
			if (ab.count >= disAB || ac.count >= disAC)
				break; 
			if(!isShadowMapGen)
				RasterLine(ab, ac, info); 
			else
				RasterLineShadow(ab, ac, info); 
			y += yDir;
		}
		if (ab.count >= disAB && ac.count >= disAC)
			return;
		if (ab.count >= disAB) { 
			bc.x = B->position->x; bc.y = B->position->y;
			dir.x = C->position->x - B->position->x; dir.y = C->position->y - B->position->y;
			normalized(dir); bc.dir.x = dir.x; bc.dir.y = dir.y;
			bc.FROM = B; bc.TO = C;
			ab.count = ac.count;	Swap(ab, ac); disAB = disAC;
		}  

		if (B->position->y == C->position->y) {
			return;
		}

		for (; ab.count < disAB && bc.count < disbc;) {
			if(!isShadowMapGen)
				RasterLine(ab, bc, info);
			else
				RasterLineShadow(ab, bc, info);
			while ((int)ab.y != y)
			{
				ab.count++;	ab.Move();
				if (!isShadowMapGen)
					Drawpoint(info, ab.x, ab.y);
				else
					DrawpointS(info, ab.x, ab.y);

				if (ab.count >= disAB)
					break;
			}
			while ((int)bc.y != y)
			{
				bc.count++;  bc.Move();
				if (!isShadowMapGen)
					Drawpoint(info, bc.x, bc.y);
				else
					DrawpointS(info, bc.x, bc.y);

				if (bc.count >= disAB)
					break;
			}
			if (!isShadowMapGen)
				RasterLine(ab, bc, info);
			else
				RasterLineShadow(ab, bc, info);
			y++;
		}  

	}
	void ThreadDone() {
		m_locker.lock(); 
		done++;
		m_locker.unlock();
	}
	void RenderShadowMap(Object& object);
};  


void MultiFaceDraw(Object & object , int from ,int to,VECTOR4* viewDir,PipelineController* pipeline,VShaderInfo * vInfo) {
	
	VECTOR3 ab, bc, normal;
	int a, b, c; 
	VERT* A, *B, *C; 
	for (int i = from; i < to; i++) { 
		a = object.prefab->f[i]->a.x - 1;
		b = object.prefab->f[i]->b.x - 1;
		c = object.prefab->f[i]->c.x - 1;
		A = &(object.verts[a]); B = &(object.verts[b]); C = &(object.verts[c]);
		ab.x = B->position->x - A->position->x;
		ab.y = B->position->y - A->position->y;
		ab.z = B->position->z - A->position->z;
		bc.x = C->position->x - B->position->x;
		bc.y = C->position->y - B->position->y;
		bc.z = C->position->z - B->position->z;
		crossV3(normal, ab, bc);
		normalized(normal);
		viewDir->x = 0; viewDir->y = 0; viewDir->z = -1;
		if (dotV3(normal, *viewDir) < 0 && !pipeline->isShadowMapGen)
			continue;
		if (!pipeline->ValidScreamPos(A->position->x, A->position->y)) {
			if (!pipeline->ValidScreamPos(B->position->x, B->position->y)) {
				if (!pipeline->ValidScreamPos(C->position->x, C->position->y)) {
					continue;
				}
			} 
		}
		if (A->position->z < 0 && B->position->z < 0 && C->position->z < 0) {
			continue;
		} 
		vInfo[i].A = A, vInfo[i].B = B, vInfo[i].C = C;
		pipeline->RasterizationTriangle(vInfo[i]);
	} 
	pipeline->ThreadDone(); 
}
 
void PipelineController::RenderAll() {
	m_buffer.ClearColBuffer(m_penCol); 
	isShadowMapGen = true;
	for (int i = 0; i < targetLength; i++) { 
		RenderShadowMap(*m_renderTargets[i]);
	}

	isShadowMapGen = false;  

	for (int i = 0; i < targetLength; i++) {
		if (m_renderTargets[i]->prefab->name == "cube")
			useTexture = true;
		else
			useTexture = false;
		RenderTarget(*m_renderTargets[i]);
	}
	if (showShadowMap) {
		ShowShadowMap();
	  } 
	m_buffer.SwapBuffer();
	m_directionLight.ClearShadowMap();
}
Object* PipelineController::GetObject(string name) { 
	for (int i = 0; i < targetLength; i++) {
		if (m_renderTargets[i]->objectName == name) {
			return m_renderTargets[i];
		}
	}
	return NULL;
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
	VECTOR4 col(51/255.0, 230/255.0,204/255.0, 1);
	copy(&col, &m_penCol);
	lightDir.x = 0; lightDir.y = 0; lightDir.z = -1; lightDir.w = 0;
	normalizedVector3(&lightDir, &lightDir);
	m_lightIntense = 1.5;
	m_lightDiffuse = 0.6; 
}

bool PipelineController::RemoveRenderTarget(Object ob) {
	return false;
}
void PipelineController::RenderShadowMap(Object& object) {
	VECTOR4 viewDir(0, 0, 0, 0);
	int a, b, c; 
	VECTOR2 va, vb, vc;
	GenerateRotateMatrix(*object.rotation, &m_directionLight.store.local2WorldR);
	GenerateRotateMatrix(m_directionLight.rotation, &m_directionLight.store.W2CamR);
	GenerateTransformMatrix(object.position, &m_directionLight.store.loacl2WorldM); 
	VShaderInfo* vInfo = new VShaderInfo[object.prefab->faceCount];
	for (int i = 0; i < object.prefab->faceCount; i++) {
		a = object.prefab->f[i]->a.x - 1;
		b = object.prefab->f[i]->b.x - 1;
		c = object.prefab->f[i]->c.x - 1; 
		VERT& AA = object.verts[a], &BB = object.verts[b], &CC = object.verts[c];
		AA.vertNum = a; BB.vertNum = b; CC.vertNum = c; 
		vInfo[i].order[0] = AA.vertNum;
		vInfo[i].order[1] = BB.vertNum;
		vInfo[i].order[2] = CC.vertNum; 
	}  
	GetVertsWorldSpace(&object, ref(m_directionLight.store));
	m_directionLight.GetLightCoordinateTransformVert(&object);
	m_directionLight.GenerateOrthoMatrix(m_directionLight.store.C2OthroM); 
	m_directionLight.GetClipSpaceTransfromVert(&object );
	minusV3(&viewDir, m_cam.position, object.position);
	int i = 0; 
	int threadCount = 5;
	int threadMissionCount = object.prefab->faceCount / threadCount;
	int startCount = 0, endCount = threadMissionCount; 
	
	thread t1(MultiFaceDraw, ref(object), startCount, endCount, &viewDir, this, vInfo);
	startCount = endCount; endCount += threadMissionCount;
	thread t2(MultiFaceDraw, ref(object), startCount, endCount, &viewDir, this, vInfo);
	startCount = endCount; endCount += threadMissionCount;
	thread t3(MultiFaceDraw, ref(object), startCount, endCount, &viewDir, this, vInfo);
	startCount = endCount; endCount += threadMissionCount;
	thread t4(MultiFaceDraw, ref(object), startCount, endCount, &viewDir, this, vInfo);
	startCount = endCount; endCount += threadMissionCount;
	thread t5(MultiFaceDraw, ref(object), startCount, object.prefab->faceCount, &viewDir, this, vInfo);
	t1.join(); t2.join(); t3.join(); t4.join();
	t5.join(); 
	while (done != 5)
	{
	}
	done = 0;
	delete[]vInfo;
}
void PipelineController::RenderTarget(Object &  object) { 
	VECTOR4 viewDir(0, 0, 0, 0);
	int a, b, c;
	int na, nb, nc;
	VECTOR2 va, vb, vc; 
	GenerateRotateMatrix(*object.rotation,  &store.local2WorldR);
	GenerateRotateMatrix(m_cam.rotation, &store.W2CamR);
	GenerateTransformMatrix(object.position, &store.loacl2WorldM); 
	VShaderInfo* vInfo = new VShaderInfo[object.prefab->faceCount];
	GetVertsWorldSpace(&object, ref(store));
	for (int i = 0; i < object.prefab->faceCount; i++) { 
		a = object.prefab->f[i]->a.x - 1;
		b = object.prefab->f[i]->b.x - 1;
		c = object.prefab->f[i]->c.x - 1;
		na = object.prefab->f[i]->a.z - 1;
		nb = object.prefab->f[i]->b.z - 1;
		nc = object.prefab->f[i]->c.z - 1;
		VERT& AA = object.verts[a], &BB = object.verts[b], &CC = object.verts[c]; 
		AA.vertNum = a; BB.vertNum = b; CC.vertNum = c;
		copy(object.prefab->n[na], AA.normal);
		copy(object.prefab->n[nb], BB.normal);
		copy(object.prefab->n[nc], CC.normal);  
		matrixdot(AA.normal, AA.normal, &store.local2WorldR);
		matrixdot(BB.normal, BB.normal, &store.local2WorldR);
		matrixdot(CC.normal, CC.normal, &store.local2WorldR);
		vInfo[i].order[0] = AA.vertNum;
		vInfo[i].order[1] = BB.vertNum;
		vInfo[i].order[2] = CC.vertNum;
		VertexShader( AA,vInfo[i]);
		VertexShader( BB,vInfo[i]);
		VertexShader( CC,vInfo[i]);
	}
	m_cam.GetCamCoordinateTransformVert(&object, store); 
	m_cam.GetClipSpaceTransfromVert(&object,store); 
	VECTOR3 ab, bc, normal;
	VERT* A, *B, *C;
	int i = 0;
	minusV3(&viewDir, m_cam.position, object.position);  
	for (; i < object.prefab->faceCount; i++) {
		a = object.prefab->f[i]->a.x - 1;
		b = object.prefab->f[i]->b.x - 1;
		c = object.prefab->f[i]->c.x - 1;
		A = &(object.verts[a]); B = &(object.verts[b]); C = &(object.verts[c]);
		a = object.prefab->f[i]->a.y - 1;
		b = object.prefab->f[i]->b.y - 1;
		c = object.prefab->f[i]->c.y - 1;
		if (useTexture) {
			copy(object.prefab->t[a], A->tv);
			copy(object.prefab->t[b], B->tv);
			copy(object.prefab->t[c], C->tv); 
		}
	} 
	 
	int threadCount = 5;
	int threadMissionCount = object.prefab->faceCount / threadCount;
	int startCount = 0,endCount = threadMissionCount;
	
	thread t1(MultiFaceDraw, ref(object), startCount, endCount, &viewDir, this,vInfo);
	startCount = endCount; endCount += threadMissionCount;
	thread t2(MultiFaceDraw, ref(object), startCount, endCount, &viewDir, this, vInfo);
	startCount = endCount; endCount += threadMissionCount;
	thread t3(MultiFaceDraw, ref(object), startCount, endCount, &viewDir, this, vInfo);
	startCount = endCount; endCount += threadMissionCount;
	thread t4(MultiFaceDraw, ref(object), startCount, endCount, &viewDir, this, vInfo);
	startCount = endCount; endCount += threadMissionCount;
	thread t5(MultiFaceDraw, ref(object), startCount, object.prefab->faceCount, &viewDir, this, vInfo);
	t1.join(); t2.join(); t3.join(); t4.join();
	t5.join(); 
	while (done != 5)
	{ 
	}
	done = 0;     
	//i = 10;
	//for (; i < object.prefab->faceCount; i++) {
	//	VERT* A, *B, *C;
	//	a = object.prefab->f[i]->a.x - 1;
	//	b = object.prefab->f[i]->b.x - 1;
	//	c = object.prefab->f[i]->c.x - 1;
	//	A = &(object.verts[a]); B = &(object.verts[b]); C = &(object.verts[c]);
	//	a = object.prefab->f[i]->a.y - 1;
	//	b = object.prefab->f[i]->b.y - 1;
	//	c = object.prefab->f[i]->c.y - 1;
	//	copy(object.prefab->t[a], A->tv);
	//	copy(object.prefab->t[b], B->tv);
	//	copy(object.prefab->t[c], C->tv);
	//	ab.x = B->position->x - A->position->x;
	//	ab.y = B->position->y - A->position->y;
	//	ab.z = B->zValue - A->zValue;
	//	bc.x = C->position->x - B->position->x;
	//	bc.y = C->position->y - B->position->y;
	//	bc.z = C->zValue - B->zValue;
	//	crossV3(normal, ab, bc);
	//	normalized(normal);
	//	//	if (VECTOR::dotV3(normal, viewDir) < 0)
	//	//		continue;    
	//	vInfo[i].A = A, vInfo[i].B = B, vInfo[i].C = C;
	//	RasterizationTriangle(vInfo[i]);
	//}

	 
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
	//ab.x = B->position->x - A->position->x;
	//ab.y = B->position->y - A->position->y;
	//ab.z = B->zValue - A->zValue;
	//bc.x = C->position->x - B->position->x;
	//bc.y = C->position->y - B->position->y;
	//bc.z = C->zValue - B->zValue;
	//crossV3(normal, ab, bc);
	//normalized(normal); 
	//A->color->x = (float)i / 8; A->color->y = (float)i / 8; A->color->z = (float)i / 8;
	//B->color->x = (float)i / 8; B->color->y = (float)i / 8; B->color->z = (float)i / 8;
	//C->color->x = (float)i / 8; C->color->y = (float)i / 8; C->color->z = (float)i / 8;
	//RasterizationTriangle(A, B, C);
	delete[]vInfo;
}