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
#include"PipelineController.h"
int width, height;   

PipelineController pipeline;
ObjectHolder * holder;


/*
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
	//	DrawPoint(i, j, col); 
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
	//		DrawPoint(y, x0 + i);
		else
	//		DrawPoint(x0 + i, y);
		error = error - deltay;
		if (error < 0)
		{
			y = y + ystep;
			error = error + deltax;
		}
	}

}
*/
   


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


//create a cube and set with a location ,add into objectinfo  
/*
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
*/

/*
void ClipOne(VERT* A , VERT* B ,VERT* C,VERT* D , VERT* E ) {
	float kAB,kAC;
	D->position->x = -1; D->position->y = -1; E->position->x = -1; E->position->y = -1;
	if (A->position->x < 0 ) {
		if (A->position->y < 0) {
			// clip using line x = 0
			if (!ValidScreamPos(D->position)) { 
				kAB = (A->position->y - B->position->y) / (A->position->x - B->position->x);
				D->position->x = 0; D->position->y = (int)(A->position->y - A->position->x * kAB);
			}
			if (!ValidScreamPos(E->position)) {
				kAC = (A->position->y - C->position->y) / (A->position->x - C->position->x);
				E->position->x = 0; E->position->y = (int)(A->position->y - A->position->x * kAC); 
			}
			// clip using line y = 0 
			//if D does not hava valid value 
			if (!ValidScreamPos(D->position)) {
				kAB = (A->position->x - B->position->x) / (A->position->y - B->position->y);
				D->position->y = 0; D->position->x = (int)(A->position->x - A->position->y * kAB); 
			}
			if (!ValidScreamPos(E->position)) { 
				kAC = (A->position->x - C->position->x) / (A->position->y - C->position->y);
				E->position->y = 0; E->position->x = (int)(A->position->x - A->position->y * kAC);
			}  
		}
		else if (A->position->y >= height) {
			// clip using line x = 0
			if (!ValidScreamPos(D->position)) {
				kAB = (A->position->y - B->position->y) / (A->position->x - B->position->x);
				D->position->x = 0; D->position->y = (int)(A->position->y - A->position->x * kAB);
			}
			if (!ValidScreamPos(E->position)) {
				kAC = (A->position->y - C->position->y) / (A->position->x - C->position->x);
				E->position->x = 0; E->position->y = (int)(A->position->y - A->position->x * kAC);
			}
			// clip using line y = height -1 
			//if D does not hava valid value 
			if (!ValidScreamPos(D->position)) {
				kAB = (A->position->x - B->position->x) / (A->position->y - B->position->y);
				D->position->y = height-1; D->position->x = (int)(A->position->x + (height-1- A->position->y )* kAB);
			}
			if (!ValidScreamPos(E->position)) {
				kAC = (A->position->x - C->position->x) / (A->position->y - C->position->y);
				E->position->y = height-1; E->position->x = (int)(A->position->x + (height - 1 - A->position->y)* kAC);
			}

		}
		else { 
			// clip using line x = 0
			if (!ValidScreamPos(D->position)) {
				kAB = (A->position->y - B->position->y) / (A->position->x - B->position->x);
				D->position->x = 0; D->position->y = (int)(A->position->y - A->position->x * kAB);
			}
			if (!ValidScreamPos(E->position)) {
				kAC = (A->position->y - C->position->y) / (A->position->x - C->position->x);
				E->position->x = 0; E->position->y = (int)(A->position->y - A->position->x * kAC);
			}
		}

	}
	else if( A->position->x >= width) {
		
		if (A->position->y < 0) {
			// clip using line x = width -1
			if (!ValidScreamPos(D->position)) { 
				kAB = (A->position->y - B->position->y) / (A->position->x - B->position->x);
				D->position->x = width -1; D->position->y = (int)(A->position->y+( width-1- A->position->x) * kAB);
			}
			if (!ValidScreamPos(E->position)) {
				kAC = (A->position->y - C->position->y) / (A->position->x - C->position->x);
				E->position->x = width - 1; E->position->y = (int)(A->position->y + (width - 1 - A->position->x) * kAC);
			}
			// clip using line y = 0 
			//if D does not hava valid value 
			if (!ValidScreamPos(D->position)) {
				kAB = (A->position->x - B->position->x) / (A->position->y - B->position->y);
				D->position->y = 0; D->position->x = (int)(A->position->x - A->position->y * kAB); 
			}
			if (!ValidScreamPos(E->position)) { 
				kAC = (A->position->x - C->position->x) / (A->position->y - C->position->y);
				E->position->y = 0; E->position->x = (int)(A->position->x - A->position->y * kAC);
			}  
		}
		else if (A->position->y >= height) {
			// clip using line x = width -1
			if (!ValidScreamPos(D->position)) {
				kAB = (A->position->y - B->position->y) / (A->position->x - B->position->x);
				D->position->x = width - 1; D->position->y = (int)(A->position->y + (width - 1 - A->position->x) * kAB);
			}
			if (!ValidScreamPos(E->position)) {
				kAC = (A->position->y - C->position->y) / (A->position->x - C->position->x);
				E->position->x = width - 1; E->position->y = (int)(A->position->y + (width - 1 - A->position->x) * kAC);
			}
			// clip using line y = height -1 
			//if D does not hava valid value 
			if (!ValidScreamPos(D->position)) {
				kAB = (A->position->x - B->position->x) / (A->position->y - B->position->y);
				D->position->y = height-1; D->position->x = (int)(A->position->x + (height-1- A->position->y )* kAB);
			}
			if (!ValidScreamPos(E->position)) {
				kAC = (A->position->x - C->position->x) / (A->position->y - C->position->y);
				E->position->y = height-1; E->position->x = (int)(A->position->x - (height - 1 - A->position->y)* kAC);
			}

		}
		else { 

			// clip using line x = width -1
			if (!ValidScreamPos(D->position)) {
				kAB = (A->position->y - B->position->y) / (A->position->x - B->position->x);
				D->position->x = width - 1; D->position->y = (int)(A->position->y + (width - 1 - A->position->x) * kAB);
			}
			if (!ValidScreamPos(E->position)) {
				kAC = (A->position->y - C->position->y) / (A->position->x - C->position->x);
				E->position->x = width - 1; E->position->y = (int)(A->position->y + (width - 1 - A->position->x) * kAC);
			}
		}

	}
}
*/

