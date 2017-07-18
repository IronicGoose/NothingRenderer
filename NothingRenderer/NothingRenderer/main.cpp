#include <iostream>
#include<string>
#include<fstream>
#include<sstream>
#include<algorithm>
#include<list> 
using namespace std;


#include <GL/GLUT.h> 
#include"ObjectInfo.h" 
#include"camera.h"
int width, height;
ObjectHolder holder;
Camera eye;
VECTOR4* colorBuffer;
float* ZBuffer;
bool isCreated = false;


VECTOR4* penCol;
VECTOR4* backgroundColor;

void SetPenColor(VECTOR4* col) {
	if (penCol == nullptr) {
		penCol = new VECTOR4();
		backgroundColor = new VECTOR4;
		copy(col, backgroundColor); 
	} 
	penCol->x = col->x; penCol->y = col->y;
	penCol->z = col->z; penCol->w = col->w;
	return;

} 



void CreateBuffer() {
	if (penCol == NULL) {
		cerr << "pen color not set " << endl;
		return;
	}

	colorBuffer = new VECTOR4[width * height];
	ZBuffer = new float[width * height];
	for (int i = 0; i < width * height; i++) {
		copy(penCol, &colorBuffer[i]);
		ZBuffer[i] = -99;
	} 
}

void ClearColBuffer() { 

	for (int i = 0; i < width * height; i++) {
		copy(backgroundColor, &colorBuffer[i]);
	}
}

void DrawPoint( int x, int y,VECTOR4* col  ) { 
	int index = y* width + x;
	if (index >= width * height)
	{
		cerr << "draw point out of scream" << endl;
	}
	copy(col, &colorBuffer[index]); 
}
void DrawPoint(int x, int y ) { 
	int index = y* width + x;
	if (index >= width * height)
	{
		cerr << "draw point out of scream" << endl;
	}
	copy(penCol, &colorBuffer[index]);
}

void Drawline(VECTOR2* from , VECTOR2* to ,VECTOR4 * col = NULL) {
	if (col == NULL)
		col = penCol;
	float step = 1; 
	VECTOR2 del,drawPoint;
	del.x = to->x - from->x; 
	del.y = to->y - from->y;
	drawPoint.x = from->x; drawPoint.y = from->y;
	normalized(&del);
	del.x *= step;
	del.y *= step;
	while (sqdistance(&drawPoint , to) > 1)
	{
		drawPoint.x += del.x; 
		drawPoint.y += del.y;
		int i = drawPoint.x, j = drawPoint.y; 
		DrawPoint(i, j, col); 
	}
	cout << "draw line " << col->x << " " << col->y << " " << col->z << endl;
	step = 1;//donothing here for a breakpoint 
}



void DrawLine2(VECTOR2* from, VECTOR2* to, VECTOR4 * col = NULL) { 
	if (col == NULL)
		col = penCol; 
	int x0,x1,y0,y1;
	x0 = from->x, x1 = to->x, y0 = from->y, y1 = to->y;
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
	for (int i = 0; x0 + i < x1; i++) {
		if (steep)
			DrawPoint(y, x0 + i);
		else
			DrawPoint(x0 + i, y);
		error = error - deltay;
		if (error < 0)
		{
			y = y + ystep;
			error = error + deltax;
		}
	}

}

float TriangleArea(int x0,int y0, int x1 ,int y1) {
	return abs(( (float)(x0 *y1  - x1 * y0 ))   /2 ) ; 
}

void SampleColor(VECTOR4* A, VECTOR4 * B, VECTOR4* C,VECTOR4 * P,VECTOR4* res) { 
	float a, b, c,total;
	c = TriangleArea(A->x - P->x, A->y - P->y, B->x - P->x, B->y - P->y);
	b = TriangleArea(A->x - P->x, A->y - P->y, C->x - P->x, C->y - P->y);
	a = TriangleArea(B->x - P->x, B->y - P->y, C->x - P->x, C->y - P->y);
	total = a + b + c;
	a = a / total;
	b = b / total;
	c = c / total;
	res->x = a ;
	res->y = b;
	res->z = c; 
}

