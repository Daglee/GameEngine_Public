#pragma once

class Player;
class Gamepad;
class Mesh;

struct BasePlayerSystems
{
	Mesh* defaultBulletMesh;
	Mesh* defaultPlayerMesh;
};

class PlayerConfiguration
{
public:
	PlayerConfiguration(BasePlayerSystems playerComponents);
	~PlayerConfiguration();

	void InitialisePlayer(Player* player, int playerNumber);

private:
	void SetBasicProperties(Player* player, int playerNumber);
	void SetGameplayComponents(Player* player);

	BasePlayerSystems playerComponents;
};

