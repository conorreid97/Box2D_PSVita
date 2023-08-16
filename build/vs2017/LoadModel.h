#pragma once
#include <graphics/mesh_instance.h>
#include "graphics/scene.h"
namespace gef
{
	class Platform;
	class SpriteRenderer;
	class Font;
	class InputManager;
	class Renderer3D;
}


class LoadModel
{
public:
	void Init();

	gef::Scene* LoadSceneAssets(gef::Platform& platform, const char* filename);
	gef::Mesh* GetMeshFromSceneAssets(gef::Scene* scene);
};

