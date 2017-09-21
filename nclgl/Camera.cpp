#include "Camera.h"

/*
Polls the camera for keyboard / mouse movement.
Should be done once per frame! Pass it the msec since
last frame (default value is for simplicities sake...)
*/
void Camera::UpdateCamera(float msec)
{
	//Bounds check the pitch, to be between straight up and straight down ;)
	pitch = min(pitch, 90.0f);
	pitch = max(pitch, -90.0f);

	if (yaw < 0)
	{
		yaw += 360.0f;
	}
	if (yaw > 360.0f)
	{
		yaw -= 360.0f;
	}
}

/*
Generates a view matrix for the camera's viewpoint. This matrix can be sent
straight to the shader...it's already an 'inverse camera' matrix.
*/
Matrix4 Camera::BuildViewMatrix()
{
	//Why do a complicated matrix inversion, when we can just generate the matrix
	//using the negative values ;). The matrix multiplication order is important!
	return	Matrix4::Rotation(-pitch, Vector3(1, 0, 0)) *
		Matrix4::Rotation(-yaw, Vector3(0, 1, 0)) *
		Matrix4::Translation(-node.GetTransform().GetPositionVector());
};

void Camera::Read(string resourcename)
{
	this->SetName(resourcename);
}

void Camera::ReadParams(string params)
{
	std::istringstream iss(params);
	vector<string> tokens{ istream_iterator<string>{iss},
		istream_iterator<string>{} };

	string name = tokens.at(0);
	float pitch = std::stof(tokens.at(1));
	float yaw = std::stof(tokens.at(2));
	float xPos = std::stof(tokens.at(3));
	float yPos = std::stof(tokens.at(4));
	float zPos = std::stof(tokens.at(5));

	this->pitch = pitch;
	this->yaw = yaw;
	this->SetPosition(Vector3(xPos, yPos, zPos));

	this->SetName(name);
	this->SetSize(sizeof(*this));
}