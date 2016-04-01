#include "MeshUtility.h"
#include "Mesh.h"
#include "Material.h"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "Model.h"

static void TransformVertex(FModelVertex& VertexData, const glm::mat4& Matrix, const glm::mat4& InverseTranspose)
{
	VertexData.Position = glm::vec3(Matrix * glm::vec4(VertexData.Position, 1.0f));
	VertexData.Normal = glm::mat3(InverseTranspose) * VertexData.Normal;
}

FMeshBuilder FMeshUtility::GenerateCube(FMeshMaterialParameters MaterialParameters, const glm::mat4& Transform)
{
	FMeshBuilder MeshData(sizeof(FModelVertex), 24, 36);
	TVector<FModelVertex> Vertices;

	glm::mat4 InverseTransposed = glm::transpose(glm::inverse(Transform));

	// A box has six faces, each one pointing in a different direction.
	const int FaceCount = 6;

	static const glm::vec3 FaceNormals[FaceCount] =
	{
		{ 0, 0, 1 },
		{ 0, 0, -1 },
		{ 1, 0, 0 },
		{ -1, 0, 0 },
		{ 0, 1, 0 },
		{ 0, -1, 0 },
	};

	static const glm::vec2 TextureCoordinates[4] =
	{
		{ 1, 0 },
		{ 1, 1 },
		{ 0, 1 },
		{ 0, 0 },
	};

	glm::vec3 tsize = glm::vec3(1.0f)* 0.5f;

	// Create each face in turn.
	for (int i = 0; i < FaceCount; i++)
	{
		glm::vec3 Normal = FaceNormals[i];

		// Get two vectors perpendicular both to the face normal and to each other.
		glm::vec3 Basis = (i >= 4) ? glm::vec3(0.0f, 0.0f, 1.0f) : glm::vec3(0.0f, 1.0f, 0.0f);

		glm::vec3 Side1 = glm::cross(Normal, Basis);
		glm::vec3 Side2 = glm::cross(Normal, Side1);

		// Six indices (two triangles) per face.
		size_t VertexBase = i * 4;
		MeshData.AddIndex(VertexBase + 2);
		MeshData.AddIndex(VertexBase + 1);
		MeshData.AddIndex(VertexBase + 0);

		MeshData.AddIndex(VertexBase + 3);
		MeshData.AddIndex(VertexBase + 2);
		MeshData.AddIndex(VertexBase + 0);

		// Four vertices per face.
		auto v0 = FModelVertex((Normal - Side1 - Side2) * tsize, Normal, TextureCoordinates[0]);
		auto v1 = FModelVertex((Normal - Side1 + Side2) * tsize, Normal, TextureCoordinates[1]);
		auto v2 = FModelVertex((Normal + Side1 + Side2) * tsize, Normal, TextureCoordinates[2]);
		auto v3 = FModelVertex((Normal + Side1 - Side2) * tsize, Normal, TextureCoordinates[3]);

		TransformVertex(v0, Transform, InverseTransposed);
		TransformVertex(v1, Transform, InverseTransposed);
		TransformVertex(v2, Transform, InverseTransposed);
		TransformVertex(v3, Transform, InverseTransposed);

		MeshData.CopyAddVertex(v0);
		MeshData.CopyAddVertex(v1);
		MeshData.CopyAddVertex(v2);
		MeshData.CopyAddVertex(v3);
	}

	MeshData.AddPart(FMeshPartData(0, 36, 0, 0));
	MeshData.AddMaterialParameters(MaterialParameters);

	return MeshData;
}

