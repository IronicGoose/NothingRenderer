#pragma once
#include<math.h>
namespace VECTOR {

	struct VECTOR4 {
		float x, y, z, w;
		VECTOR4::VECTOR4(float r,float g, float b ) {
			x = r; y = g; z = b; w = 0;
		}
		VECTOR4::VECTOR4(float r, float g, float b,float a ) {
			x = r; y = g; z = b; w = a;
		}
		VECTOR4::VECTOR4(){}
	};
	struct VECTOR2 {
		float x, y;
		VECTOR2::VECTOR2(float a ,float b) {
			x = a; y = b;
		}
		VECTOR2::VECTOR2(){}
	};
	
	void normalized(VECTOR2* n) {
		float abs =	sqrt( n->x* n->x + n->y * n->y );
		n->x = n->x / abs; 
		n->y = n->y / abs;
	}
	float distance(VECTOR2* a , VECTOR2* b) {
		return sqrt((a->x - b->x) * (a->x - b->x) + (a->y - b->y)*(a->y - b->y));
	}

	float dot(VECTOR4* a, VECTOR4* b )  { 
		
		return a->x *b->x + a->y * b->y + a->z * b->z ;
	}
	float matrixdot(VECTOR4* a, VECTOR4* b) { 
		return a->x *b->x + a->y * b->y + a->z * b->z + a->w * b->w;
	}
	void copy(VECTOR4* from, VECTOR4* to) {
		to->x = from->x;
		to->y = from->y;
		to->z = from->z;
		to->w = from->w;
	}
	VECTOR4* dot(VECTOR4*res, VECTOR4* a, float b) { 
		res->x = a->x * b; 
		res->y = a->y * b;  
		res->z = a->z * b;
		res->w = a->w * b;
		return res; 
	}

	VECTOR4* add(VECTOR4*res, VECTOR4* a, VECTOR4* b) { 
		res->x = a->x + b->x;
		res->y = a->y + b->y;
		res->z = a->z + b->z;
		res->w = a->w ;
		return res;

	}

	VECTOR4* minus(VECTOR4*res, VECTOR4* a, VECTOR4* b) { 
		res->x = a->x - b->x;
		res->y = a->y - b->y;
		res->z = a->z - b->z;
		res->w = a->w ;
		return res;

	}
	VECTOR4* normalized(VECTOR4*res, VECTOR4* vec) {
		float abs = dot(vec, vec);
		abs = sqrt(abs);
		abs = 1 / abs;
		res->x = vec->x * abs;
		res->y = vec->y * abs;
		res->z = vec->z * abs;
		return res;
	}
}
namespace MATRIX {
	using namespace VECTOR;
	
	struct MATRIX4x4 { 
		float val[4][4];  
		MATRIX4x4() {
			for (int i = 0; i < 4; i++) { 
				for (int j = 0; j < 4; j++) {
					val[i][j] = 0;
				}
			}
			
		}  
	};
	// 统一使用左乘形式，右乘为特殊使用时再开启 
	/*
	VECTOR4*  matrixdot( MATRIX4x4 *matrix, VECTOR4* col) {
		float x = col->x, y = col->y, z = col->z, w = col->w;
		VECTOR4 * res = new VECTOR4();
		res->x = matrix->val[0][0] * x + matrix->val[0][1] * y + matrix->val[0][2] * z + w * matrix->val[0][3];
		res->y = matrix->val[1][0] * x + matrix->val[1][1] * y + matrix->val[1][2] * z + w* matrix->val[1][3];
		res->z = matrix->val[2][0] * x + matrix->val[2][1] * y + matrix->val[2][2] * z + w* matrix->val[2][3];
		res->w = matrix->val[3][0] * x + matrix->val[3][1] * y + matrix->val[3][2] * z + w * matrix->val[3][3];
		return res;
	} 
	*/
	 
	VECTOR4*  matrixdot( VECTOR4* res,VECTOR4* col, MATRIX4x4* matrix) {
		float x = col->x, y = col->y, z = col->z, w = col->w; 
		res->x = matrix->val[0][0] * x + matrix->val[1][0] * y + matrix->val[2][0] * z + matrix->val[3][0] * w;
		res->y = matrix->val[0][1] * x + matrix->val[1][1] * y + matrix->val[2][1] * z + matrix->val[3][1] * w;
		res->z = matrix->val[0][2] * x + matrix->val[1][2] * y + matrix->val[2][2] * z + matrix->val[3][2] * w;
		res->w = matrix->val[0][3] * x + matrix->val[1][3] * y + matrix->val[2][3] * z + matrix->val[3][3] * w; 
		return res;
	}
	void GenerateTransformMatrix(VECTOR4* pos, MATRIX4x4* ma) {
		ma->val[3][0] = pos->x;
		ma->val[3][1] = pos->y;
		ma->val[3][2] = pos->z;
		ma->val[3][3] = 1;
		ma->val[2][2] = 1;
		ma->val[1][1] = 1;
		ma->val[0][0] = 1;
	}
}