void Swap(VERT * a, VERT * b) {
	VERT * temp = a;
	a = b;
	b = temp;
}
float GetDepth(int x, int y) {

	int index = y* width + x; 
	//return 99;
	return ZBuffer[index];
}
void WriteDepth(float depth, int x, int y) {
	int index = y* width + x;
	ZBuffer[index] = depth; 
} 
void DrawTriangle3(VERT* A, VERT* B, VERT * C,bool recurse = true) {

	if ((A->position->y == B->position->y|| A->position->y == C->position->y)) {
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
		DrawTriangle3(B, C, A,false);
		DrawTriangle3(C, B, A,false); 
	}

	float step = 0.5,dis = 0.5f;
	VECTOR2 delAB,delAC;
	VECTOR4	drawPointAB,drawPointAC;
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
	y+= dir;
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
			if (depth > GetDepth(i, j)) { 
				WriteDepth(depth, i, j);
				Col.x = A->color->x * res.x + B->color->x * res.y + C->color->x * res.z;
				Col.y = A->color->y * res.x + B->color->y * res.y + C->color->y * res.z;
				Col.z = A->color->z * res.x + B->color->z * res.y + C->color->z * res.z;

				DrawPoint(i, j,&Col);
			}
			continue;
		}
		while(sqdistance2D(&drawPointAC, C->position) > dis)
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
				if (depth > GetDepth(k, w)) {
					WriteDepth(depth, k, w);
					Col.x = A->color->x * res.x + B->color->x * res.y + C->color->x * res.z;
					Col.y = A->color->y * res.x + B->color->y * res.y + C->color->y * res.z;
					Col.z = A->color->z * res.x + B->color->z * res.y + C->color->z * res.z;
					DrawPoint(k, w,&Col);
				}
				continue;
			}
			int x = min(i, k), xm = max(i, k);
			P.y = y;
			for (int ii = 0; x + ii < xm; ii++) {
				P.x = x + ii;
				SampleColor(A->position, B->position, C->position, &P, &res);
				depth = A->position->z * res.x + B->position->z * res.y + C->position->z * res.z;
				if (depth > GetDepth(x+ii, y)) {
					WriteDepth(depth, x + ii, y);
					Col.x = A->color->x * res.x + B->color->x * res.y + C->color->x * res.z;
					Col.y = A->color->y * res.x + B->color->y * res.y + C->color->y * res.z;
					Col.z = A->color->z * res.x + B->color->z * res.y + C->color->z * res.z;
					//calculate Z  value in this position
					DrawPoint(x + ii, y, &Col);
				}
			} 
			y += dir; 
			break; 
		}
	}



}


