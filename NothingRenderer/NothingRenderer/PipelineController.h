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
	int drawx, drawy;
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
	float m_lightDiffuse;
	mutex m_locker;
public :
	int done = 0;
	int m_threads;
	bool useVShader = true,useFShader = false,useTexture;
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
	Object * GetObject(string name );/*
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

	}*/
	void GetVertsWorldSpace(Object* obj, MATRIX4x4* rotateMatrix, bool normalConvert = false) {
		MATRIX4x4* ma = new MATRIX4x4();
		GenerateTransformMatrix(obj->position, ma); 
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
		}
	} 
	float TriangleArea(float x0, float y0, float x1, float y1) {
		return abs( ( x0 *y1 -  x1 * y0)/ 2.0f);
	}  
	float TriangleArea(float x0, float y0, float x1, float y1,float x2 , float y2) {
		return abs(  (x0 * (y1 - y2) + x1 * (y2 - y0) + x2* (y0 - y1))/2.0 );
	}
	void GetAffineWeight(VECTOR4* A , VECTOR4 * B ,  VECTOR4* C,  float Px ,float Py , float pw, VECTOR3* res) {
		float a, b, c;
		float x1, y1, x2, y2, x3, y3;
		x1 = (int)A->x /A->w;
		y1 = (int)A->y /A->w;
		x2 = (int)B->x /B->w;
		y2 = (int)B->y /B->w;
		x3 = (int)C->x /C->w;
		y3 = (int)C->y /C->w;
		Px = Px  /pw;
		Py = Py /pw;  
		
		
		c = ((y1 - y2) * Px + (x2 - x1) * Py + x1 * y2 - x2 * y1) /(float) ((y1 - y2) * x3 + (x2 - x1) * y3 + x1 * y2 - x2 * y1)  ; 
		b = ((y1 - y3) * Px + (x3 - x1) * Py + x1 * y3 - x3 * y1) /(float) ((y1 - y3) * x2 + (x3 - x1) * y2 + x1 * y3 - x3 * y1) ; 
		a = 1 - b - c;   
/*
		a =((y2 - y3) * (Px - x3) + (x3 - x2) * (Py - y3)) / ((y2 - y3) * (x1 - x3) + (x3 - x2) * (y1 - y3));
		b =((y3 - y3) * (Px - x3) * (x1 - x3) * (Py - y3)) / ((y2 - y3) * (x1 - x3) + (x3 - x2) * (y1 - y3));
		c = 1 - a - b*/;
		res->x = a;
		res->y = b;
		res->z = c;
		if (a < 0 || a> 1 || b < 0 || b > 1 || c< 0 ||c > 1) { 
		}
		else { 
		}
	} 
	float GetWValue(VECTOR4* A  ,VECTOR4 * B,    VECTOR4* C ,float Px,float  Py) {
		float a, b, c, total;
		float  x1, y1, x2, y2, x3, y3;
		x1 = (int)A->x  ;
		y1 = (int)A->y  ;
		x2 = (int)B->x  ;
		y2 = (int)B->y ;
		x3 = (int)C->x  ;
		y3 = (int)C->y  ;
		Px = Px  ;
		Py = Py ;

		c = ((y1 - y2) * Px + (x2 - x1) * Py + x1 * y2 - x2 * y1) / (float)((y1 - y2) * x3 + (x2 - x1) * y3 + x1 * y2 - x2 * y1);
		b = ((y1 - y3) * Px + (x3 - x1) * Py + x1 * y3 - x3 * y1) / (float)((y1 - y3) * x2 + (x3 - x1) * y2 + x1 * y3 - x3 * y1);
		a = 1 - b - c;
		return A->w * a + B->w * b + C->w * c;
	}
	float GetZValue(VECTOR4* A, VECTOR4 * B, VECTOR4* C, float Px, float  Py) {
		float a, b, c, total;
		float  x1, y1, x2, y2, x3, y3;
		x1 = (int)A->x;
		y1 = (int)A->y;
		x2 = (int)B->x;
		y2 = (int)B->y;
		x3 = (int)C->x;
		y3 = (int)C->y;
		Px = Px;
		Py = Py;

		c = ((y1 - y2) * Px + (x2 - x1) * Py + x1 * y2 - x2 * y1) / (float)((y1 - y2) * x3 + (x2 - x1) * y3 + x1 * y2 - x2 * y1);
		b = ((y1 - y3) * Px + (x3 - x1) * Py + x1 * y3 - x3 * y1) / (float)((y1 - y3) * x2 + (x3 - x1) * y2 + x1 * y3 - x3 * y1);
		a = 1 - b - c;
		return A->z * a + B->z * b + C->z * c;
	}
	int GetOrder(VShaderInfo i, VERT&  A) {
		if (i.order[0] == A.vertNum)
			return 0;
		else if (i.order[1] == A.vertNum)
			return 1;
		else
			return 2;
	}
	void VertexShader(VERT& A ,VShaderInfo & info ) {
		int order = GetOrder(info, A);
		VECTOR4 viewDir;
		VECTOR::minusV4(&viewDir, A.position, m_cam.position);
		normalizedVector3(&viewDir, &viewDir);
		normalizedVector3(A.normal, A.normal);
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
		copy (A.normal, &(info.normal[order] ));
		copy(A.position, &(info.pos[order]));

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
	void DrawPoint(DrawTrianglePoint a,VShaderInfo& info) {
		if (!ValidScreamPos(a.x, a.y))
			return;
		VECTOR4 col(1, 1, 1, 0);
		float alpha = 1 - (float)a.count / a.dis;
		float depth = a.FROM->zValue * alpha + a.TO->zValue * (1 - alpha);
		if (depth > m_buffer.GetZBufferValue(a.x, a.y)) {
			m_buffer.WriteToZBuffer(depth, a.x, a.y);
		}
		else
			return; 
		float pw = (a.count / a.dis) * a.TO->position->w + (1-(a.count / a.dis) )* a.FROM->position->w;
		float xw = a.x / pw; float yw = a.y / pw;
		float fxw = a.FROM->position->x / a.FROM->position->w, txw = a.TO->position->x / a.TO->position->w;
		alpha = abs(fxw - xw) / abs(fxw - txw);
		int orderAFrom = GetOrder(info, *a.FROM), orderATO = GetOrder(info, *a.TO) ;
		float u = a.FROM->tv->x * alpha + a.TO->tv->x * (1 - alpha) ;
		float v = a.FROM->tv->y *alpha + a.TO->tv->y * (1 - alpha) ;
		float VlightAdjust = alpha *info.lightIntense[orderAFrom] + (1 - alpha) * info.lightIntense[orderATO]; 
		if (useVShader) {
			dot(&col, &col, VlightAdjust);
		}
		col.x = 1; col.y = 1; col.z = 1;
		//m_buffer.WriteToColorBuffer(a.x, a.y, &col);
		//m_buffer.WriteToColorBuffer(a.x + 1, a.y, &col);
		if (a.y + 1 >= m_height) {
			return;
		}
		//m_buffer.WriteToColorBuffer(a.x+1, a.y+1, &col);
		//m_buffer.WriteToColorBuffer(a.x, a.y+1, &col);

	}
	void DrawLine(DrawTrianglePoint& a, DrawTrianglePoint& b, DrawTrianglePoint& c, VShaderInfo& info ) {
		//for line ab 
		VECTOR4 col(0, 0, 0, 0);
		float u = a.x, v = a.y;
		float depth;
		for (int i = 0; i <= a.dis; i++) {
			u = a.x + i* a.dir.x; v = a.y + i* a.dir.y;
			if (!ValidScreamPos(u, v))
				continue;
			m_buffer.WriteToColorBuffer(u, v, &col);


			if (ValidScreamPos(u+1, v)) { 
				m_buffer.WriteToColorBuffer(u+1, v, &col);
			}
			if (ValidScreamPos(u , v +1)) {
				m_buffer.WriteToColorBuffer(u + 1, v, &col);
			}
			m_buffer.WriteToColorBuffer(u, v, &col);
/*
			depth = GetZValue(a.FROM->position, c.FROM->position, c.TO->position,u,v);
			if (depth > m_buffer.GetZBufferValue(u, v)) {
				m_buffer.WriteToZBuffer(depth, u,v);
			}
			else {
				continue;
			}*/

		//	FragmentProcessShader(u, v, a.FROM, c.FROM, c.TO, info, &col);   


		}

		u = b.x, v = b.y;
		for (int i = 0; i < b.dis; i++) {
			u = b.x + i* b.dir.x; v = b.y + i* b.dir.y;
			if (!ValidScreamPos(u, v))
				continue;

			if (ValidScreamPos(u + 1, v)) {
				m_buffer.WriteToColorBuffer(u + 1, v, &col);
			}
			if (ValidScreamPos(u, v + 1)) {
				m_buffer.WriteToColorBuffer(u + 1, v, &col);
			}

			m_buffer.WriteToColorBuffer(u, v, &col);
/*
			depth = GetZValue(a.FROM->position, c.FROM->position, c.TO->position, u, v);
			if (depth > m_buffer.GetZBufferValue(u, v)) {
				m_buffer.WriteToZBuffer(depth, u, v);
			}
			else {
				continue;
			}*/

		//	FragmentProcessShader(u, v, a.FROM, c.FROM, c.TO, info, &col); 
			 
		}



		u = c.x, v = c.y;
		for (int i = 0; i < c.dis; i++) {
			u = c.x + i* c.dir.x; v = c.y + i* c.dir.y;
			if (!ValidScreamPos(u, v))
				continue; 

			if (ValidScreamPos(u + 1, v)) {
				m_buffer.WriteToColorBuffer(u + 1, v, &col);
			}
			if (ValidScreamPos(u, v + 1)) {
				m_buffer.WriteToColorBuffer(u + 1, v, &col);
			}


			m_buffer.WriteToColorBuffer(u , v, &col);
/*
			depth = GetZValue(a.FROM->position, c.FROM->position, c.TO->position, u, v);
			if (depth > m_buffer.GetZBufferValue(u, v)) {
				m_buffer.WriteToZBuffer(depth, u, v);
			}
			else {
				continue;
			}
*/
			//FragmentProcessShader(u, v, a.FROM, c.FROM, c.TO, info, &col); 
			 
		}

	}
	bool FragmentProcessShader(int x, int y, VERT* a, VERT*  b, VERT* c, VShaderInfo& info, VECTOR4* outColor) {
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


		float amb = dot3x3(&lightDir, &normalA);
		VECTOR4 r;
		VECTOR::dot(&r, &normalA, 2 * amb);
		minusV3(&r, &r, &lightDir);
		normalizedVector3(&r, &r);
		float  sth = dot3x3(&r, &viewDir);
		sth = sth *0.5 + 0.5;
		sth = sth * sth *(sth * 0.25);
		amb = amb * 0.5 + 0.5;
		amb *= m_lightDiffuse;
		VlightAdjust = (sth + amb) * m_lightIntense;
		
		outColor->x = 1; outColor->y = 1; outColor->z = 1;
		
		dot(outColor, outColor, VlightAdjust);


		m_buffer.WriteToColorBuffer(x, y, outColor);
		return true;
	} 

	void RasterLine(DrawTrianglePoint a,DrawTrianglePoint b,VShaderInfo info) { 
		 
/*
		int dxl = a.x; int dxr = b.x;
		if (dxl > dxr)
			swap(dxl, dxr);
		for (int i = 0; dxl + i <= dxr; i ++) {
			m_buffer.WriteToColorBuffer(dxl + i, a.y, &gray);
		}
*/ 
		int xl = a.x , xr = b.x;  
		int errorPoint = 0, normalPoint = 0;
		if (xl == xr) {
			DrawPoint(a); 
			return;
		}
		VECTOR4 col(1, 1, 1, 0);
		float alpha = 1 - (float)a.count / a.dis;
		float beta = 1- (float)b.count / b.dis; 
		float dl = a.FROM->zValue * alpha + a.TO->zValue * (1- alpha),
			dr = b.FROM->zValue * beta + b.TO->zValue * (1- beta);  
		float gamma = 0,depth;
		bool lr = true;
		if (xl > xr) {
			swap(xl, xr); 
			lr = false;
		}   
		
		for (int i = 0; xl + i <= xr; i++) {
			if (!ValidScreamPos(xl + i, a.y)) {  
				continue;
			}
			if (lr)
				gamma = 1 - i / (float)(xr - xl);
			else
				gamma = (float)i / (float)(xr - xl);  
			float depth = dl * gamma + dr *  (1- gamma);
			if (depth > m_buffer.GetZBufferValue(xl + i, a.y)) { 
				m_buffer.WriteToZBuffer(depth, xl + i, a.y); 
			}
			else { 
				continue;
			}
			float u, v, VlightAdjust;
			VECTOR3 res;
			if (a.FROM == b.FROM) {
				float pw = GetWValue(a.FROM->position, a.TO->position, b.TO->position, xl + i, a.y);
				GetAffineWeight(a.FROM->position, a.TO->position, b.TO->position, xl + i, a.y, pw, &res);
				if (res.x < 0 || res.x > 1 || res.y < 0 || res.y > 1 || res.z < 0 || res.z > 1)
					continue;

				int orderAFrom = GetOrder(info, *a.FROM), orderATO = GetOrder(info, *a.TO), orderBTO = GetOrder(info, *b.TO);
				u = a.FROM->tv->x * res.x + a.TO->tv->x * res.y + b.TO->tv->x * res.z;
				v = a.FROM->tv->y * res.x + a.TO->tv->y * res.y + b.TO->tv->y * res.z;
				VlightAdjust = res.x *info.lightIntense[orderAFrom] + res.y * info.lightIntense[orderATO]
					+ res.z *info.lightIntense[orderBTO];




				VECTOR4 fragPosA, fragPosB, fragPosC;
				dot(&fragPosA, &info.pos[orderAFrom], res.x);
				dot(&fragPosB, &info.pos[orderATO], res.y);
				dot(&fragPosC, &info.pos[orderBTO], res.z);
				add(&fragPosA, &fragPosA, &fragPosB);
				add(&fragPosA, &fragPosA, &fragPosC);


				VECTOR4 normalA, normalB, normlaC;
				dot(&normalA, &info.normal[orderAFrom], res.x);
				dot(&normalB, &info.normal[orderATO], res.y);
				dot(&normlaC, &info.normal[orderBTO], res.z);
				add(&normalA, &normalA, &normalB);
				add(&normalA, &normalA, &normlaC);

				VECTOR4 viewDir;
				minusV4(&viewDir, m_cam.position, &fragPosA);

				normalizedVector3(&viewDir, &viewDir);
				normalizedVector3(&normalA, &normalA);


				float amb = dot3x3(&lightDir, &normalA);
				VECTOR4 r;
				VECTOR::dot(&r, &normalA, 2 * amb);
				minusV3(&r, &r, &lightDir);
				normalizedVector3(&r, &r);
				float  sth = dot3x3(&r, &viewDir);
				sth = sth *0.5 + 0.5;
				sth = sth * sth *(sth * 0.25);
				amb = amb * 0.5 + 0.5;
				amb *= m_lightDiffuse;
				VlightAdjust = (sth + amb) * m_lightIntense; 

			}
			else {

				float pw = GetWValue(a.FROM->position, a.TO->position, b.FROM->position, xl + i, a.y);
				GetAffineWeight(a.FROM->position, a.TO->position, b.FROM->position, xl + i, a.y, pw, &res);
				if (res.x < 0 || res.x > 1 || res.y < 0 || res.y > 1 || res.z < 0 || res.z > 1)
					continue;
				int orderAFrom = GetOrder(info, *a.FROM), orderATO = GetOrder(info, *a.TO), orderBFrom = GetOrder(info, *b.FROM);
				u = a.FROM->tv->x * res.x + a.TO->tv->x * res.y + b.FROM->tv->x * res.z;
				v = a.FROM->tv->y * res.x + a.TO->tv->y * res.y + b.FROM->tv->y * res.z;
				VlightAdjust = res.x *info.lightIntense[orderAFrom] + res.y * info.lightIntense[orderATO]
					+ res.z *info.lightIntense[orderBFrom];

				VECTOR4 fragPosA, fragPosB, fragPosC;
				dot(&fragPosA, &info.pos[orderAFrom], res.x);
				dot(&fragPosB, &info.pos[orderATO], res.y);
				dot(&fragPosC, &info.pos[orderBFrom], res.z);
				add(&fragPosA, &fragPosA, &fragPosB);
				add(&fragPosA, &fragPosA, &fragPosC);


				VECTOR4 normalA, normalB, normlaC;
				dot(&normalA, &info.normal[orderAFrom], res.x);
				dot(&normalB, &info.normal[orderATO], res.y);
				dot(&normlaC, &info.normal[orderBFrom], res.z);
				add(&normalA, &normalA, &normalB);
				add(&normalA, &normalA, &normlaC);

				VECTOR4 viewDir;
				minusV4(&viewDir, m_cam.position, &fragPosA);

				normalizedVector3(&viewDir, &viewDir);
				normalizedVector3(&normalA, &normalA);


				float amb = dot3x3(&lightDir, &normalA);
				VECTOR4 r;
				VECTOR::dot(&r, &normalA, 2 * amb);
				minusV3(&r, &r, &lightDir);
				normalizedVector3(&r, &r);
				float  sth = dot3x3(&r, &viewDir);
				sth = sth *0.5 + 0.5;
				sth = sth * sth *(sth * 0.25);
				amb = amb * 0.5 + 0.5;
				amb *= m_lightDiffuse;
				VlightAdjust = (sth + amb) * m_lightIntense;
			}
			normalPoint++;
			VECTOR4 col(1, 1, 1, 0);
			if(useTexture)
				copy(m_BMPManager.GetBMPColor(u, v), &col);
			if (useVShader) { 
				dot(&col, &col, VlightAdjust);
			}
		//	col.x = 0; col.y = 0; col.z = 0;
			m_buffer.WriteToColorBuffer(xl + i, a.y,&col);
		}
	//	cout << errorPoint << "  " << " normal point " << normalPoint << endl;
	}  
	void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, int x, int y) {
		VECTOR4 col;
		float a, b ,c ;
		c = ((y1 - y2) * x + (x2 - x1) * y + x1 * y2 - x2 * y1) / (float)((y1 - y2) * x3 + (x2 - x1) * y3 + x1 * y2 - x2 * y1);
		b = ((y1 - y3) * x + (x3 - x1) * y + x1 * y3 - x3 * y1) / (float)((y1 - y3) * x2 + (x3 - x1) * y2 + x1 * y3 - x3 * y1);
		a = 1 - b - c;
		
		if (a < 0 || a > 1 || b < 0 || b > 1 || c < 0 || c > 1)
			return;
		col.x = a; col.y = b; col.z = c;
		m_buffer.WriteToColorBuffer(x,y,&col);
	}
	void RasterizationTriangle(VShaderInfo& info) {
		VERT* A = info.A, *B = info.B, *C = info.C;
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

		DrawTrianglePoint bc;

		float disbc = distance2D(*(B->uv), *(C->uv));
		bc.count = 0; bc.dis = disbc;
		bc.x = C->uv->x; bc.y = C->uv->y;
		dir.x = B->uv->x - C->uv->x; dir.y = B->uv->y - C->uv->y;
		normalized(dir); bc.dir.x = dir.x; bc.dir.y = dir.y;
		bc.FROM = C; bc.TO = B;


		int y = A->uv->y;



		if (A->uv->y == B->uv->y && A->uv->y == C->uv->y)
		{ 
			return;
		}
		DrawLine(ab, ac,bc, info);
		for (; ab.count < disAB && ac.count < disAC;) {  

			while ((int)ab.y != y )
			{
				ab.count++;	ab.x += ab.dir.x; ab.y += ab.dir.y; DrawPoint(ab,info);  
			}
			while ((int)ac.y != y)
			{
				ac.count++;  ac.Move(); DrawPoint(ac, info);  
			} 
			if (ab.count >= disAB || ac.count >= disAC)
				break;
			if ((int)ab.y !=(int) ac.y)
				cout << "error occur in raster" << endl;
			RasterLine(ab, ac, info); 
			y += yDir;
		}
		if (B->uv->y == C->uv->y) {
			return;
		}
		if (ab.count >= disAB) { 
			bc.x = B->uv->x; bc.y = B->uv->y;
			dir.x = C->uv->x - B->uv->x; dir.y = C->uv->y - B->uv->y;
			normalized(dir); bc.dir.x = dir.x; bc.dir.y = dir.y;
			bc.FROM = B; bc.TO = C;
			ab.count = ac.count;	Swap(ab, ac); disAB = disAC;
		}  
		for (; ab.count < disAB && bc.count < disbc;) {
			RasterLine(ab, bc, info);
			while ((int)ab.y != y)
			{
				ab.count++;	ab.Move(); DrawPoint(ab, info); 
			}
			while ((int)bc.y != y)
			{
				bc.count++;  bc.Move(); DrawPoint(bc, info); 
			}
			RasterLine(ab, bc,info); 
			y++;
		}  
	}
	void ThreadDone() {
		m_locker.lock(); 
		done++;
		m_locker.unlock();
	}
};  


