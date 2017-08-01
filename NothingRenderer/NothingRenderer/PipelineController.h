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
	int m_threads;
	DirectionLight m_directionLight;
	//float orthoLeft, orthoRight, orthoTop, orthoBottom;
	bool isShadowMapGen = false;
	bool useVShader = true,useFShader = false,useTexture;
	VECTOR4 lightDir;
	PipelineController();
	void CreateBuffer(int w, int h) {
		m_buffer.SetWidthHeight(w, h); 
		m_buffer.CreateBuffer(&m_penCol); 
		m_cam.SetWidthHeight(w, h);
		m_directionLight.SetWidthHeight(w, h);
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
	void GetVertsWorldSpace(Object* obj, MatrixStore& store,  bool normalConvert = false) { 
		GenerateTransformMatrix(obj->position , &store.loacl2WorldM); 
		for (int i = 0; i < obj->prefab->vertCount; i++) {
			matrixdot(obj->verts[i].position, obj->prefab->v[i], &store.local2WorldR); 
			matrixdot(obj->verts[i].position, obj->verts[i].position,&store.loacl2WorldM);

		//	matrixdot(obj->verts[i].normal, obj->verts[i].normal, &store.local2WorldR); 

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
	void GetAffineWeight(VECTOR4* A , VECTOR4 * B ,  VECTOR4* C,  float Px ,float Py , float pw, VECTOR3* res) {
		float a, b, c;
		float x1, y1, x2, y2, x3, y3;
		x1 = A->x /A->w;
		y1 = A->y /A->w;
		x2 = B->x /B->w;
		y2 = B->y /B->w;
		x3 = C->x /C->w;
		y3 = C->y /C->w;
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
	} 
	float GetWValue(VECTOR4* A  ,VECTOR4 * B,    VECTOR4* C ,float Px,float  Py) {
		float a, b, c;
		float  x1, y1, x2, y2, x3, y3;
		x1 = A->x  ;
		y1 = A->y  ;
		x2 = B->x  ;
		y2 = B->y ;
		x3 = C->x  ;
		y3 = C->y  ; 
		c = ((y1 - y2) * Px + (x2 - x1) * Py + x1 * y2 - x2 * y1) / (float)((y1 - y2) * x3 + (x2 - x1) * y3 + x1 * y2 - x2 * y1);
		b = ((y1 - y3) * Px + (x3 - x1) * Py + x1 * y3 - x3 * y1) / (float)((y1 - y3) * x2 + (x3 - x1) * y2 + x1 * y3 - x3 * y1);
		a = 1 - b - c;
		return A->w * a + B->w * b + C->w * c;
	}
	float GetZValue(VECTOR4* A, VECTOR4 * B, VECTOR4* C, float Px, float  Py) {
		float a, b, c;
		float  x1, y1, x2, y2, x3, y3;
		x1 = A->x;
		y1 = A->y;
		x2 = B->x;
		y2 = B->y;
		x3 = C->x;
		y3 = C->y; 

		c = ((y1 - y2) * Px + (x2 - x1) * Py + x1 * y2 - x2 * y1) / (float)((y1 - y2) * x3 + (x2 - x1) * y3 + x1 * y2 - x2 * y1);
		b = ((y1 - y3) * Px + (x3 - x1) * Py + x1 * y3 - x3 * y1) / (float)((y1 - y3) * x2 + (x3 - x1) * y2 + x1 * y3 - x3 * y1);
		a = 1 - b - c;
		if (a < 0 || a> 1 || b < 0 || b > 1 || c< 0 || c > 1) {
			return -99;
		}
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
	void VertexShader(VERT& A ,VShaderInfo & info  ) {
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
	void DrawLine(DrawTrianglePoint& a, DrawTrianglePoint& b, DrawTrianglePoint& c, VShaderInfo& info ) {
	//	//for line ab 
	//	VECTOR4 col(1, 0, 0, 0);
	//	float u = a.x, v = a.y;
	////	float depth;
	//	for (int i = 0; i <= a.dis; i++) {
	//		u = a.x + i* a.dir.x; v = a.y + i* a.dir.y;
	//		if (!ValidScreamPos(u, v))
	//			continue; 
	//		  
	//		m_buffer.WriteToColorBuffer(u, v, &col);
	//		m_buffer.WriteToZBuffer(99,u, v );

	//	}

	//	u = b.x, v = b.y;
	//	for (int i = 0; i < b.dis; i++) {
	//		u = b.x + i* b.dir.x; v = b.y + i* b.dir.y;
	//		if (!ValidScreamPos(u, v))
	//			continue;
	//		 

	//		m_buffer.WriteToColorBuffer(u, v, &col);
	//		m_buffer.WriteToZBuffer(99, u, v);
	//		 
	//	}



	//	u = c.x, v = c.y;
	//	for (int i = 0; i < c.dis; i++) {
	//		u = c.x + i* c.dir.x; v = c.y + i* c.dir.y;
	//		if (!ValidScreamPos(u, v))
	//			continue; 
	//		 
	//		 
	//		m_buffer.WriteToColorBuffer(u , v, &col);
	//		m_buffer.WriteToZBuffer(99, u, v);
	//	}

	}
	bool FragmentProcessShader(float x, float y, VERT* a, VERT*  b, VERT* c, VShaderInfo& info, VECTOR4* outColor) {
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
	void DrawPoint(int x, int y, VShaderInfo& info) {
		if (!ValidScreamPos(x, y)) {
			return;
		}
		VECTOR4 col(1, 1, 1, 1);
		float depth = GetZValue(info.A->position, info.B->position, info.C->position, x, y);
		if (depth < m_buffer.GetZBufferValue(x, y) || depth < 0 ) { 
			return;
		}
		if (FragmentProcessShader(x, y, info.A, info.B, info.C, info, &col)) {
			m_buffer.WriteToZBuffer(depth, x, y);
		} 
	}

	void ShowShadowMap() {
		VECTOR4 col;
		float dis = m_directionLight.farZ - m_directionLight.nearZ;
		for (int i = 0; i < m_directionLight.width; i++) {
			for (int j = 0; j < m_directionLight.height -1; j++) {
				col.x =1 / (dis* m_directionLight.ReadShadowMap(i, j));
				col.y = col.x; col.z = col.x;
				m_buffer.WriteToColorBuffer(i, j,&col);
			} 
		}


	}
	void RasterLineShadow(DrawTrianglePoint a, DrawTrianglePoint b, VShaderInfo info) { 
		float xl = a.x, xr = b.x;
		int errorPoint = 0, normalPoint = 0;
		if (xl == xr) { 
			return;
		}
		VECTOR4 col(1, 1, 1, 0);
		bool lr = true;
		if (xl > xr) {
			swap(xl, xr);
		}
		float length = m_directionLight.farZ - m_directionLight.nearZ;
		for (int i = 0; xl + i <= xr; i++) {
			if (!ValidScreamPos(xl + i, a.y)) {
				continue;
			}
			VECTOR4 col(1, 1, 1, 0);
			float depth;
			if (a.FROM == b.FROM)
				depth = GetZValue(a.FROM->position, a.TO->position, b.TO->position, xl + i, a.y);
			else
				depth = GetZValue(a.FROM->position, a.TO->position, b.FROM->position, xl + i, a.y);

			depth = 1 / depth; 
			if ( depth < m_directionLight.ReadShadowMap(xl + i, a.y) || depth < 0) {
				continue;
			}  
			m_directionLight.WriteToShadowMap(depth,xl + i, a.y); 
		} 
	}
	void RasterLine(DrawTrianglePoint a,DrawTrianglePoint b,VShaderInfo info) { 
	 
		float xl = a.x , xr = b.x  ;  
		int errorPoint = 0, normalPoint = 0;
		if (xl == xr) { 
			return;
		}
		VECTOR4 col(1, 1, 1, 0); 
		bool lr = true;
		if (xl > xr) {
			swap(xl, xr);  
		}    

		xl = xl - 2; xr = xr + 2;

		for (int i = 0; xl + i <= xr ; i++) {
			if (!ValidScreamPos(xl + i, a.y)) {  
				continue;
			}
			VECTOR4 col(1, 1, 1, 0);
			float depth;
			if(a.FROM == b.FROM)
				depth = GetZValue(a.FROM->position, a.TO->position, b.TO->position, xl + i, a.y);
			else
				depth = GetZValue(a.FROM->position, a.TO->position, b.FROM->position, xl + i, a.y); 


			depth = 1 / depth;
			if (depth < m_buffer.GetZBufferValue(xl + i, a.y) || depth < 0) {
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
			if(useTexture)
				copy(m_BMPManager.GetBMPColor(u, v), &col);
			if (useVShader) { 
				dot(&col, &col, VlightAdjust);
			}
		//	col.x = 0; col.y = 0; col.z = 0;
			m_buffer.WriteToColorBuffer(xl + i, a.y,&col);


			m_buffer.WriteToZBuffer(depth, xl + i, a.y);
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
		if ((int)B->uv->y == (int)A->uv->y) {
			VERT* temp = A; A = C;C = temp; 
		} 
		if (A->uv->y > B->uv->y)
			yDir = -1;  

		float disAB = distance2D(*(A->uv), *(B->uv));
		
		float disAC = distance2D(*(A->uv), *(C->uv));


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



		if ((int) A->uv->y == (int)B->uv->y && (int)A->uv->y == (int)C->uv->y)
		{ 
			return;
		}

		if(!isShadowMapGen)
			DrawLine(ab, ac,bc, info); 

		for (; ab.count < disAB && ac.count < disAC;) {  

			while ((int)ab.y != y )
			{
				ab.count++;	ab.x += ab.dir.x; ab.y += ab.dir.y;
				//DrawPoint(ab,info);  
			}
			while ((int)ac.y != y)
			{
				ac.count++;  ac.Move(); 
				//DrawPoint(ac, info);  
			} 
			if (ab.count >= disAB || ac.count >= disAC)
				break;
			if ((int)ab.y !=(int) ac.y)
				cout << "error occur in raster" << endl;
			if(!isShadowMapGen)
				RasterLine(ab, ac, info); 
			else
				RasterLineShadow(ab, ac, info);

			
			y += yDir;
		}

		if (B->uv->y == C->uv->y) {
			return;
		}

		if (ab.count >= disAB && ac.count >= disAC)
			return;
		if (ab.count >= disAB) { 
			bc.x = B->uv->x; bc.y = B->uv->y;
			dir.x = C->uv->x - B->uv->x; dir.y = C->uv->y - B->uv->y;
			normalized(dir); bc.dir.x = dir.x; bc.dir.y = dir.y;
			bc.FROM = B; bc.TO = C;
			ab.count = ac.count;	Swap(ab, ac); disAB = disAC;
		}  

		for (; ab.count < disAB && bc.count < disbc;) {
			if(!isShadowMapGen)
				RasterLine(ab, bc, info);
			else
				RasterLineShadow(ab, bc, info);
			while ((int)ab.y != y)
			{
				ab.count++;	ab.Move();
				//DrawPoint(ab, info);
				if (ab.count >= disAB)
					break;
			}
			while ((int)bc.y != y)
			{
				bc.count++;  bc.Move();
				//DrawPoint(bc, info);
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
		viewDir->x = 0; viewDir->y = 0; viewDir->z = -1;
		if (dotV3(normal, *viewDir) < 0 && !pipeline->isShadowMapGen)
			continue;
		if (!pipeline->ValidScreamPos(A->uv->x, A->uv->y)) {
			if (!pipeline->ValidScreamPos(B->uv->x, B->uv->y)) {
				if (!pipeline->ValidScreamPos(C->uv->x, C->uv->y)) {
					continue;
				}
			} 
		}
		if (A->position->z < 0 && B->position->z < 0 && C->position->z < 0) {
			continue;
		}
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
	isShadowMapGen = true;
	for (int i = 0; i < targetLength; i++) {
		if (m_renderTargets[i]->prefab->name == "cube")
			useTexture = true;
		else
			useTexture = false;
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
	 
	for (int i = 0; i < 250; i++) {
		for (int j = 0; j < 2250; j++) {
			DrawTriangle(150, 30, 0, 0, 30, 180, i, j);
		}
	}
	ShowShadowMap();
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
	VECTOR4 col(0.953, 0.447, 0.8156, 1);
	copy(&col, &m_penCol);
	lightDir.x = 0; lightDir.y = 0; lightDir.z = -1; lightDir.w = 0;
	normalizedVector3(&lightDir, &lightDir);
	m_lightIntense = 1.5;
	m_lightDiffuse = 0.6;
	m_threads = 5;
}

bool PipelineController::RemoveRenderTarget(Object ob) {
	return false;
}
void PipelineController::RenderShadowMap(Object& object) {
	VECTOR4 viewDir(0, 0, 0, 0);
	int a, b, c;
	int na, nb, nc;
	VECTOR2 va, vb, vc;
	GenerateRotateMatrix(*object.rotation, &m_directionLight.store.local2WorldR);
	GenerateRotateMatrix(m_directionLight.rotation, &m_directionLight.store.W2CamR);
	GenerateTransformMatrix(object.position, &m_directionLight.store.loacl2WorldM); 
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
		vInfo[i].order[0] = AA.vertNum;
		vInfo[i].order[1] = BB.vertNum;
		vInfo[i].order[2] = CC.vertNum;
		VertexShader(AA, vInfo[i]);
		VertexShader(BB, vInfo[i]);
		VertexShader(CC, vInfo[i]);
	}  
	GetVertsWorldSpace(&object, ref(m_directionLight.store), true);
	m_directionLight.GetLightCoordinateTransformVert(&object);
	m_directionLight.GenerateOrthoMatrix(m_directionLight.store.C2OthroM);
	
	m_directionLight.GetClipSpaceTransfromVert(&object );
	VECTOR4 col(0.1, 0.1, 0.1, 1);
	VECTOR3 ab, bc, normal;
	minusV3(&viewDir, m_cam.position, object.position);
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
	} 

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
}
void PipelineController::RenderTarget(Object &  object) { 
	VECTOR4 viewDir(0, 0, 0, 0);
	int a, b, c;
	int na, nb, nc;
	VECTOR2 va, vb, vc; 
	GenerateRotateMatrix(*object.rotation,  &store.local2WorldR);
	GenerateRotateMatrix(m_cam.rotation, &store.W2CamR);
	GenerateTransformMatrix(object.position, &store.loacl2WorldM);
	//cout << "term" << endl;
	VShaderInfo* vInfo = new VShaderInfo[object.prefab->faceCount];
	GetVertsWorldSpace(&object, ref(store), true);
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
	//GenerateOrthoMatrix(store.C2OthroM);
	m_cam.GetClipSpaceTransfromVert(&object,store);
	VECTOR4 col(0.1, 0.1, 0.1, 1);
	VECTOR3 ab, bc, normal;
	minusV3(&viewDir, m_cam.position, object.position);  
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
	//	ab.x = B->uv->x - A->uv->x;
	//	ab.y = B->uv->y - A->uv->y;
	//	ab.z = B->zValue - A->zValue;
	//	bc.x = C->uv->x - B->uv->x;
	//	bc.y = C->uv->y - B->uv->y;
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