/*
void DrawTriganle2(VERT* A, VERT* B, VERT * C ) {
	if( !(A->position->y == B->position->y && A->position->y == C->position->y)) { 
		if (A->position->y == B->position->y) {
			DrawTriganle2(C, B, A);
			return;
		}
		if (A->position->y == C->position->y) {
			DrawTriganle2(B, C, A);
			return;
		}
	}
	int x0 = A->position->x, x1 = B->position->x, y0 = A->position->y, y1 = B->position->y;
	int x2 = A->position->x, x3 = C->position->x, y2 = A->position->y, y3 = C->position->y;
	bool steepab = abs(y1 - y0) > abs(x1 - x0);
	bool steepac = abs(y3 - y2) > abs(x3 - x2);
	if (steepab) {
		swap(x0, y0);
		swap(x1, y1);

	}
	if (steepac) {
		swap(x2, y2);
		swap(x3, y3);

	}
	if (x0 > x1) {
		swap(x0, x1);
		swap(y0, y1);
	}
	if (x2 > x3) {
		swap(x2, x3);
		swap(y2, y3);
	}


	int deltabx = x1 - x0;
	int deltaby = abs(y1 - y0);

	

	int deltacx = x3 - x2;
	int deltacy = abs(y3 - y2);

	int errorb = deltabx / 2;
	int errorc = deltacx / 2;
	int ybstep;
	int  yb = y0;
	int ycstep;
	int  yc = y2;

	if (y0 < y1) {
		ybstep = 1;
	}
	else
		ybstep = -1;


	if (y2 < y3) {
		ycstep = 1;
	}
	else
		ycstep = -1;

	bool triangleClip = false; 
	int x;
	int pointAx, pointAy;
	int pointBx, pointBy;
	if (steepab)
		pointAx = yb;
	else
		pointAx = x0;

	if (steepac)
		pointBx = yc;
	else
		pointBx = x2;
	x = max(pointAx, pointBx);
	x++;
	VECTOR4 res,P;
	bool bUpdate = true, cUpdate = true;
	for (int i = 0; x0 + i < x1 || x2 + i < x3; i++) {

		if (x0 + i < x1  && bUpdate ) {

			if (steepab) {
				pointAx = yb;
				pointAy = x0 + i;

				P.x = pointAx; P.y = pointAy;
				SampleColor(A->position, B->position, C->position, &P, &res);
				P.x = A->color->x * res.x + B->color->x * res.y + C->color->x * res.z;
				P.y = A->color->y * res.x + B->color->y * res.y + C->color->y * res.z;
				P.z = A->color->z * res.x + B->color->z * res.y + C->color->z * res.z;
				//calculate Z  value in this position
				DrawPoint(yb, x0 + i,&P);
				cout << " A" << pointAx << " " << pointAy << endl;
				
			}
			else {
				pointAx = x0 + i;
				pointAy = yb;

				P.x = pointAx; P.y = pointAy;
				SampleColor(A->position, B->position, C->position, &P, &res);
				P.x = A->color->x * res.x + B->color->x * res.y + C->color->x * res.z;
				P.y = A->color->y * res.x + B->color->y * res.y + C->color->y * res.z;
				P.z = A->color->z * res.x + B->color->z * res.y + C->color->z * res.z;
				//calculate Z  value in this position
				DrawPoint(x0 + i, yb,&P);
				cout<<" A" << pointAx << " " << pointAy << endl;
			}
			errorb = errorb - deltaby;
			if (errorb < 0)
			{
				yb = yb + ybstep;
				errorb = errorb + deltabx;
			}
		}
		else {
			triangleClip = true;
		}

		if (x2 + i < x3  && cUpdate) {
			if (steepac) {
				pointBx = yc; 
				pointBy = x2 + i;

				P.x = pointBx;
				P.y = pointBy; 
				SampleColor(A->position, B->position, C->position, &P, &res);
				P.x = A->color->x * res.x + B->color->x * res.y + C->color->x * res.z;
				P.y = A->color->y * res.x + B->color->y * res.y + C->color->y * res.z;
				P.z = A->color->z * res.x + B->color->z * res.y + C->color->z * res.z;
				//calculate Z  value in this position
				cout << "B " << pointBx << " " << pointBy << endl;
				DrawPoint(yc, x2 + i,&P);
			}
			else {
				pointBx = x2 + i;
				pointBy = yc;

				P.x = pointBx;
				P.y = pointBy;
				SampleColor(A->position, B->position, C->position, &P, &res);
				P.x = A->color->x * res.x + B->color->x * res.y + C->color->x * res.z;
				P.y = A->color->y * res.x + B->color->y * res.y + C->color->y * res.z;
				P.z = A->color->z * res.x + B->color->z * res.y + C->color->z * res.z;
				cout << "B " << pointBx << " " << pointBy << endl;
				//calculate Z  value in this position
				DrawPoint(x2 + i, yc,&P);
			}
			errorc = errorc - deltacy;
			if (errorc < 0)
			{
				yc = yc + ycstep;
				errorc = errorc + deltacx;
			} 
		}
		else {
			triangleClip = true;
		}

		if (  x == pointAx && x == pointBx) {
			P.x = x;
			int y = min(pointAy, pointBy),ym = max(pointAy,pointBy	);
			for (int i = 0; y + i < ym; i++) { 
				P.y = y+i;
				SampleColor(A->position, B->position, C->position, &P, &res);
				P.x = A->color->x * res.x + B->color->x * res.y + C->color->x * res.z;
				P.y = A->color->y * res.x + B->color->y * res.y + C->color->y * res.z;
				P.z = A->color->z * res.x + B->color->z * res.y + C->color->z * res.z;
				//calculate Z  value in this position
				DrawPoint(x, y+i, &P);
			} 
			x++;
			
		}
		if (pointAx < pointBx) {
	//		cUpdate = false;
			bUpdate = true;
		}
		if (pointAx > pointBx) {
//			bUpdate = false;
			cUpdate = true;
		}
		if (pointAx == pointBx) {
			bUpdate = true;
			cUpdate = true;
		}


	} 

}
*/
/*
void DrawTriangle(VECTOR4* A, VECTOR4* B, VECTOR4 * C = NULL) {  
	int xab,xac, xb, yab,yac ,yb,xc,yc;
	xab = A->x; xb = B->x; xc = C->x; yab = A->y; yb = B->y; yc = C->y;
	xac = xab; yac = yab;
	bool steepAB = abs(yb - yab) > abs(xb - xab);
	bool steepAC = abs(yc - yac) > abs(xc - xac);
	if (steepAB) {
		swap(xab, yab);
		swap(xb, yb); 
	}
	if (steepAC) {
		swap(xac, yac);
		swap(xc, yc);

	}
	if (xab > xb) {
		swap(xab, xb);
		swap(yab, yb);
	}
	if (xac > xc) {
		swap(xac, xc);
		swap(yac, yc);
	}

	int deltaAB = xb - xab;
	int deltaAC = xc - xab;
	deltaAB = abs(yb - yab); deltaAC = abs(yb - yab);
	int errorAB = deltaAB / 2, errorAC = deltaAC / 2, ystepAB, ystepAC;
	yb = yab; yc = yac;

	if (yab < yb) {
		ystepAB = 1;
	}
	else
		ystepAB = -1;

	if (yac < yc) {
		ystepAC = 1;
	}
	else
		ystepAC = -1;
	 
	for (int i = 0; xab + i < xb || xac +i  < xc; i++) {
		if (xab + i < xb) {

			if (steepAB) {
				DrawPoint(yb, xab + i);
			}
			else {

				DrawPoint(xab + i, yb);
			}
			errorAB = errorAB - deltaAB;
			if (errorAB < 0)
			{
				yb = yb + ystepAB;
				errorAB = errorAB + deltaAB;
			}
		}
		if (xac + i < xc) { 
			if (steepAC) {
				DrawPoint(yc, xac + i);
			}
			else {
				DrawPoint(xac + i, yc);
			}
			errorAC = errorAC - deltaAC;
			if (errorAC < 0)
			{
				yc = yc + ystepAC;
				errorAC = errorAC + deltaAC;
			}
		}
	}

}
*/

  
// main loop for opengl
//read a cube file and create a cube with basic info 
void splitObjString(const string& s,VECTOR4 * out ) {

	int i = 0, j = 0;
	int count = 0;
	int res[3];
	for (int i = 0; s[i] != '\0';i++) {

		for (j = i; s[j] != '\0'; j++) {
			if (s[j] == '/')
			{
				break;
			}
		} 
		res[count++] = stoi(s.substr(i, j - i + 1));
		i = j;
		if (s[i] == '\0')
			break;
	}
	out->x = res[0];
	out->y = res[1];
	out->z = res[2];
}