void RenderACube() {
	VECTOR4  pos (5, 0, 4, 1); 
//	eye.GenerateCenterPoint();  
	Object *object = new Object(); 
	pipeline.CreateObject("cube001", "cube", &pos);
	pipeline.RenderAll(); 
//	pipeline.RenderTarget(*object);
	
}  

float  t = 0;
void Mainloop() {
	t += 3; 
	if (t > 360)
		t = 0;
	Object* ob = pipeline.GetObject("cube001");
	ob->SetRotation(0, 1, 1, t);
	VECTOR4 col;
	glClear(GL_COLOR_BUFFER_BIT);
	glBegin(GL_POINTS);
	for (int x = 0; x < width; ++x)
	{
		for (int y = 0; y < height; ++y) {
			col = pipeline.GetBufferColor(x, y);
			glColor3f(col.x, col.y, col.z);
			glVertex2i(x, height - y);
		}
	}
	pipeline.RenderAll();
	glEnd();
	glFlush();
}
void RenderWindow()
{ 
	VECTOR4 col;  
	pipeline.RenderAll();
	glClear(GL_COLOR_BUFFER_BIT);
	glBegin(GL_POINTS); 
	for (int x = 0; x < width; ++x)
	{
		for (int y = 0; y < height; ++y) {
			col = pipeline.GetBufferColor(x, y);
			glColor3f(col.x, col.y, col.z);
			glVertex2i(x, height - y);
		}
	}
	glEnd();
	glFlush();
		 
} 
int main(int argc, char ** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition(0,0);
	width = 1500; 
	height = 1080;
	//eye.SetWidthHeight(width, height);  

	VECTOR4 col(0.953, 0.447, 0.8156, 1); 
	VECTOR2 from(100, 0), to(0, 245);
	VECTOR4 lineColor(0.1, 0.1, 0.1, 1);
	 

	glutInitWindowSize(width, height);
	glutCreateWindow("sth");
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0.0, width, 0.0, height);
	glutDisplayFunc(RenderWindow);  
	pipeline.CreateBuffer(width, height);
	holder = pipeline.GetObjectHolder();
	RenderACube(); 
	glutIdleFunc(Mainloop);
	glutMainLoop();
	return 0; 
}