FMeshBuilder FMeshUtility::GeneratePlane(FMeshMaterialParameters MaterialParameters, const glm::mat4& Transform, int Width, int Height, float CellSize)
{
	FMeshBuilder MeshData(sizeof(FModelVertex), Width * Height, 6 * (Width - 1) * (Height - 1));

	glm::mat4 InverseTransposed = glm::transpose(glm::inverse(Transform));

	float PlaneWidth = Width * CellSize;
	float PlaneHeight = Height * CellSize;

	float UIncrement = 1 / (float)Width;
	float VIncrement = 1 / (float)Height;

	float HalfSize = CellSize * 0.5f;

	glm::vec3 Normal(0.0f, 1.0f, 0.0f);

	for (int y = 0; y < Height; y++)
	{
		for (int x = 0; x < Width; x++)
		{
			glm::vec3 Point(-HalfSize, 0.0f, -HalfSize);
			glm::vec2 UV(x / (float)(Width - 1), y / (float)(Height - 1));
			glm::mat4 PositionTransform = glm::translate(glm::mat4(1.0f), glm::vec3(-PlaneWidth * 0.5f + x * CellSize, 0.0f, -PlaneHeight * 0.5f + y * CellSize));
			Point = glm::vec3(PositionTransform * glm::vec4(Point, 1.0f));
			FModelVertex Vertex(Point, Normal, UV);
			TransformVertex(Vertex, Transform, InverseTransposed);
			MeshData.CopyAddVertex(Vertex);
		}
	}

	for (int x = 0; x < Width - 1; x++)
	{
		for (int y = 0; y < Height - 1; y++)
		{
			int VertexIndex = x + (y * Width);

			MeshData.AddIndex(VertexIndex);
			MeshData.AddIndex(VertexIndex + Width);
			MeshData.AddIndex(VertexIndex + 1);

			MeshData.AddIndex(VertexIndex + Width);
			MeshData.AddIndex(VertexIndex + Width + 1);
			MeshData.AddIndex(VertexIndex + 1);
		}
	}

	MeshData.AddPart(FMeshPartData(0, (Width - 1) * (Height - 1) * 6, 0, 0));
	MeshData.AddMaterialParameters(MaterialParameters);

	return MeshData;
}

FMeshBuilder FMeshUtility::GenerateSphere(FMeshMaterialParameters MaterialParameters, int NumberOfSubdivisions, const glm::mat4& Transform)
{
	size_t VerticalSegments = NumberOfSubdivisions;
	size_t HorizontalSegments = NumberOfSubdivisions * 2;

	FMeshBuilder MeshData(sizeof(FModelVertex), (VerticalSegments + 1) * (HorizontalSegments + 1), VerticalSegments * (HorizontalSegments + 1) * 6);
	glm::mat4 InverseTransposed = glm::transpose(glm::inverse(Transform));

	float Radius = 0.5f;

	// Create rings of vertices at progressively higher latitudes.
	for (size_t i = 0; i <= VerticalSegments; i++)
	{
		float v = 1 - (float)i / VerticalSegments;

		float latitude = ((float)i * glm::pi<float>() / VerticalSegments) - glm::half_pi<float>();
		float dy, dxz;

		dy = glm::sin(latitude);
		dxz = glm::cos(latitude);

		// Create a single ring of vertices at this latitude.
		for (size_t j = 0; j <= HorizontalSegments; j++)
		{
			float u = (float)j / HorizontalSegments;

			float longitude = j * glm::pi<float>() * 2.0f / HorizontalSegments;
			float dx, dz;

			dx = glm::sin(longitude);
			dz = glm::cos(longitude);

			dx *= dxz;
			dz *= dxz;

			glm::vec3 Normal = glm::vec3(dx, dy, dz);
			glm::vec2 UV = glm::vec2(u, v);

			FModelVertex Vertex(Normal * Radius, Normal, UV);
			TransformVertex(Vertex, Transform, InverseTransposed);

			MeshData.CopyAddVertex(Vertex);
		}
	}

	// Fill the index buffer with triangles joining each pair of latitude rings.
	size_t stride = HorizontalSegments + 1;

	for (size_t i = 0; i < VerticalSegments; i++)
	{
		for (size_t j = 0; j <= HorizontalSegments; j++)
		{
			size_t nextI = i + 1;
			size_t nextJ = (j + 1) % stride;

			MeshData.AddIndex(i * stride + nextJ);
			MeshData.AddIndex(nextI * stride + j);
			MeshData.AddIndex(i * stride + j);

			MeshData.AddIndex(nextI * stride + nextJ);
			MeshData.AddIndex(nextI * stride + j);
			MeshData.AddIndex(i * stride + nextJ);
		}
	}

	MeshData.AddPart(FMeshPartData(0, VerticalSegments * HorizontalSegments * 6, 0, 0));
	MeshData.AddMaterialParameters(MaterialParameters);

	return MeshData;
}

