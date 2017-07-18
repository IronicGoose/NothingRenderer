#pragma once
#include"ObjectInfo.h"
#include"camera.h"
#include"Vector.h"
 

class PipelineController {
	vector<Object> m_renderTargets;
public :
	PipelineController() {

	}
	void RenderAll() {

	}
	void AddRenderTarget(Object ob) {
		auto i = m_renderTargets.begin();
		while (i != m_renderTargets.end())
		{
			if (i->objectName == ob.objectName) { 
				return;
			}
		}
		m_renderTargets.push_back(ob);
	}
	bool RemoveRenderTarget(Object ob) {
		auto i = m_renderTargets.begin();
		while (i != m_renderTargets.end())
		{
			if (i->objectName == ob.objectName) {
				m_renderTargets.erase(i);
				return true;
			}
		}
		return false; 
	}
};