#pragma once
#include"Vector.h"
#include<vector>
using namespace std;
using namespace VECTOR;
using namespace MATRIX;
struct FACEINFO {
	VECTOR4 a;
	VECTOR4 b;
	VECTOR4	c;
};
class GEOMETRY
{
public :
	string name;
	vector<VECTOR4*> v; 
	vector<VECTOR4*> n;
	vector<FACEINFO*> f;
	int vertCount;
	int faceCount; 
	int normalCount;
	GEOMETRY() { 
	}
	~GEOMETRY() { 
	}
};
class  VERT {
public :
	
	VECTOR4 * position;
	VECTOR4 * normal;
	VECTOR4 * color; 
	VERT::VERT() {
		position = new VECTOR4();
		normal = new VECTOR4();
		color = new VECTOR4();
		color->x = 0; color->y = 0; color->z = 0; color->w = 1;
		normal->w = 0;
		position->w = 1;
	}
};
class Object {
public :
	const GEOMETRY* prefab;
	VERT* verts;
	int vertCount;
	string objectName;
	VECTOR4 * position;
	VECTOR4 * rotation;
	void SetRotation(float x, float y, float z , float w ) { 
		float qr = cos(w * 3.1415926 / 360);
		float s = sin(w * 3.1415926 / 360);
		rotation->w = qr; rotation->x = x * s; rotation->y = y * s; rotation->z = z *s;
		normalizedVector4(rotation);
	}
	void SetPosition(float x, float y, float z, float w) { 
		position->x = x; position->y = y; position->z = z; position->w = w;
	}
	Object::Object() {
		position = new VECTOR4();
		rotation = new VECTOR4();
		rotation->x = 0; rotation->y = 0; rotation->z = 0; rotation->w = 1;
	}
	Object::~Object() {
		delete position;
	}
};

class ObjectHolder {
	vector<GEOMETRY> prefabs; 

public:

	void RegistCubeInfo(GEOMETRY* cu) { 
		prefabs.push_back(*cu);
	} 
	const GEOMETRY * CreatePrefab(string objname ,string filePath) {
		string line;

		ifstream objFile(filePath, ifstream::in);
		GEOMETRY* geomery = new GEOMETRY(); 
		int vertCount = 0;
		geomery->faceCount = 0;
		geomery->normalCount = 0;
		while (getline(objFile, line)) {
			stringstream ss(line);
			string token;
			ss >> token;
			// read vertex here 
			if (token == "v") {
				VECTOR4* vec = new VECTOR4();
				ss >> vec->x >> vec->y >> vec->z;
				vec->w = 1; // make it a be  point 
				geomery->v.push_back(vec);
				vertCount++;
			}
			if (token == "vn") {
				VECTOR4* vec = new VECTOR4();
				ss >> vec->x >> vec->y >> vec->z;
				vec->w = 0; // make it a be  point 
				geomery->n.push_back(vec); 
				geomery->normalCount++;

			}
			if (token == "f") {
				FACEINFO * face = new FACEINFO();
				string temp;
				ss >> temp;
				SplitObjString(temp, face->a);
				ss >> temp;
				SplitObjString(temp, face->b);
				ss >> temp;
				SplitObjString(temp, face->c);
//				cout << temp;
				geomery->f.push_back(face);
				geomery->faceCount++;
			}
		}
		geomery->vertCount = vertCount;
		geomery->name = objname;
		objFile.close(); 
		return geomery;
		
	}

	void SplitObjString(const string& s, VECTOR4& out) {

		int i = 0, j = 0;
		int count = 0;
		int res[3];
		for (int i = 0; s[i] != '\0'; i++) {

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
		out.x = res[0];
		out.y = res[1];
		out.z = res[2];
	}
	//once a prefab is created and stored ,it can not be modified again
	const GEOMETRY&  GetPrefab(string objname) { 
		auto it = prefabs.begin();
		while (it != prefabs.end()) {
			if (it->name == objname) {
				return *it;// iterator use *iterator to get the value , use & to get the pointer 
			}
			it++;
		} 
		string filePath;
		if (objname == "cube") {
			filePath = "./Objects/cube.obj";
		}
		if (objname == "sphere") {
			filePath = "./Objects/sphere.obj";
		}
		const GEOMETRY* geometry = CreatePrefab(objname, filePath);
		prefabs.push_back(*geometry);
		return *geometry;
	} 

	Object* CreateObject(string objName,string type ,const VECTOR4 * pos) {
		const GEOMETRY& geo = GetPrefab(type);
		Object * output = new Object();
		
		output->prefab = &geo;

		VERT* vert = new VERT[output->prefab->vertCount]; 
		output->verts = vert;
		output->vertCount = output->prefab->vertCount;
		output->objectName = objName;
		copy(pos, output->position);
		return output;
	} 
	void GetVertsWorldSpace(Object* obj, MATRIX4x4* rotateMatrix ) {
		MATRIX4x4* ma = new MATRIX4x4();   
		GenerateTransformMatrix(obj->position, ma);
		
		for (int i = 0; i < obj->prefab->vertCount; i++) {
			matrixdot(obj->verts[i].position, obj->prefab->v[i],rotateMatrix);
		//	cout<<"Rot" << obj->verts[i].position->x << " " << obj->verts[i].position->y << " " << obj->verts[i].position->z << endl;
			matrixdot(obj->verts[i].position, obj->verts[i].position, ma);
		//	cout << obj->verts[i].position->x <<" "<< obj->verts[i].position->y << " " << obj->verts[i].position->z <<endl;
			/*if (i % 3 == 0) {
				obj->verts[i].color->x = 1; 
			}
			else if (i % 3 == 1) {
				obj->verts[i].color->y = 1;
			}
			else
				obj->verts[i].color->z = 1;*/
			obj->verts[i].color->x = 1;
			obj->verts[i].color->y = 1;
			obj->verts[i].color->z = 1;
		}  
		 
	} 

	void GetVertsWorldSpace(Object* obj, GEOMETRY* prefab ) {
		MATRIX4x4* ma = new MATRIX4x4();
		GenerateTransformMatrix(obj->position, ma);

		for (int i = 0; i < prefab->vertCount; i++) {
			matrixdot(obj->verts[i].position, prefab->v[i], ma);
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
	~ObjectHolder() { 
	} 
	 
};