FMeshBuilder FMeshUtility::GenerateTessellationPlane(FMeshMaterialParameters MaterialParameters, const glm::mat4& Transform, int Width, int Height, float CellSize)
{
	FMeshBuilder MeshData(sizeof(FModelVertex), (Width + 1) * (Height + 1), 4 * Width * Height);

	glm::mat4 InverseTransposed = glm::transpose(glm::inverse(Transform));

	float PlaneWidth = Width * CellSize;
	float PlaneHeight = Height * CellSize;

	float UIncrement = 1.0f / (float)Width;
	float VIncrement = 1.0f / (float)Height;

	float HalfSize = CellSize * 0.5f;

	glm::vec3 Normal(0.0f, 1.0f, 0.0f);

	for (int y = 0; y <= Height; y++)
	{
		for (int x = 0; x <= Width; x++)
		{
			glm::vec3 Point(-HalfSize, 0.0f, -HalfSize);
			glm::vec2 UV(x / (float)(Width), y / (float)(Height));
			glm::mat4 PositionTransform = glm::translate(glm::mat4(1.0f), glm::vec3(-PlaneWidth * 0.5f + x * CellSize, 0.0f, -PlaneHeight * 0.5f + y * CellSize));
			Point = glm::vec3(PositionTransform * glm::vec4(Point, 1.0f));
			FModelVertex Vertex(Point, Normal, UV);
			TransformVertex(Vertex, Transform, InverseTransposed);
			MeshData.CopyAddVertex(Vertex);
		}
	}

	for (int y = 0; y < Height; y++)
	{
		for (int x = 0; x < Width; x++)
		{
			int VertexIndex = x + (y * (Width + 1));

			MeshData.AddIndex(VertexIndex);
			MeshData.AddIndex(VertexIndex + 1);
			MeshData.AddIndex(VertexIndex + (Width + 1));
			MeshData.AddIndex(VertexIndex + (Width + 1) + 1);
		}
	}

	MeshData.AddPart(FMeshPartData(0, Width * Height * 4, 0, 0));
	MeshData.AddMaterialParameters(MaterialParameters);

	return MeshData;
}

FMeshBuilder FMeshUtility::GenerateTerrainQuiltPlane(FMeshMaterialParameters MaterialParameters, const glm::mat4& Transform, int Width, int Height, float CellSize)
{
	FMeshBuilder MeshData(sizeof(FModelVertex), Width * Height, 6 * (Width - 1) * (Height - 1));

	glm::mat4 InverseTransposed = glm::transpose(glm::inverse(Transform));

	float PlaneWidth = Width * CellSize;
	float PlaneHeight = Height * CellSize;

	float UIncrement = 1 / (float)Width;
	float VIncrement = 1 / (float)Height;

	float HalfSize = CellSize * 0.5f;

	glm::vec3 Normal(0.0f, 1.0f, 0.0f);

	for (int y = 0; y < Height; y++)
	{
		for (int x = 0; x < Width; x++)
		{
			glm::vec3 Point(-HalfSize, 0.0f, -HalfSize);
			glm::vec2 UV(x / (float)(Width - 1), y / (float)(Height - 1));
			glm::mat4 PositionTransform = glm::translate(glm::mat4(1.0f), glm::vec3(-PlaneWidth * 0.5f + x * CellSize, 0.0f, -PlaneHeight * 0.5f + y * CellSize));
			Point = glm::vec3(PositionTransform * glm::vec4(Point, 1.0f));
			FModelVertex Vertex(Point, Normal, UV);
			TransformVertex(Vertex, Transform, InverseTransposed);
			MeshData.CopyAddVertex(Vertex);
		}
	}

	bool bShouldInvert = false;

	for (int x = 0; x < Width - 1; x++)
	{
		for (int y = 0; y < Height - 1; y++)
		{
			int VertexIndex = x + (y * Width);

			if (bShouldInvert)
			{
				MeshData.AddIndex(VertexIndex);
				MeshData.AddIndex(VertexIndex + Width);
				MeshData.AddIndex(VertexIndex + Width + 1);

				MeshData.AddIndex(VertexIndex + 1);
				MeshData.AddIndex(VertexIndex);
				MeshData.AddIndex(VertexIndex + Width + 1);
			}
			else
			{
				MeshData.AddIndex(VertexIndex);
				MeshData.AddIndex(VertexIndex + Width);
				MeshData.AddIndex(VertexIndex + 1);

				MeshData.AddIndex(VertexIndex + Width);
				MeshData.AddIndex(VertexIndex + Width + 1);
				MeshData.AddIndex(VertexIndex + 1);
			}

			bShouldInvert = !bShouldInvert;
		}
	}

	MeshData.AddPart(FMeshPartData(0, (Width - 1) * (Height - 1) * 6, 0, 0));
	MeshData.AddMaterialParameters(MaterialParameters);

	return MeshData;
}

