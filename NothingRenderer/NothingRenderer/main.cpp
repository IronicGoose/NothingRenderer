﻿#include <iostream>
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
	pipeline.CreateObject("sphere", "sphere", &pos);
	pipeline.LoadPicture();
	pipeline.RenderAll(); 
//	pipeline.RenderTarget(*object);
	
}  

float  t = 0;
void Mainloop() {
	t += 5;
	Object* cube = pipeline.GetObject("cube001");
	Object* sphere = pipeline.GetObject("sphere");
	sphere->SetRotation(1,1 ,1,t); 
	cube->SetRotation(1, 1, 1,  t);
	cube->SetPosition(sin(t* 3.141592657/180) *1, cos(t* 3.141592657 / 180) * 1, 1,  t);
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
