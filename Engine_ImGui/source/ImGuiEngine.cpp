#include "ImGuiEngine.h"
#include "imgui.h"
#include "GraphicsContext.h"
#include "Input.h"
#include "Texture.h"
#include "MaterialManager.h"
#include "ConstantBufferTypes.h"
#include "GraphicsBuffer.h"
#include "RenderState.h"
#include "Camera.h"
#include "ConfigVariableManager.h"
#include "ConfigVariable.h"

namespace ImGui
{
	void ShowEngineVars();
}

FImGuiImpl::FImGuiImpl(FGraphicsContext* GraphicsContext, FMaterialManager* MaterialManager, FInput* Input) : GraphicsContext(GraphicsContext), MaterialManager(MaterialManager), Input(Input)
{

}

FImGuiImpl::~FImGuiImpl()
{
	delete FontTexture;
	delete VertexBuffer;
	delete IndexBuffer;
	ImGui::Shutdown();
}

void FImGuiImpl::Init()
{
	ImGuiIO& IO = ImGui::GetIO();
	IO.DisplaySize = ImVec2((float)GraphicsContext->GetWidth(), (float)GraphicsContext->GetHeight());
	IO.KeyMap[ImGuiKey_Tab] = VK_TAB;                       // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array that we will update during the application lifetime.
	IO.KeyMap[ImGuiKey_LeftArrow] = VK_LEFT;
	IO.KeyMap[ImGuiKey_RightArrow] = VK_RIGHT;
	IO.KeyMap[ImGuiKey_UpArrow] = VK_UP;
	IO.KeyMap[ImGuiKey_DownArrow] = VK_DOWN;
	IO.KeyMap[ImGuiKey_PageUp] = VK_PRIOR;
	IO.KeyMap[ImGuiKey_PageDown] = VK_NEXT;
	IO.KeyMap[ImGuiKey_Home] = VK_HOME;
	IO.KeyMap[ImGuiKey_End] = VK_END;
	IO.KeyMap[ImGuiKey_Delete] = VK_DELETE;
	IO.KeyMap[ImGuiKey_Backspace] = VK_BACK;
	IO.KeyMap[ImGuiKey_Enter] = VK_RETURN;
	IO.KeyMap[ImGuiKey_Escape] = VK_ESCAPE;
	IO.KeyMap[ImGuiKey_A] = 'A';
	IO.KeyMap[ImGuiKey_C] = 'C';
	IO.KeyMap[ImGuiKey_V] = 'V';
	IO.KeyMap[ImGuiKey_X] = 'X';
	IO.KeyMap[ImGuiKey_Y] = 'Y';
	IO.KeyMap[ImGuiKey_Z] = 'Z';

	// Also load fonts.
	unsigned char* Pixels;
	int Width, Height;
	IO.Fonts->GetTexDataAsRGBA32(&Pixels, &Width, &Height);
	FontTexture = new FTexture(GraphicsContext, Width, Height, Pixels);
	IO.Fonts->TexID = (void *)FontTexture;
	IO.Fonts->ClearInputData();
	IO.Fonts->ClearTexData();

	const int kMaxSprites = 20000;

	VertexBuffer = new FGraphicsBuffer(GraphicsContext, EBufferType::Vertex, EBufferUsage::Dynamic, sizeof(ImDrawVert) * kMaxSprites * 4, nullptr);
	IndexBuffer = new FGraphicsBuffer(GraphicsContext, EBufferType::Index, EBufferUsage::Dynamic, sizeof(ImDrawIdx) * kMaxSprites * 6, nullptr);

	SetUIStyle();
}

