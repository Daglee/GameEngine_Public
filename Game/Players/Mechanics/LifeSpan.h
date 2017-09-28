#pragma once
class LifeSpan
{
public:
	LifeSpan(const float maxHealth, const int numberOfFramesUntilRespawn);
	LifeSpan();
	~LifeSpan() {}

	const bool IsDead() const;
	const bool TimeToRespawn() const;
	const bool HasNotBegunRespawn() const;

	void IncrementFramesSpentDead();
	void ResetHealth();
	void ResetFramesSpentDead();

	float* GetCurrentHealth()
	{
		return &currentHealth;
	}

private:
	float maxHealth;
	float currentHealth;

	int numberOfFramesUntilRespawn;
	int framesSpentDead;
};

