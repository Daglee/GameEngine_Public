#pragma once

#include "../nclgl/Vector3.h"
#include "../nclgl/SceneNode.h"
#include "../nclgl/Mesh.h"
#include "../ResourceManagment/Resource.h"

class Renderer;

/*
  Wrapper class for scene nodes.
  Any objects to be rendered must derive from this.
  No physics aplied to this. GameObjects are static unless
  SetPosition(...) is called.
*/
class GameObject : public Resource
{
public:
	GameObject(Renderer& r);
	GameObject();

	virtual ~GameObject() {}

	virtual void AddToRenderer(Renderer& r);

	void AddMesh(const string& meshLocation);
	void AddMesh(Mesh& m);
	void SetTexture(const char& texloc);
	void SetBumpMap(const char& bmLoc);

	void MakeQuad();

	inline virtual Vector3 GetPosition() const
	{
		return position;
	}

	virtual Vector3 GetSize() const
	{
		return scale;
	}

	SceneNode* GetSceneNode()
	{
		return &node;
	}

	Mesh GetMesh()
	{
		return *node.GetMesh();
	}

	inline virtual void SetPosition(const Vector3& pos)
	{
		position = pos;
		node.SetTransform(pos);
	}

	inline virtual void SetSize(const Vector3& size)
	{
		scale = size;
		node.SetModelScale(scale);
		node.AutoSetBoundingRadius();
	}

	void SetSceneNode(SceneNode& sn)
	{
		node = sn;
	}

	virtual void Read(string resourcename)
	{
		this->SetName(resourcename);
	}

	virtual void ReadParams(string params)
	{
		Read(params);
	}

protected:
	SceneNode node;
	Vector3	scale;
	Vector3	position;
};

