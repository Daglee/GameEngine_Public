#include "CharacterModel.h"

#include "../../nclgl/OBJMesh.h"
#include "../ResourceManagment/Log.h"
#include "../GameLogicFSM/MessageSystem.h"

#include <istream>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>

/*
  Dimensions for different scenenodes for this character model
  are read from the passed in file.
*/
CharacterModel::CharacterModel(string playertag, string filename)
{
	body = SceneNode(nullptr, Vector4());
	head = SceneNode(nullptr, Vector4());

	dleftArm = SceneNode(nullptr, Vector4());
	uleftArm = SceneNode(nullptr, Vector4());

	drightArm = SceneNode(nullptr, Vector4());
	urightArm = SceneNode(nullptr, Vector4());

	leftLeg = SceneNode(nullptr, Vector4());
	rightLeg = SceneNode(nullptr, Vector4());

	gun = SceneNode(nullptr, Vector4());

	nodeMap["body"] = &body;
	nodeMap["head"] = &head;
	nodeMap["uleftArm"] = &uleftArm;
	nodeMap["dleftArm"] = &dleftArm;
	nodeMap["urightArm"] = &urightArm;
	nodeMap["drightArm"] = &drightArm;
	nodeMap["leftLeg"] = &leftLeg;
	nodeMap["rightLeg"] = &rightLeg;
	nodeMap["gun"] = &gun;

	ReadFile(filename);

	AddChild(&body);
	body.AddChild(&head);
	body.AddChild(&uleftArm);
	uleftArm.AddChild(&dleftArm);
	body.AddChild(&urightArm);
	urightArm.AddChild(&drightArm);
	body.AddChild(&leftLeg);
	body.AddChild(&rightLeg);
	body.AddChild(&gun);

	for each (std::pair<std::string, SceneNode*> nodepair in nodeMap)
	{
		modelParts.push_back(nodepair.second);
		nodepair.second->AutoSetBoundingRadius();
	}

	this->playertag = playertag;
}

CharacterModel::CharacterModel()
{
	body = SceneNode(nullptr, Vector4());
	head = SceneNode(nullptr, Vector4());

	dleftArm = SceneNode(nullptr, Vector4());
	uleftArm = SceneNode(nullptr, Vector4());

	drightArm = SceneNode(nullptr, Vector4());
	urightArm = SceneNode(nullptr, Vector4());

	leftLeg = SceneNode(nullptr, Vector4());
	rightLeg = SceneNode(nullptr, Vector4());

	gun = SceneNode(nullptr, Vector4());

	nodeMap["body"] = &body;
	nodeMap["head"] = &head;
	nodeMap["uleftArm"] = &uleftArm;
	nodeMap["dleftArm"] = &dleftArm;
	nodeMap["urightArm"] = &urightArm;
	nodeMap["drightArm"] = &drightArm;
	nodeMap["leftLeg"] = &leftLeg;
	nodeMap["rightLeg"] = &rightLeg;
	nodeMap["rightLeg"] = &rightLeg;
	nodeMap["gun"] = &gun;

	AddChild(&body);
	body.AddChild(&head);
	body.AddChild(&uleftArm);
	uleftArm.AddChild(&dleftArm);
	body.AddChild(&urightArm);
	urightArm.AddChild(&drightArm);
	body.AddChild(&leftLeg);
	body.AddChild(&rightLeg);
	body.AddChild(&gun);
}

/*
  File should have sections in this format
  for each node.
  Example in ../../CharacterModel/PlayerModel.txt

  line 0: node name			-- head
  line 1: node colour		-- colour 1 0 1 1
  line 2: node scale		-- scale 5 23 1
  line 3: node transform	-- transform 0 52 12
*/
void CharacterModel::ReadFile(string filename)
{
	std::ifstream file(filename);
	string line;

	for (int i = 0; i < 9; ++i)
	{
		getline(file, line);
		SceneNode* node;

		auto it = nodeMap.find(line.c_str()); //Find the pointer to the correct node
		if (it == nodeMap.end())
		{
			//Probably a typo in the file if its in the unordered_map
			Log::Error("Node " + line + " not found in map.");
		}

		node = it->second;

		vector<vector<string>> tokens;

		getline(file, line);
		while (line != "-")
		{
			vector<string> lineTokens = Log::tokenise(line);
			tokens.push_back(lineTokens);

			getline(file, line);
		}

		vector<string> colourLine = tokens.at(0);
		Vector4 colour(stof(colourLine.at(1)), stof(colourLine.at(2)),
			stof(colourLine.at(3)), stof(colourLine.at(4)));

		vector<string> scaleLine = tokens.at(1);
		Vector3 scale(stof(scaleLine.at(1)), stof(scaleLine.at(2)),
			stof(scaleLine.at(3)));

		vector<string> transformLine = tokens.at(2);
		Vector3 transform(stof(transformLine.at(1)), stof(transformLine.at(2)),
			stof(transformLine.at(3)));

		vector<Matrix4> rotations;
		if (tokens.size() > 3)
		{
			for (int i = 3; i < tokens.size(); ++i)
			{
				vector<string> rotationLine = tokens.at(i);
				float degrees = stof(rotationLine.at(1));
				Vector3 axis(stof(rotationLine.at(2)), stof(rotationLine.at(3)),
					stof(rotationLine.at(4)));

				rotations.push_back(Matrix4::Rotation(degrees, axis));
			}
		}

		node->SetColour(colour);
		node->SetModelScale(scale);
		node->SetTransform(Matrix4::Translation(transform));

		for each (Matrix4 rotation in rotations)
		{
			node->SetTransform(node->GetTransform() * rotation);
		}
	}

	file.close();
}

//Applies to all nodes
void CharacterModel::UpdateMesh(Mesh* m) const
{
	for each (std::pair<std::string, SceneNode*> nodepair in nodeMap)
	{
		nodepair.second->SetMesh(m);
	}
}

//Applies to all nodes
void CharacterModel::UpdateColour(const Vector4& col) const
{
	for each (std::pair<std::string, SceneNode*> nodepair in nodeMap)
	{
		if (nodepair.first != "gun") nodepair.second->SetColour(col);
	}
}

//For animations (if any are added in the future...
void CharacterModel::Update(const float& msec)
{
	SceneNode::Update(msec);

	//Move the legs if the player is moving!
	if (MessageSystem::GetInstance()->MessageTransmitting(Log::Hash(playertag + "moving")))
	{
		leftLegRotation = std::sinf(animFrames) * 30;
		leftLeg.SetTransform(leftLeg.GetTransform().Translation(leftLeg.GetTransform().GetPositionVector()) *
			Matrix4::Rotation(leftLegRotation, Vector3(1, 0, 0)));

		rightLegRotation = -std::sinf(animFrames) * 30;
		rightLeg.SetTransform(rightLeg.GetTransform().Translation(rightLeg.GetTransform().GetPositionVector()) *
			Matrix4::Rotation(rightLegRotation, Vector3(1, 0, 0)));

		animFrames = animFrames + (msec / 100);// 0.05f;
		MessageSystem::GetInstance()->StopEvent(Log::Hash(playertag + "moving"));

		animReset = false;
	}
	//otherwise reset back to the original position.
	else if (!animReset)
	{
		leftLeg.SetTransform(leftLeg.GetTransform() *
			Matrix4::Rotation(-leftLegRotation, Vector3(1, 0, 0)));
		leftLegRotation = 0;

		rightLeg.SetTransform(rightLeg.GetTransform() *
			Matrix4::Rotation(-rightLegRotation, Vector3(1, 0, 0)));
		rightLegRotation = 0;

		animReset = true;
	}

}
