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

static PipelineController pipeline;
ObjectHolder * holder; 
 

void StartRender() {
//	eye.GenerateCenterPoint();  
	VECTOR4  pos(0, 0, 0, 1);
	Object *object = new Object(); 
	pipeline.CreateObject("cube001", "cube", &pos); 
	pos.y = 0;
	pipeline.CreateObject("sphere", "sphere", &pos);
	pipeline.CreateObject("floor", "cube", &pos);
	pipeline.LoadPicture();
	pipeline.RenderAll();  
}  
float camRot = 0;
float camX = 0, camY = 0, camZ = -10;
void GlKeyCall(unsigned char key, int xmouse, int ymouse) { 
	switch (key) {
	case 'e':
		camRot += 3;
		pipeline.m_directionLight.SetRotation(1, 0, 0, camRot);
		break;

	case 'q':
		camRot -= 3;
		pipeline.m_directionLight.SetRotation(1, 0, 0, camRot);
		break;
	case 's':
		camY += 0.3;
		pipeline.m_directionLight.SetPosition(camX, camY, camZ);
		break;

	case 'a':
		camX -= 0.3;
		pipeline.m_directionLight.SetPosition(camX, camY, camZ);
		break;
	case 'z':
		camZ -= 0.3;
		pipeline.m_directionLight.SetPosition(camX, camY, camZ);
		break;
	case 'c':
		camZ += 0.3;
		pipeline.m_directionLight.SetPosition(camX, camY, camZ);
		break;
	case 'w':
		camY -= 0.3;
		pipeline.m_directionLight.SetPosition(camX, camY, camZ);
		break;

	case 'd':
		camX += 0.3;
		pipeline.m_directionLight.SetPosition(camX, camY, camZ);
		break;
	case 'm':
		pipeline.showShadowMap = !pipeline.showShadowMap;
		break;
	} 
}
float  t = 0;
void Mainloop() {
	t += 5;
	Object* cube = pipeline.GetObject("cube001");
	Object* floor = pipeline.GetObject("floor");
	Object* sphere = pipeline.GetObject("sphere");
	floor->SetScale(1, 0.5, 1);
	sphere->SetRotation(0,1 ,0,t); 
	sphere->SetPosition(0, 0, -2,1); 
	floor->SetPosition(0, 0, 5, 1);
	floor->SetRotation(0, 1, 0, t);
	//cube->SetRotation(0, 1, 0,  t);
	cube->SetPosition(sin(t* 3.141592657/180) *1, cos(t* 3.141592657 / 180) * 1, 3,  t);
	VECTOR4 col;
	glClear(GL_COLOR_BUFFER_BIT);
	glBegin(GL_POINTS);
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++) {
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
	width = 800; 
	height = 800; 
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
	StartRender(); 
	glutIdleFunc(Mainloop);
	glutKeyboardFunc(GlKeyCall);
	glutMainLoop();
	return 0; 
}