void MultiFaceDraw(Object & object , int from ,int to,VECTOR4* viewDir,PipelineController* pipeline,VShaderInfo * vInfo) {
	
	VECTOR3 ab, bc, normal;
	int a, b, c; 
	VERT* A, *B, *C;
	VECTOR2 va, vb, vc;
	for (int i = from; i < to; i++) { 
		a = object.prefab->f[i]->a.x - 1;
		b = object.prefab->f[i]->b.x - 1;
		c = object.prefab->f[i]->c.x - 1;
		A = &(object.verts[a]); B = &(object.verts[b]); C = &(object.verts[c]);
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
	//	A->color->x = (float)i / 8; A->color->y = (float)i / 8; A->color->z = (float)i / 8;
	//	B->color->x = (float)i / 8; B->color->y = (float)i / 8; B->color->z = (float)i / 8;
	//	C->color->x = (float)i / 8; C->color->y = (float)i / 8; C->color->z = (float)i / 8;
		vInfo[i].A = A, vInfo[i].B = B, vInfo[i].C = C;
		pipeline->RasterizationTriangle(vInfo[i]);
	} 
	pipeline->ThreadDone(); 
}


void PipelineController::RenderAll() {
	m_buffer.ClearColBuffer(m_penCol);
	 
	for (int i = 0; i < targetLength; i++) {
		if (m_renderTargets[i]->prefab->name == "cube")
			useTexture = true;
		else
			useTexture = false;
		RenderTarget(*m_renderTargets[i]);
	}
	ShowBMPPicture();
	for (int i = 0; i < 250; i++) {
		for (int j = 0; j < 2250; j++) {
			DrawTriangle(150, 30, 0, 0, 30, 180, i, j);
		}
	}
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
	m_lightDiffuse = 0.8;
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
	//cout << "term" << endl;
	VShaderInfo* vInfo = new VShaderInfo[object.prefab->faceCount];
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
		matrixdot(AA.normal, AA.normal, &rot);
		matrixdot(BB.normal, BB.normal, &rot);
		matrixdot(CC.normal, CC.normal, &rot);

		vInfo[i].order[0] = AA.vertNum;
		vInfo[i].order[1] = BB.vertNum;
		vInfo[i].order[2] = CC.vertNum;
		VertexShader( AA,vInfo[i]);
		VertexShader( BB,vInfo[i]);
		VertexShader( CC,vInfo[i]);
	}
	m_cam.GetClipSpaceTransfromVert(&object);
	VECTOR4 col(0.1, 0.1, 0.1, 1);
	VECTOR3 ab, bc, normal;
	minusV3(&viewDir, m_cam.position, object.position);  


	//7
	int i = 0;  
	for (; i < object.prefab->faceCount; i++) {
		VERT* A, *B, *C;
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

	//i = 0;
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
	//	ab.x = B->uv->x - A->uv->x;
	//	ab.y = B->uv->y - A->uv->y;
	//	ab.z = B->zValue - A->zValue;
	//	bc.x = C->uv->x - B->uv->x;
	//	bc.y = C->uv->y - B->uv->y;
	//	bc.z = C->zValue - B->zValue;
	//	crossV3(normal, ab, bc);
	//	normalized(normal);
	////	if (VECTOR::dotV3(normal, viewDir) < 0)
	////		continue;    
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
	//RasterizationTriangle(A, B, C);
}