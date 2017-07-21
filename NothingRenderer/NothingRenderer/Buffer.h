#pragma once
#include"Vector.h"
using namespace VECTOR;
class Buffer
{
	int m_width, m_height;
	float* m_ZBuffer;
	bool m_isSwap = false;
	VECTOR::VECTOR4 * m_frontColorBuffer,*m_backColorBuffer;
public:
	Buffer();
	~Buffer();
	int GetWidth() { return m_width;  }
	int GetHeith() { return m_height; } 
	void SetWidthHeight(int width, int height) { m_width = width; m_height = height; }
	void CreateBuffer(VECTOR4 * col) { 

		m_frontColorBuffer = new VECTOR4[m_width * m_height];
		m_backColorBuffer = new VECTOR4[m_width * m_height];
		m_ZBuffer = new float[m_width * m_height];
		for (int i = 0; i < m_width * m_height; i++) {
			copy(col, &m_frontColorBuffer[i]);
			copy(col, &m_backColorBuffer[i]);
			m_ZBuffer[i] = -99;
		}
	}
	VECTOR4* GetWriteBuffer() {
		if (m_isSwap) {
			return m_backColorBuffer;
		}
		else
			return m_frontColorBuffer;
	}
	VECTOR4* GetReadBuffer() {
		if (!m_isSwap) {
			return m_backColorBuffer;
		}
		else
			return m_frontColorBuffer;
	}
	void ClearColBuffer(VECTOR4& col) { 
		for (int i = 0; i < m_width * m_height; i++) {
			copy(&col, &GetWriteBuffer()[i]);
			m_ZBuffer[i] = -99;
		}
	}

	void WriteToZBuffer(float v,int x ,int y ) { 
		int index = y* m_width + x;
		m_ZBuffer[index] = v;
	}

	void WriteToColorBuffer(int x, int y, VECTOR4 * col) {
		int index = y* m_width + x; 
		copy(col, &GetWriteBuffer()[index]);
	}
	float  GetZBufferValue(int x, int y) { 
		int index = y* m_width + x;
		//return -20;
		return m_ZBuffer[index];
	}
	const VECTOR4&  GetColorBufferValue(int x, int y) {
		int index = y* m_width + x;
		return GetReadBuffer()[index];
	}
	void SwapBuffer() {
		m_isSwap = !m_isSwap;
	}
	void Antialising();
}; 

void Buffer::Antialising() {



}

Buffer::Buffer()
{ 
}

Buffer::~Buffer()
{
	delete[] m_ZBuffer;
	delete[] m_frontColorBuffer;
	delete[] m_backColorBuffer;
}