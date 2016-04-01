#include "LuaSceneLoader.h"
#include "LuaDataReader.h"
#include "FileArchive.h"
#include "BaseScene.h"
#include <glm/glm.hpp>
#include "ResourceManager.h"
#include "World.h"
#include "LightComponents.h"
#include "Entity.h"
#include "MeshComponent.h"

static FSpotlight LoadSpotLight(FLuaDataReader& Reader)
{
	FSpotlight Light;

	Reader.GetTable("direction").ReadVector(Light.Direction).Pop();
	Reader.GetTable("position").ReadVector(Light.Position).Pop();
	Reader.GetTable("radius").ReadNumber(Light.Radius).Pop();
	Reader.GetTable("intensity").ReadNumber(Light.Intensity).Pop();
	Reader.GetTable("phi").ReadNumber(Light.Phi).Pop();
	Reader.GetTable("castShadow").ReadBoolean(Light.bCastShadow).Pop();
	Reader.GetTable("colour").ReadVector(Light.Colour).Pop();

	Light.Direction = glm::normalize(Light.Direction);
	return Light;
}


static FPointLight LoadPointLight(FLuaDataReader& Reader)
{
	FPointLight Light;

	Reader.GetTable("position").ReadVector(Light.Position).Pop();
	Reader.GetTable("colour").ReadVector(Light.Colour).Pop();
	Reader.GetTable("intensity").ReadNumber(Light.Intensity).Pop();
	Reader.GetTable("radius").ReadNumber(Light.Radius).Pop();

	return Light;
}

void LoadScene(class FWorld* World, class FResourceGroup* ResourceGroup, const std::string& Filename, bool bLoadPackages)
{
	FLuaState LuaParser;
	if (LuaParser.LoadFile(Filename.c_str()) == false)
	{
		return;
	}

	FLuaDataReader DataReader(LuaParser);
	std::string SceneName;
	DataReader.ReadGlobalString(SceneName, "SceneName");

	if (bLoadPackages)
	{
		if (DataReader.BeginGlobalTable("ResourcePacks"))
		{
			do
			{
				std::string PackageName;
				DataReader.ReadString(PackageName);
				ResourceGroup->LoadPackage(PackageName);
			} while (DataReader.NextTableElement());
			DataReader.EndTable();
		}
	}
	
	if (DataReader.BeginGlobalTable("SpotLights"))
	{
		do
		{
			FSpotlight Light = LoadSpotLight(DataReader);
			FEntity* Entity = World->CreateEntity();
			Entity->AddComponent(new FSpotLightComponent(Light));
		} while (DataReader.NextTableElement());
		DataReader.EndTable();
	}

	if (DataReader.BeginGlobalTable("PointLights"))
	{
		do
		{
			FPointLight Light = LoadPointLight(DataReader);
			FEntity* Entity = World->CreateEntity();
			Entity->AddComponent(new FPointLightComponent(Light));
		} while (DataReader.NextTableElement());
		DataReader.EndTable();
	}

	if (DataReader.BeginGlobalTable("Models"))
	{
		do 
		{
			LuaParser.StackDump();
			std::string ModelPath;
			DataReader.GetTable("modelPath").ReadString(ModelPath).Pop();
			LuaParser.StackDump();

			glm::vec3 Position, Rotation, Scale;
			DataReader.GetTable("position").ReadVector(Position).Pop();
			DataReader.GetTable("rotation").ReadVector(Rotation).Pop();
			DataReader.GetTable("scale").ReadVector(Scale).Pop();

			FModel* Model = ResourceGroup->LoadModel(ModelPath);
			FEntity* ModelEntity = World->CreateEntity();
			ModelEntity->AddComponent(new FMeshComponent(Model->Mesh));
			ModelEntity->AddComponent(new FTransformComponent(FTransform(Position, Rotation, Scale)));
		} while (DataReader.NextTableElement());
		DataReader.EndTable();
	}
}

