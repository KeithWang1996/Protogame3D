#pragma once
#include "Game/Entity.hpp"
class Texture;
class Player : public Entity
{
public:
	Player(const Vec2& position);
	virtual void Update(float deltaSeconds) override;
	virtual void Render() const override;
	void UpdateFromJoystick(float deltaSeconds);

	int m_controllerID = -1;
private:
	Texture* m_texture;
};