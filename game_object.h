#ifndef _GAME_OBJECT_H
#define _GAME_OBJECT_H

#include <graphics/mesh_instance.h>
#include <box2d/Box2D.h>

enum OBJECT_TYPE
{
	PLAYER,
	ENEMY,
	END_PLAT,
	COIN,
	PLATFORM
};

class GameObject : public gef::MeshInstance
{
public:
	GameObject() : generation_(0) {}

	void UpdateFromSimulation(const b2Body* body);
	void MyCollisionResponse();
	void DecrementHealth();

	inline void set_type(OBJECT_TYPE type) { type_ = type; }
	inline OBJECT_TYPE type() { return type_; }
	bool alive;
	int health;
	int score;
	int generation_;

private:
	OBJECT_TYPE type_;
};

class Player : public GameObject
{
public:
	Player();
	void Init();
	void DecrementHealth();

};

class Enemy : public GameObject
{
public:
	Enemy();
	void Init();
};

class End_Plat : public GameObject
{
public:
	End_Plat();
};

#endif // _GAME_OBJECT_H