void FImGuiImpl::Update(float DeltaTime)
{
	ImGuiIO& IO = ImGui::GetIO();
	IO.DeltaTime = DeltaTime;

	// Fill Input Data.
	const bool* KeysDown = Input->GetLowLevelKeysDown();
	const bool* MouseDown = Input->GetLowLevelMouseDown();

	IO.MousePos.x = Input->GetMousePosition().x;
	IO.MousePos.y = Input->GetMousePosition().y;

	for (int i = 0; i < 3; i++)
		IO.MouseDown[i] = MouseDown[i];

	IO.MouseWheel = 0; // TODO: Implement mouse wheel
	IO.KeyCtrl = Input->IsKeyDown(VK_CONTROL);
	IO.KeyShift = Input->IsKeyDown(VK_SHIFT);
	IO.KeyAlt = Input->IsKeyDown(VK_MENU);
	memcpy(IO.KeysDown, KeysDown, sizeof(bool) * 255);
	
	u8* TextBuffer;
	int CurrentInputCharCount = Input->ConsumeInputTextBuffer(TextBuffer);
	for (int i = 0; i < CurrentInputCharCount; i++)
	{
		IO.AddInputCharacter(TextBuffer[i]);
	}

	ImGui::NewFrame();

	if (Input->IsKeyPressed(VK_HOME))
	{
		bShowEngineVars = !bShowEngineVars;
	}

}

