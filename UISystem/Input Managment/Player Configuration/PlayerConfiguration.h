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
	explicit PlayerConfiguration(const BasePlayerSystems playerComponents);
	~PlayerConfiguration();

	void InitialisePlayer(Player* player, const int playerNumber);

private:
	void SetBasicProperties(Player* player, const int playerNumber) const;
	void SetGameplayComponents(Player* player) const;

	BasePlayerSystems playerComponents;
};

