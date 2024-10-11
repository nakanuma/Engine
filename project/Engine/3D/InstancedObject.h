#pragma once
#include "Object3D.h"
#include "StructuredBuffer.h"

class InstancedObject : public Object3D
{
public:
	InstancedObject();

	/// <summary>
	/// 描画
	/// </summary>
	void InstancedDraw();

	StructuredBuffer<Object3D::TransformationMatrix> gTransformationMatrices = StructuredBuffer<Object3D::TransformationMatrix>(0, true);
};

