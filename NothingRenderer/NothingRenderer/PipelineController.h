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
	void GetVertsWorldSpace(Object* obj, MATRIX4x4* rotateMatrix) {
		MATRIX4x4* ma = new MATRIX4x4();
		GenerateTransformMatrix(obj->position, ma);

		for (int i = 0; i < obj->prefab->vertCount; i++) {
			matrixdot(obj->verts[i].position, obj->prefab->v[i], rotateMatrix);
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
		}
	} 
	float TriangleArea(int x0, int y0, int x1, int y1) {
		return abs(((float)(x0 *y1 - x1 * y0)) / 2);
	}
	void SampleColor(VECTOR4* A, VECTOR4 * B, VECTOR4* C, VECTOR4 * P, VECTOR4* res) {
		float a, b, c, total;
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
	}
	void DrawTriangle3(VERT*A , VERT*B , VERT * C, bool recurse = true);

};

void PipelineController::RenderAll() {
	m_buffer.ClearColBuffer(m_penCol);
	for (int i = 0; i < targetLength; i++) {
		RenderTarget(*m_renderTargets[i]);
	}
	m_buffer.SwapBuffer();
}
void PipelineController::RenderTarget(Object &  object) {

	int a, b, c;
	MATRIX4x4 rot;
	GenerateRotateMatrix(*object.rotation, &rot); 
	for (int i = 0; i < object.prefab->faceCount; i++) {
		a = object.prefab->f[i]->a.x - 1;
		b = object.prefab->f[i]->b.x - 1;
		c = object.prefab->f[i]->c.x - 1; 
		object.verts[a].normal = object.prefab->n[a];
		object.verts[b].normal = object.prefab->n[b];
		object.verts[c].normal = object.prefab->n[c];
	}
	GetVertsWorldSpace(&object, &rot);
	m_cam.GetCamCoordinateTransformVert(&object); 
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

		if ((A->position->y == B->position->y || A->position->y == C->position->y)) {
			if (A->position->y == B->position->y) {
				DrawTriangle3(C, B, A);
				return;
			}
			if (A->position->y == C->position->y) {
				DrawTriangle3(B, C, A);
				return;
			}
		}
		if (B->position->y == C->position->y)
			recurse = false;
		if (recurse) {
			recurse = false;
			DrawTriangle3(B, C, A, false);
			DrawTriangle3(C, B, A, false);
		}

		float step = 0.2, dis = 0.2f;
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
			drawPointAB.x += delAB.x;
			drawPointAB.y += delAB.y;
			int i = drawPointAB.x, j = drawPointAB.y;
			if (j != y) {
				//draw point
				P.x = i; P.y = j;
				SampleColor(A->position, B->position, C->position, &P, &res);
				depth = A->position->z * res.x + B->position->z * res.y + C->position->z * res.z;
				if (depth > m_buffer.GetZBufferValue(i, j) && ValidScreamPos(i, j)) {
					m_buffer.WriteToZBuffer(depth, i, j);
					Col.x = A->color->x * res.x + B->color->x * res.y + C->color->x * res.z;
					Col.y = A->color->y * res.x + B->color->y * res.y + C->color->y * res.z;
					Col.z = A->color->z * res.x + B->color->z * res.y + C->color->z * res.z;

					m_buffer.WriteToColorBuffer(i, j, &Col);
				}

				continue;
			}
			while (sqdistance2D(&drawPointAC, C->position) > dis)
			{
				drawPointAC.x += delAC.x;
				drawPointAC.y += delAC.y;
				int k = drawPointAC.x, w = drawPointAC.y;
				if (w != y)
				{
					//draw point
					P.x = k; P.y = w;
					SampleColor(A->position, B->position, C->position, &P, &res);
					depth = A->position->z * res.x + B->position->z * res.y + C->position->z * res.z;
					if (depth > m_buffer.GetZBufferValue(k, w) && ValidScreamPos(k, w)) {
						m_buffer.WriteToZBuffer(depth, k, w);
						Col.x = A->color->x * res.x + B->color->x * res.y + C->color->x * res.z;
						Col.y = A->color->y * res.x + B->color->y * res.y + C->color->y * res.z;
						Col.z = A->color->z * res.x + B->color->z * res.y + C->color->z * res.z;
						m_buffer.WriteToColorBuffer(k, w, &Col);
					}
					continue;
				}
				int x = min(i, k), xm = max(i, k);
				P.y = y;
				for (int ii = 0; x + ii < xm; ii++) {
					P.x = x + ii;
					SampleColor(A->position, B->position, C->position, &P, &res);
					depth = A->position->z * res.x + B->position->z * res.y + C->position->z * res.z;
					if (depth > m_buffer.GetZBufferValue(x + ii, y) && ValidScreamPos(x + ii, y)) {
						m_buffer.WriteToZBuffer(depth, x + ii, y);
						Col.x = A->color->x * res.x + B->color->x * res.y + C->color->x * res.z;
						Col.y = A->color->y * res.x + B->color->y * res.y + C->color->y * res.z;
						Col.z = A->color->z * res.x + B->color->z * res.y + C->color->z * res.z;
						//calculate Z  value in this position
						m_buffer.WriteToColorBuffer(x + ii, y, &Col);
					}
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
}
//void PipelineController::RenderAll() {

//}
bool PipelineController::RemoveRenderTarget(Object ob) { 
	return false;
} 