/*
	AUTHOR: RICH DAVISON +
	KIYAVASH KDNAR WHERE SPECIFIED
*/
#pragma once

#include "Window.h"
#include "Matrix4.h"
#include "Vector3.h"
#include "../ResourceManagment/Resource.h"
#include "SceneNode.h"

class Camera : public Resource
{
public:
	Camera(void) : Resource()
	{
		yaw = 0.0f;
		pitch = 0.0f;

		//	CSC3224 NCODE [Kiyavash Kandar] [140245239]
		this->SetSizeInBytes(sizeof(*this));
		//	CSC3224 NCODE BLOCK ENDS
	};

	Camera(float pitch, float yaw, Vector3 position) : Resource()
	{
		this->pitch = pitch;
		this->yaw = yaw;

		SetPosition(position);

		//	CSC3224 NCODE [Kiyavash Kandar] [140245239]
		this->SetSizeInBytes(sizeof(*this));
		//	CSC3224 NCODE BLOCK ENDS
	}

	~Camera() {}

	void UpdateCamera(float msec = 10.0f);

	//Builds a view matrix for the current camera variables, suitable for sending straight
	//to a vertex shader (i.e it's already an 'inverse camera matrix').
	Matrix4 BuildViewMatrix();

	//Gets position in world space
	Vector3 GetPosition() const
	{
		return node.GetTransform().GetPositionVector();
	}

	//Sets position in world space
	void SetPosition(Vector3 val)
	{
		node.SetTransform(node.GetTransform() * Matrix4::Translation(val));
	}

	SceneNode* GetSceneNode()
	{
		return &node;
	}

	//Gets yaw, in degrees
	float	GetYaw()   const
	{
		return yaw;
	}
	//Sets yaw, in degrees
	void	SetYaw(float y)
	{
		yaw = y;
	}

	//Gets pitch, in degrees
	float	GetPitch() const
	{
		return pitch;
	}
	//Sets pitch, in degrees
	void	SetPitch(float p)
	{
		pitch = p;
	}

	//	CSC3224 NCODE [Kiyavash Kandar] [140245239]
	void Read(const string resourcename) override;
	void ReadParams(const string params) override;
	//	CSC3224 NCODE BLOCK ENDS

protected:
	SceneNode node;
	float	yaw;
	float	pitch;
};