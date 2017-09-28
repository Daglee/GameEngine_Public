#pragma once
/*
	AUTHOR: RICH DAVISON
*/

#include "../Maths/Plane.h"
#include "../Maths/Matrix4.h"
#include "../Meshes/SceneNode.h"
class Matrix4; // Compile Mat4 class first

class Frustum
{
public:
	Frustum(void) {};
	~Frustum(void) {};

	void FromMatrix(const Matrix4 &mvp);
	bool InsideFrustum(SceneNode &n);

protected:
	Plane planes[6];
};

