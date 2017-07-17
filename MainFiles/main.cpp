#include <iostream>
#include<string>
#include<fstream>
#include<sstream>
#include<math.h>

using namespace std;


#include <GL/GLUT.h> 
#include"ObjectInfo.h" 
#include"camera.h"
int width, height;
ObjectHolder holder;
Camera eye;
VECTOR4* colorBuffer;
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
	for (int i = 0; i < width * height; i++) {
		copy(penCol, &colorBuffer[i]);
	} 
}

void ClearColBuffer() { 

	for (int i = 0; i < width * height; i++) {
		copy(backgroundColor, &colorBuffer[i]);
	}
}

void DrawPoint( int x, int y,VECTOR4* col = NULL) {
	if (col == NULL)
		col = penCol;
	int index = y* width + x;
	copy(col, &colorBuffer[index]);
}

void Drawline(VECTOR2* from , VECTOR2* to ,VECTOR4 * col = NULL) {
	if (col == NULL)
		col = penCol;
	float step = 0.5f; 
	VECTOR2 del,drawPoint;
	del.x = to->x - from->x; 
	del.y = to->y - from->y;
	drawPoint.x = from->x; drawPoint.y = from->y;
	normalized(&del);
	del.x *= step;
	del.y *= step;
	while (distance(&drawPoint , to) > 0.5f)
	{
		drawPoint.x += del.x; 
		drawPoint.y += del.y;
		int i = drawPoint.x, j = drawPoint.y; 
		DrawPoint(i, j, col); 
	}
	step = 1;//donothing here for a breakpoint
	

}

// main loop for opengl
//read a cube file and create a cube with basic info 
GEOMETRY* ReadCubeFile(string filename) { 
	string line;
	ifstream objFile(filename, ifstream::in);
	GEOMETRY* cube = new GEOMETRY();
	 
	cube->v = new VECTOR4[8];
	int vertCount = 0;
	while (getline(objFile, line)) {  
		stringstream ss(line);  
		string token;
		ss >> token;
		// read vertex here 
		if (token == "v") { 
			ss >> cube->v[vertCount].x >> cube->v[vertCount].y >> cube->v[vertCount].z;
			cube->v[vertCount].w = 1; // make it a be  point 
			vertCount++;
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
	denominator = dot(minusRes, eye.direction);
	if (denominator == 0)
		return new VECTOR4(0,0,0,-1);
	VECTOR::minus(minusRes, eye.centerPoint, eye.position);
	fraction = dot(minusRes, eye.direction);
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
	holder.GetVertsWorldCoor(verts->verts,cube);
	eye.GetCamCoordinateTransformVert(verts);
	eye.GetClipSpaceTransfromVert(verts);
	eye.GetUV(verts);

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
	width = 1200; 
	height = 800;
	eye.SetWidthHeight(width, height);  

	VECTOR4 col(0.953, 0.447, 0.8156, 1);
	SetPenColor(&col);
	CreateBuffer();
	VECTOR2 from(0, 0), to(500, 500);
	VECTOR4 lineColor(0.1, 0.1, 0.1, 1);

	Drawline(&from, &to, &lineColor);

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