FMeshBuilder FMeshUtility::GenerateTerrainQuilt(FMeshMaterialParameters MaterialParameters, const glm::mat4& Transform, int Width, int Height, float CellSize, float MaxHeight, const TVector<float>& Heightmap)
{
	FMeshBuilder MeshData(sizeof(FModelVertex), Width * Height, 6 * (Width - 1) * (Height - 1));

	glm::mat4 InverseTransposed = glm::transpose(glm::inverse(Transform));

	float PlaneWidth = Width * CellSize;
	float PlaneHeight = Height * CellSize;

	float UIncrement = 1 / (float)Width;
	float VIncrement = 1 / (float)Height;

	float HalfSize = CellSize * 0.5f;

	glm::vec3 Normal(0.0f, 1.0f, 0.0f);

	TVector<FModelVertex> Vertices;
	Vertices.Resize(Width * Height);

	for (int y = 0; y < Height; y++)
	{
		for (int x = 0; x < Width; x++)
		{
			glm::vec3 Point(-HalfSize, Heightmap[y * Width + x] * MaxHeight, -HalfSize);
			glm::vec2 UV(x / (float)(Width - 1), y / (float)(Height - 1));
			glm::mat4 PositionTransform = glm::translate(glm::mat4(1.0f), glm::vec3(-PlaneWidth * 0.5f + x * CellSize, 0.0f, -PlaneHeight * 0.5f + y * CellSize));
			Point = glm::vec3(PositionTransform * glm::vec4(Point, 1.0f));
			FModelVertex Vertex(Point, Normal, UV);
			TransformVertex(Vertex, Transform, InverseTransposed);
			Vertices[y * Width + x] = Vertex;
		}
	}

	for (int y = 0; y < (Height - 1); y++)
	{
		for (int x = 0; x < (Width - 1); x++)
		{
			int Index1 = (y * Width) + x;
			int Index2 = (y * Width) + (x + 1);
			int Index3 = ((y + 1) * Width) + x;

			glm::vec3 Vertex1 = Vertices[Index1].Position;
			glm::vec3 Vertex2 = Vertices[Index2].Position;
			glm::vec3 Vertex3 = Vertices[Index3].Position;

			glm::vec3 Vector1 = Vertex1 - Vertex3;
			glm::vec3 Vector2 = Vertex3 - Vertex2;

			int Index = (y * (Width - 1)) + x;

			Vertices[Index].Normal = glm::normalize(glm::cross(Vector1, Vector2));
		}
	}

	// Calculate face normals:

	bool bShouldInvert = false;
	TVector<glm::vec3> FaceNormals;
	FaceNormals.Resize(Width * Height);

	for (int x = 0; x < Width - 1; x++)
	{
		for (int y = 0; y < Height - 1; y++)
		{
			int VertexIndex = x + (y * Width);

			int Index1 = VertexIndex;
			int Index2 = VertexIndex + Width;
			int Index3 = VertexIndex + 1;

			glm::vec3 Vertex1 = Vertices[Index1].Position;
			glm::vec3 Vertex2 = Vertices[Index2].Position;
			glm::vec3 Vertex3 = Vertices[Index3].Position;

			glm::vec3 Vector1 = Vertex1 - Vertex2;
			glm::vec3 Vector2 = Vertex1 - Vertex3;

			glm::vec3 Normal = glm::normalize(glm::cross(Vector1, Vector2));

			FaceNormals[y * Width + x] = Normal;
		}
	}

	for (int y = 0; y < Height; y++)
	{
		for (int x = 0; x < Width; x++)
		{
			// Initialize the sum.
			glm::vec3 Sum(0.0f, 0.0f, 0.0f);

			// Initialize the count.
			int Count = 0;

			int Index;

			// Bottom left face.
			if (((x - 1) >= 0) && ((y - 1) >= 0))
			{
				Index = ((y - 1) * (Width)) + (x - 1);
				Sum += FaceNormals[Index];
				Count++;
			}

			// Bottom right face.
			if ((x < (Width - 1)) && ((y - 1) >= 0))
			{
				Index = ((y - 1) * (Width)) + x;
				Sum += FaceNormals[Index];
				Count++;
			}

			// Upper left face.
			if (((x - 1) >= 0) && (y < (Height - 1)))
			{
				Index = (y * (Width)) + (x - 1);
				Sum += FaceNormals[Index];
				Count++;
			}

			// Upper right face.
			if ((x < (Width - 1)) && (y < (Height - 1)))
			{
				Index = (y * (Width)) + x;
				Sum += FaceNormals[Index];
				Count++;
			}

			// Take the average of the faces touching this vertex.
			Sum /= Count;

			// Calculate the length of this normal.
			float Length = (float)Sum.length();

			// Get an index to the vertex location in the height map array.
			Index = (y * Width) + x;

			// Normalize the final shared normal for this vertex and store it in the height map array.
			Vertices[Index].Normal = Sum / Length;
		}
	}

	/*
	for (int y = 0; y < Height - 1; y++)
	{
		for (int x = 0; x < Width - 1; x++)
		{
			glm::vec3 P0 = Vertices[x + y * Width].Position;
			glm::vec3 P1 = Vertices[x + 1 + y * Width].Position;
			glm::vec3 P2 = Vertices[Width * (y + 1) + x].Position;

			glm::vec3 Cross = glm::cross((P0 - P1), (P0 - P2));

			Vertices[y * Width + x].Normal = -glm::normalize(Cross);
		}
	}
	*/

	for (int y = 0; y < Height; y++)
	{
		for (int x = 0; x < Width; x++)
		{
			MeshData.CopyAddVertex(Vertices[y * Width + x]);
		}
	}

	bShouldInvert = false;

	for (int x = 0; x < Width - 1; x++)
	{
		for (int y = 0; y < Height - 1; y++)
		{
			int VertexIndex = x + (y * Width);

			if (bShouldInvert)
			{
				MeshData.AddIndex(VertexIndex);
				MeshData.AddIndex(VertexIndex + Width);
				MeshData.AddIndex(VertexIndex + Width + 1);

				MeshData.AddIndex(VertexIndex + 1);
				MeshData.AddIndex(VertexIndex);
				MeshData.AddIndex(VertexIndex + Width + 1);
			}
			else
			{
				MeshData.AddIndex(VertexIndex);
				MeshData.AddIndex(VertexIndex + Width);
				MeshData.AddIndex(VertexIndex + 1);

				MeshData.AddIndex(VertexIndex + Width);
				MeshData.AddIndex(VertexIndex + Width + 1);
				MeshData.AddIndex(VertexIndex + 1);
			}

			bShouldInvert = !bShouldInvert;
		}
	}

	MeshData.AddPart(FMeshPartData(0, (Width - 1) * (Height - 1) * 6, 0, 0));
	MeshData.AddMaterialParameters(MaterialParameters);

	return MeshData;
}
