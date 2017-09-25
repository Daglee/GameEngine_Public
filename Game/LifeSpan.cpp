#include "LifeSpan.h"

const float NO_HEALTH = 0.0f;

LifeSpan::LifeSpan(const float maxHealth, const int numberOfFramesUntilRespawn)
{
	this->maxHealth = maxHealth;
	this->numberOfFramesUntilRespawn = numberOfFramesUntilRespawn;

	currentHealth = maxHealth;
	framesSpentDead = 0;
}

LifeSpan::LifeSpan()
{
	maxHealth = 0;
	numberOfFramesUntilRespawn = 0;
	currentHealth = 0;
	framesSpentDead = 0;
}

const bool LifeSpan::IsDead() const
{
	return currentHealth <= NO_HEALTH;
}

const bool LifeSpan::TimeToRespawn() const
{
	return framesSpentDead == numberOfFramesUntilRespawn;
}

const bool LifeSpan::HasNotBegunRespawn() const
{
	return framesSpentDead == 0;
}

void LifeSpan::IncrementFramesSpentDead()
{
	++framesSpentDead;
}

void LifeSpan::ResetHealth()
{
	currentHealth = maxHealth;
}

void LifeSpan::ResetFramesSpentDead()
{
	framesSpentDead = 0;
}