GEOMETRY* ReadCubeFile(string filename) { 
	string line;
	ifstream objFile(filename, ifstream::in);
	GEOMETRY* cube = new GEOMETRY();
	  
	int vertMax = 8;
	int vertCount = 0;
	cube->faceCount = 0;
	while (getline(objFile, line)) {  
		stringstream ss(line);  
		string token;
		ss >> token;
		// read vertex here 
		if (token == "v") { 
			VECTOR4* vec = new VECTOR4();
			ss >> vec->x >> vec->y >> vec->z;
			vec->w = 1; // make it a be  point 
			cube->v.push_back(vec);
			vertCount++;
		}
		if (token == "f") {
			FACEINFO * face = new FACEINFO();
			string temp;
			ss >> temp;
			splitObjString(temp, &face->a);
			ss >> temp;
			splitObjString(temp, &face->b);
			ss >> temp;
			splitObjString(temp, &face->c);
			cout << temp;
			cube->f.push_back(face);
			cube->faceCount++;
		}
	}  
	cube->vertCount = vertCount;
	objFile.close();
	 
 	return cube;
}



//create a cube and set with a location ,add into objectinfo 
GEOMETRY* CreateCube(VECTOR4* pos	) {
	GEOMETRY * cube = ReadCubeFile("./Objects/cube.obj");
	cube->position->x = pos->x;
	cube->position->y = pos->y;
	cube->position->z = pos->z;
	cube->position->w = pos->w;
	holder.RegistCubeInfo(cube);
	return cube;
	
}


