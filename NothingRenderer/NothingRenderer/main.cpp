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
 

void RenderACube() {
//	eye.GenerateCenterPoint();  
	VECTOR4  pos(0, 0, 4, 1);
	Object *object = new Object(); 
	pipeline.CreateObject("cube001", "cube", &pos);
	pos.y = 4;
	pipeline.CreateObject("cube002", "cube", &pos);
	pipeline.LoadPicture();
	pipeline.RenderAll(); 
//	pipeline.RenderTarget(*object);
	
}  

float  t = 0;
void Mainloop() {
	t += 0.05;
	Object* ob = pipeline.GetObject("cube001");
	Object* ob2 = pipeline.GetObject("cube002");
	ob2->SetRotation(1, 1, 1,55 );
	ob2->SetPosition(0, 0, 0, 1);
	ob->SetRotation(0, 1, 0, t * 50);
	ob->SetPosition(sin(t) *2,cos(t) *2 ,3 , 1);
	//pipeline.lightDir.z = -sin(t /180 * 3.1415926); pipeline.lightDir.y = -cos(t / 180 * 3.1415926);
	//normalizedVector3(&pipeline.lightDir, &pipeline.lightDir);
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
	RenderACube(); 
	glutIdleFunc(Mainloop);
	glutMainLoop();
	return 0; 
}
