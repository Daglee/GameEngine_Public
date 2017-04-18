#include "Player.h"

#include "../ResourceManagment/DataBase.h"
#include "../UISystem/InputMapper.h"
#include "../Physics/PhysicsEngine.h"
#include "../Physics/RigidBody.h"
#include "../../nclgl/Renderer.h"
#include "../GameLogicFSM/MessageSystem.h"

#define PLAYER_MASS 500.0f
#define PLAYER_DRAG 0.7f
#define PLAYER_GRAVITY -0.001f

Player::Player() : ResourceBase()
{
	//Load in a character model
	playerModel = new CharacterModel(CHARCT_MODEL_LOC);
	
	//Just some default values in case game logic fails
	playerModel->SetTransform(Vector3(0, 0, 0)); 
	playerModel->UpdateColour(Vector4(1, 1, 0, 1));

	rigidBody = RigidBody(Vector3(), Vector3(), 
		PLAYER_MASS, PLAYER_DRAG, PLAYER_GRAVITY);
	rigidBody.isStatic = false;
	rigidBody.atRest = false;
	rigidBody.parentMesh = playerModel;

	SphereCollider* rb = new SphereCollider(playerModel->body.GetBoundingRadius()
	+ (playerModel->leftArm.GetBoundingRadius() + playerModel->rightArm.GetBoundingRadius()));
	rigidBody.collider = rb;
	rigidBody.collider->position = rigidBody.lastPosition;

	//For the game logic FSM
	vars = new std::unordered_map<std::string, float*>;
	vars->insert({ "colourr", &colourr });
	vars->insert({ "colourg", &colourg });
	vars->insert({ "colourb", &colourb });
	vars->insert({ "coloura", &coloura });
	vars->insert({ "timer", &timer });
	vars->insert({ "collider", &collider });

	this->SetResourceSize(sizeof(*this));
}

Player::~Player()
{
	delete playerModel;
}


void Player::ApplyInputs()
{
	float rotationDegrees = input->GetRotation();

	Matrix4 rot = Matrix4::Rotation(rotationDegrees, Vector3(0, 1, 0));
	playerModel->SetTransform(playerModel->GetTransform() * rot);
	if (rotationDegrees != 0) {
		MessageSystem::GetInstance()->Transmit(Log::Hash("player_rotated"));
	}

	Vector3 movement = input->GetMovement() * 30;

	rigidBody.ApplyMomentum(movement);

	if (movement.x != 0 || movement.y != 0 || movement.z != 0) {
		AudioManager::GetInstance()->BeginPlay(walkingSoundName);
		MessageSystem::GetInstance()->Transmit(Log::Hash("player_moved"));
	}
	else AudioManager::GetInstance()->StopPlay(walkingSoundName);

	if (input->Fired()) {
		if (gun->Fire(rigidBody.lastPosition, input->rawRotation, teamID)) {
			Sound* shoot = new Sound("../Data/Sounds/14615__man__canon.wav"); //AudioManager will delete this.
			AudioManager::GetInstance()->TemporaryPlay(shoot, SOUNDPRIORITY_MEDIUM);
		}
		MessageSystem::GetInstance()->Transmit(Log::Hash("player_fired"));
	}
}

void Player::Move(const Vector3& pos) 
{
	rigidBody.UpdatePosition(pos);
}

void Player::Update(const float& msec, const float& timer) 
{
	++this->timer;

	playerModel->UpdateColour(Vector4(colourr, colourg, colourb, coloura));
	playerModel->Update(msec);
}

void Player::Read(string resourcename) 
{
	this->SetResourceName(resourcename);
}

void Player::ReadParams(string params) 
{
	Read(params);
}