VECTOR4* EyePlaneIntersection(VECTOR4* worldVertex) {  
	float d; 
	float denominator, fraction;  
	VECTOR4 *minusRes = new VECTOR4(); 
	VECTOR::minus(minusRes, eye.position, worldVertex);
	denominator = dot3x3(minusRes, eye.direction);
	if (denominator == 0)
		return new VECTOR4(0,0,0,-1);
	VECTOR::minus(minusRes, eye.centerPoint, eye.position);
	fraction = dot3x3(minusRes, eye.direction);
	delete(minusRes);
	d = (float) fraction / denominator;
	VECTOR4 * res = new VECTOR4();
	dot(res,eye.position, d);
	return res;
}



void RenderACube() {
	VECTOR4 * pos = new VECTOR4(0, 0, 4, 1);
	GEOMETRY* cube = CreateCube(pos); 
	delete pos;
//	eye.GenerateCenterPoint();  




	VERT* vert = new VERT[cube->vertCount];
	VERTS* verts = new VERTS();
	verts->verts = vert;
	verts->vertCount = cube->vertCount;
	MATRIX4x4 rot;
	VECTOR4 n(0, 1, 1);
	GenerateRotateMatrix(n, 90, &rot);
	holder.GetVertsWorldSpace(verts->verts,cube,&rot);
	eye.GetCamCoordinateTransformVert(verts);
	eye.GetClipSpaceTransfromVert(verts);
	eye.GetUV(verts); 
	VECTOR2 va, vb, vc;
	VECTOR4 col(0.1, 0.1, 0.1, 1);
	int a, b, c;
	for (int i = 0; i < cube->faceCount; i++) {
		a = cube->f[i]->a.x -1;
		b = cube->f[i]->b.x -1;
		c = cube->f[i]->c.x -1;
		/*va.x = verts->verts[a].position->x; va.y = verts->verts[a].position->y;
		vb.x = verts->verts[b].position->x; vb.y = verts->verts[b].position->y;
		vc.x = verts->verts[c].position->x, vc.y = verts->verts[c].position->y; 
		*/
		DrawTriangle3(&verts->verts[a], &verts->verts[b], &verts->verts[c]); 
		cout << a; 

	}
	a = cube->f[1]->a.x - 1;
	b = cube->f[1]->b.x - 1;
	c = cube->f[1]->c.x - 1;
	//DrawTriangle3(&verts->verts[a], &verts->verts[b], &verts->verts[c]); 

}  



void RenderWindow()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glBegin(GL_POINTS);
	for (int x = 0; x < width; ++x)
	{
		for (int y = 0; y < height; ++y) {
			int index = y* width + x;
			glColor3f(colorBuffer[index].x , colorBuffer[index].y , colorBuffer[index].z);
			glVertex2i(x, height - y);
		}
	}
	glEnd();
	glFlush();
	if (!isCreated) {
		isCreated = true;
	}
}
int main(int argc, char ** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition(0,0);
	width = 1500; 
	height = 1080;
	eye.SetWidthHeight(width, height);  

	VECTOR4 col(0.953, 0.447, 0.8156, 1);
	SetPenColor(&col);
	CreateBuffer();
	VECTOR2 from(100, 0), to(0, 245);
	VECTOR4 lineColor(0.1, 0.1, 0.1, 1);

	SetPenColor(&lineColor);
	//Drawline(&from, &to, &lineColor);
	//DrawLine2(&from, &to, &lineColor);

	glutInitWindowSize(width, height);
	glutCreateWindow("sth");
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0.0, width, 0.0, height);
	glutDisplayFunc(RenderWindow);

	RenderACube(); 
	glutMainLoop(); 
	return 0; 
}