void FImGuiImpl::Render()
{
	if (bShowEngineVars)
	{
		ImGui::ShowEngineVars();
	}

	ImGui::Render();

	ID3D11DeviceContext* DeviceContext = GraphicsContext->GetImmediateContext();
	const float BlendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

	DeviceContext->OMSetBlendState(GraphicsContext->GetRenderStates()->AlphaBlendState, BlendFactor, 0xFFFFFFF);
	DeviceContext->RSSetState(GraphicsContext->GetRenderStates()->SolidNoCullRasterizer);

	FMaterial* SpriteMaterial = MaterialManager->GetSpriteMaterial();
	ImDrawData* DrawData = ImGui::GetDrawData();

	int IndexOffset = 0;
	int BaseVertex = 0;

	ID3D11RenderTargetView* RenderTarget = GraphicsContext->GetBackBuffer();
	DeviceContext->ClearDepthStencilView(GraphicsContext->GetDepthStencil(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	DeviceContext->OMSetRenderTargets(1, &RenderTarget, GraphicsContext->GetDepthStencil());

	FViewport ScreenViewport = FViewport(0, 0, GraphicsContext->GetWidth(), GraphicsContext->GetHeight());
	D3D11_VIEWPORT Viewport = ScreenViewport.CreateRenderViewport();
	DeviceContext->RSSetViewports(1, &Viewport);

	// Bind/upload vertex and index buffer by merging all the lists.

	ImDrawVert* VertexBufferData = (ImDrawVert*)VertexBuffer->MapData(DeviceContext);
	ImDrawIdx* IndexBufferData = (ImDrawIdx*)IndexBuffer->MapData(DeviceContext);

	// Cache aligned loops
	for (int ListIndex = 0; ListIndex < DrawData->CmdListsCount; ListIndex++)
	{
		ImDrawList* DrawList = DrawData->CmdLists[ListIndex];
		memcpy(VertexBufferData, DrawList->VtxBuffer.Data, sizeof(ImDrawVert) *  DrawList->VtxBuffer.Size);
		memcpy(IndexBufferData, DrawList->IdxBuffer.Data, sizeof(ImDrawIdx) * DrawList->IdxBuffer.Size);
		
		VertexBufferData += DrawList->VtxBuffer.Size;
		IndexBufferData += DrawList->IdxBuffer.Size;
	}
	
	VertexBuffer->UnMapData(DeviceContext);
	IndexBuffer->UnMapData(DeviceContext);

	ID3D11Buffer* Vb = VertexBuffer->GetBuffer();
	UINT Offsets = 0;
	UINT VertexSize = sizeof(ImDrawVert);

	DeviceContext->IASetIndexBuffer(IndexBuffer->GetBuffer(), DXGI_FORMAT_R16_UINT, 0);
	DeviceContext->IASetVertexBuffers(0, 1, &Vb, &VertexSize, &Offsets);
	DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	SpriteMaterial->GetParameters()->UpdateUniform(DeviceContext, FInstanceBuffer::kBufferName, &FInstanceBuffer::Identity, sizeof(FInstanceBuffer));

	SpriteMaterial->Apply(DeviceContext);

	// Render Stuff here
	for (int ListIndex = 0; ListIndex < DrawData->CmdListsCount; ListIndex++)
	{
		ImDrawList* DrawList = DrawData->CmdLists[ListIndex];
		for (int CommandIndex = 0; CommandIndex < DrawList->CmdBuffer.size(); CommandIndex++)
		{
			const ImDrawCmd& DrawCommand = DrawList->CmdBuffer[CommandIndex];
			const D3D11_RECT Rect = { (LONG)DrawCommand.ClipRect.x, (LONG)DrawCommand.ClipRect.y, (LONG)DrawCommand.ClipRect.z, (LONG)DrawCommand.ClipRect.w };

			FTexture* Texture = (FTexture*)DrawCommand.TextureId;
			// Issue draw call
			SpriteMaterial->GetParameters()->SetResource(FTextureNames::kDiffuseMap, Texture->GetResourceView());
			SpriteMaterial->GetParameters()->Apply(DeviceContext);
			DeviceContext->RSSetScissorRects(1, &Rect);
			DeviceContext->DrawIndexed(DrawCommand.ElemCount, IndexOffset, BaseVertex);
			IndexOffset += DrawCommand.ElemCount;
		}
		BaseVertex += DrawList->VtxBuffer.size();
	}

	DeviceContext->RSSetScissorRects(0, nullptr);
	DeviceContext->RSSetState(GraphicsContext->GetRenderStates()->SolidRasterizer);
	DeviceContext->OMSetBlendState(GraphicsContext->GetRenderStates()->OpaqueBlendState, BlendFactor, 0xFFFFFFF);
}

void FImGuiImpl::SetUIStyle()
{
	ImVec4 Colour_Gray = ImVec4(0.5f, 0.5f, 0.5f, 1.0);

	ImGui::PushStyleColor(ImGuiCol_TitleBg, Colour_Gray);
}

static void ShowConfigVariableEditor(FConfigVariable* Variable)
{
	switch (Variable->GetType())
	{
		case EConfigVariableType::String:
			if (ImGui::InputText(Variable->GetName(), (char*)Variable->AsString().data(), FConfigVariable::kStringMaxLength))
			{
				// We need to update the new string size manually.
				std::string& CurrentString = Variable->AsString();
				CurrentString.resize(strlen(CurrentString.data()));
			}
			break;
		case EConfigVariableType::Float:
			ImGui::InputFloat(Variable->GetName(), &Variable->AsFloat());
			break;
		case EConfigVariableType::Integer:
			ImGui::InputInt(Variable->GetName(), (int*)&Variable->AsInt());
			break;
		case EConfigVariableType::Boolean:
			ImGui::Checkbox(Variable->GetName(), (bool*)&Variable->AsBoolean());
			break;
		case EConfigVariableType::Vector2:
			ImGui::InputFloat2(Variable->GetName(), (float*)&Variable->AsVec2());
			break;
		case EConfigVariableType::Vector3:
			ImGui::InputFloat3(Variable->GetName(), (float*)&Variable->AsVec3());
			break;
	}
}

void ImGui::ShowEngineVars()
{
	static bool bShowCategories = true;
	static ConstantString CurrentCategory;

	FConfigVariableManager& Manager = FConfigVariableManager::Get();

	ImGui::SetNextWindowPosCenter();
	ImGui::Begin("Engine Vars", nullptr, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoMove);

	if (ImGui::BeginMenuBar())
	{
		static bool bSaveSelected = false;
		static bool bLoadSelected = false;
		static bool bCategorySelected = false;

		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Save", "", nullptr))
			{
				Manager.Save();
			}

			if (ImGui::MenuItem("Reload", "", nullptr))
			{
				Manager.Load();
			}

			if (ImGui::MenuItem("Back to Categories", "", nullptr))
			{
				bShowCategories = true;
			}

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}


	if (bShowCategories)
	{
		for (auto& Cat : Manager.GetCategories())
		{
			if (ImGui::Button(Cat.GetName()))
			{
				bShowCategories = false;
				CurrentCategory = Cat.GetName();
			}
		}
	}
	else
	{
		FConfigVariableCategory* Category = Manager.GetVariablesCategory(CurrentCategory);
		for (auto& Variable : Category->GetVariables())
		{
			ShowConfigVariableEditor(Variable);
		}

		if (ImGui::Button("Back to Categories"))
		{
			bShowCategories = true;
		}
	}

	ImGui::End();
}

void ImGui::ShowProfiler()
{
	auto ProfilerSections = FProfiler::Get().GetStatistics();
	auto ProfilerCounters = FProfiler::Get().GetCounters();

	static bool bShowGraphs = false;

	ImGui::SetNextWindowContentSize(ImVec2(800, 0));
	ImGui::Begin("Profiler", nullptr, ImVec2(800, 600), 0.95f, ImGuiWindowFlags_HorizontalScrollbar);

	ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "FPS %d", (s32)ImGui::GetIO().Framerate);
	ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Sections:");
	ImGui::Checkbox("Show Graphs", &bShowGraphs);

	std::stack<ConstantString> TreeParentStack;
	std::stack<bool> TreeOpenedStatus;

	ImGui::SetNextTreeNodeOpened(true, ImGuiSetCond_Appearing);
	if (ImGui::TreeNode("Root"))
	{
		TreeParentStack.push("Root");
		TreeOpenedStatus.push(true);
		for (auto& Section : ProfilerSections)
		{
			ConstantString CategoryName = Section.GetSectionName().Category;
			ConstantString SectionName = Section.GetSectionName().Name;
			ConstantString ParentSectionName = Section.GetParentName().Name;

			if (strcmp(TreeParentStack.top(), ParentSectionName) != 0)
			{
				if (TreeOpenedStatus.top())
				{
					ImGui::TreePop();
				}
				TreeOpenedStatus.pop();
				TreeParentStack.pop();
			}

			if (TreeOpenedStatus.top())
			{
				ImGui::SetNextTreeNodeOpened(true, ImGuiSetCond_Appearing);
				TreeParentStack.push(SectionName);
				TreeOpenedStatus.push(ImGui::TreeNode(SectionName, "%s : Avg - %.4f ms, Median - %.4f ms, Min - %.4f ms, Max - %.4f ms",
					Section.GetSectionName().Name,
					Section.GetAverageTime(), Section.GetMedianTime(),
					Section.GetMinimumTime(), Section.GetMaximumTime()));
			}

			if (TreeOpenedStatus.top() && bShowGraphs)
			{
				ImGui::PlotLines("", Section.GetSamples().Data(), Section.GetSampleCount(), 0, nullptr, Section.GetMinimumTime(), Section.GetMaximumTime());
			}
		}

		while (TreeOpenedStatus.size() > 0)
		{
			if (TreeOpenedStatus.top())
			{
				ImGui::TreePop();
			}
			TreeOpenedStatus.pop();
		}
	}
	ImGui::Separator();
	ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Counters:");

	ImGui::Columns(4, "Counter Columns");
	ImGui::Separator();
	ImGui::Text("Category"); ImGui::NextColumn();
	ImGui::Text("Name"); ImGui::NextColumn();
	ImGui::Text("Value"); ImGui::NextColumn();
	ImGui::Text("Resets Every Frame"); ImGui::NextColumn();
	ImGui::Separator();

	for (auto& Counter : ProfilerCounters)
	{
		s32 DisplayValue = Counter.DoesResetEveryFrame() ? Counter.GetLastValue() : Counter.GetValue();
		ImGui::Text(Counter.GetIdentifier().Category); ImGui::NextColumn();
		ImGui::Text(Counter.GetIdentifier().Name); ImGui::NextColumn();
		ImGui::Text("%d", DisplayValue); ImGui::NextColumn();
		ImGui::Text((Counter.DoesResetEveryFrame() ? "Yes" : "No")); ImGui::NextColumn();
	}

	ImGui::Columns(1);

	ImGui::End();
}
