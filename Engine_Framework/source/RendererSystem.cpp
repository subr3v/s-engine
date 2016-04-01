#include "RendererSystem.h"
#include "MeshComponent.h"
#include "MeshRenderer.h"
#include "Entity.h"
#include "EntitySet.h"
#include "DrawCallParameters.h"
#include "ConstantBufferTypes.h"
#include "LightComponents.h"
#include "CameraComponent.h"
#include "ForwardPlusRenderer.h"
#include "ConfigVariable.h"

FConfigVariable AmbientIntensity("Forward+", "Ambient Intensity", 0.3f);

FRendererSystem::FRendererSystem(FForwardPlusRenderer* Renderer) : Renderer(Renderer)
{
	AmbientLight.Colour = glm::vec3(1.0f);
}

FRendererSystem::~FRendererSystem()
{

}

void FRendererSystem::Draw(FDrawCall Params)
{
	for (FEntity* Entity : ModelEntities->Get())
	{
		auto Transform = Entity->GetComponent<FTransformComponent>();
		auto Mesh = Entity->GetComponent<FMeshComponent>();
		
		if (Mesh->bVisible == false || Mesh->Mesh == nullptr)
			continue;

		Params.InstanceData->SetWorld(Transform->Transform.ToMatrix());
		FMeshRenderer::Render(Mesh->Mesh, Params, Mesh->bTessellatedMesh);
	}
}

void FRendererSystem::PreRender()
{
	// Gather lights.
	for (FEntity* Entity : PointLightEntities->Get())
	{
		PointLights.Add(Entity->GetComponent<FPointLightComponent>()->Light);
	}

	for (FEntity* Entity : SpotLightEntities->Get())
	{
		SpotLights.Add(Entity->GetComponent<FSpotLightComponent>()->Light);
	}

	for (FEntity* Entity : DirectionalLightEntities->Get())
	{
		DirectionalLights.Add(Entity->GetComponent<FDirectionalLightComponent>()->Light);
	}
}

void FRendererSystem::Render()
{
	for (FEntity* Entity : CameraEntities->Get())
	{
		auto CameraComponent = Entity->GetComponent<FCameraComponent>();
		if (CameraComponent->bIsActive)
		{
			Renderer->DrawScene(CameraComponent->Camera, this);
		}
	}
}

void FRendererSystem::Update(float DeltaTime)
{
	AmbientLight.Intensity = AmbientIntensity.AsFloat();
	Renderer->Update(DeltaTime);
}

void FRendererSystem::OnAdded()
{
	PointLightEntities = GetEntitySet({ TYPE_OF(FPointLightComponent) });
	SpotLightEntities = GetEntitySet({ TYPE_OF(FSpotLightComponent) });
	DirectionalLightEntities = GetEntitySet({ TYPE_OF(FDirectionalLightComponent) });
	CameraEntities = GetEntitySet({ TYPE_OF(FCameraComponent) });
	ModelEntities = GetEntitySet({ TYPE_OF(FMeshComponent), TYPE_OF(FTransformComponent) });
}

void FRendererSystem::PostRender()
{
	// Clear light data.
	PointLights.Clear();
	DirectionalLights.Clear();
	SpotLights.Clear();
}

void FRendererSystem::SetAmbientLight(const FAmbientLight& AmbientLight)
{
	this->AmbientLight = AmbientLight;
}

FForwardPlusRenderer* FRendererSystem::GetRenderer()
{
	return Renderer;
}
