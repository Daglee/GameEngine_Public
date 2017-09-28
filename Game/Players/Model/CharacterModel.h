#pragma once

#include "../../nclgl/Meshes/SceneNode.h"

#include <string>
#include <unordered_map>
#include <vector>

class OBJMesh;

/*
  Robot character model for players and NPCs
   - No textures
   - Colour can be applied (for teams)
   - Animations can be updated
  Uses multiple scenenodes.
*/
class CharacterModel : public SceneNode
{
public:
	CharacterModel(string playertag, string filename);
	CharacterModel();
	~CharacterModel() {}

	void UpdateMesh(Mesh* m) const;
	void UpdateColour(const Vector4& col) const;
	void ReadFile(string file);
	virtual void Update(const float& msec);

	std::vector<SceneNode*> modelParts;

	SceneNode body;
	SceneNode head;
	SceneNode dleftArm;
	SceneNode uleftArm;
	SceneNode drightArm;
	SceneNode urightArm;
	SceneNode leftLeg;
	SceneNode rightLeg;
	SceneNode gun;

	string playertag;
protected:
	SceneNode nodes[6];
	unordered_map<string, SceneNode*> nodeMap;

	float leftLegRotation	= 0;
	float rightLegRotation	= 0;
	float animFrames		= 0;

	bool animReset = true;
};

