/*
* Copyright (c) 2014-2021, NVIDIA CORPORATION. All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
* DEALINGS IN THE SOFTWARE.
*/

/*
License for Dear ImGui

Copyright (c) 2014-2019 Omar Cornut

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "imgui_renderer.h"
#include "../../VFS/VFS.h"

using namespace donut::vfs;
using namespace donut::engine;
using namespace donut::app;

namespace ImGui_Renderer_Private
{
    // https://github.com/ocornut/imgui/issues/707#issuecomment-917151020
	void EmbraceTheDarkness()
	{
		ImVec4* colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
		colors[ImGuiCol_Border] = ImVec4(0.19f, 0.19f, 0.19f, 0.29f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
		colors[ImGuiCol_Button] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
		colors[ImGuiCol_Separator] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
		colors[ImGuiCol_Tab] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_TableHeaderBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_TableBorderStrong] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_TableBorderLight] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
		colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
		colors[ImGuiCol_NavHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.35f);

		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowPadding = ImVec2(8.00f, 8.00f);
		style.FramePadding = ImVec2(5.00f, 2.00f);
		style.CellPadding = ImVec2(6.00f, 6.00f);
		style.ItemSpacing = ImVec2(6.00f, 6.00f);
		style.ItemInnerSpacing = ImVec2(6.00f, 6.00f);
		style.TouchExtraPadding = ImVec2(0.00f, 0.00f);
		style.IndentSpacing = 25;
		style.ScrollbarSize = 15;
		style.GrabMinSize = 10;
		style.WindowBorderSize = 1;
		style.ChildBorderSize = 1;
		style.PopupBorderSize = 1;
		style.FrameBorderSize = 1;
		style.TabBorderSize = 1;
		style.WindowRounding = 7;
		style.ChildRounding = 4;
		style.FrameRounding = 3;
		style.PopupRounding = 4;
		style.ScrollbarRounding = 9;
		style.GrabRounding = 3;
		style.LogSliderDeadzone = 4;
		style.TabRounding = 4;
	}
}


ImGui_Renderer::ImGui_Renderer(DeviceManager *devManager)
    : IRenderPass(devManager)
{
    ImGui::CreateContext();

    // ImGui style
    //ImGui::StyleColorsDark();
    ImGui_Renderer_Private::EmbraceTheDarkness();
}

ImGui_Renderer::~ImGui_Renderer()
{
    ImGui::DestroyContext();
}

bool ImGui_Renderer::Init(std::shared_ptr<ShaderFactory> shaderFactory)
{
    // Set up keyboard mapping.
    // ImGui will use those indices to peek into the io.KeyDown[] array
    // that we will update during the application lifetime.
    ImGuiIO& io = ImGui::GetIO();
    io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
    io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
    io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
    io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
    io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
    io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
    io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
    io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
    io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
    io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
    io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
    io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
    io.KeyMap[ImGuiKey_A] = 'A';
    io.KeyMap[ImGuiKey_C] = 'C';
    io.KeyMap[ImGuiKey_V] = 'V';
    io.KeyMap[ImGuiKey_X] = 'X';
    io.KeyMap[ImGuiKey_Y] = 'Y';
    io.KeyMap[ImGuiKey_Z] = 'Z';

    imgui_nvrhi = std::make_unique<ImGui_NVRHI>();
    return imgui_nvrhi->init(GetDevice(), shaderFactory);
}

ImFont* ImGui_Renderer::LoadFont(IFileSystem& fs, const std::filesystem::path& fontFile, float fontSize)
{
	auto fontData = fs.readFile(fontFile);
	if (!fontData)
		return nullptr;

    ImFontConfig fontConfig;

	// XXXX mk: this appears to be a bug: the atlas copies (& owns) the data when the
	// flag is set to false !
	fontConfig.FontDataOwnedByAtlas = false;
	ImFont* imFont = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(
		(void*)(fontData->data()), (int)(fontData->size()), fontSize, &fontConfig);

	return imFont;
}

bool ImGui_Renderer::KeyboardUpdate(int key, int scancode, int action, int mods)
{
    auto& io = ImGui::GetIO();

    bool keyIsDown;
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        keyIsDown = true;
    } else {
        keyIsDown = false;
    }

    // update our internal state tracking for this key button
    keyDown[key] = keyIsDown;

    if (keyIsDown)
    {
        // if the key was pressed, update ImGui immediately
        io.KeysDown[key] = true;
    } else {
        // for key up events, ImGui state is only updated after the next frame
        // this ensures that short keypresses are not missed
    }

    return io.WantCaptureKeyboard;
}

bool ImGui_Renderer::KeyboardCharInput(unsigned int unicode, int mods)
{
    auto& io = ImGui::GetIO();

    io.AddInputCharacter(unicode);

    return io.WantCaptureKeyboard;
}

bool ImGui_Renderer::MousePosUpdate(double xpos, double ypos)
{
    auto& io = ImGui::GetIO();
    io.MousePos.x = float(xpos);
    io.MousePos.y = float(ypos);

    return io.WantCaptureMouse;
}

bool ImGui_Renderer::MouseScrollUpdate(double xoffset, double yoffset)
{
    auto& io = ImGui::GetIO();
    io.MouseWheel += float(yoffset);

    return io.WantCaptureMouse;
}

bool ImGui_Renderer::MouseButtonUpdate(int button, int action, int mods)
{
    auto& io = ImGui::GetIO();

    bool buttonIsDown;
    int buttonIndex;

    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        buttonIsDown = true;
    } else {
        buttonIsDown = false;
    }

    switch(button)
    {
        case GLFW_MOUSE_BUTTON_LEFT:
            buttonIndex = 0;
            break;

        case GLFW_MOUSE_BUTTON_RIGHT:
            buttonIndex = 1;
            break;

        case GLFW_MOUSE_BUTTON_MIDDLE:
            buttonIndex = 2;
            break;
    }

    // update our internal state tracking for this mouse button
    mouseDown[buttonIndex] = buttonIsDown;

    if (buttonIsDown)
    {
        // update ImGui state immediately
        io.MouseDown[buttonIndex] = true;
    } else {
        // for mouse up events, ImGui state is only updated after the next frame
        // this ensures that short clicks are not missed
    }

    return io.WantCaptureMouse;
}

void ImGui_Renderer::Animate(float elapsedTimeSeconds)
{
    if (!imgui_nvrhi) return;

    int w, h;
    float scaleX, scaleY;

    GetDeviceManager()->GetWindowDimensions(w, h);
    GetDeviceManager()->GetDPIScaleInfo(scaleX, scaleY);

    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(float(w), float(h));
    io.DisplayFramebufferScale.x = scaleX;
    io.DisplayFramebufferScale.y = scaleY;

    io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
    io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
    io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
    io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];

    imgui_nvrhi->beginFrame(elapsedTimeSeconds);
}

void ImGui_Renderer::Render(nvrhi::IFramebuffer* framebuffer)
{
    if (!imgui_nvrhi) return;

    BuildUI();

    ImGui::Render();
    imgui_nvrhi->render(framebuffer);

    // reconcile mouse button states
    auto& io = ImGui::GetIO();
    for(size_t i = 0; i < mouseDown.size(); i++)
    {
        if (io.MouseDown[i] == true && mouseDown[i] == false)
        {
            io.MouseDown[i] = false;
        }
    }

    // reconcile key states
    for(size_t i = 0; i < keyDown.size(); i++)
    {
        if (io.KeysDown[i] == true && keyDown[i] == false)
        {
            io.KeysDown[i] = false;
        }
    }
}

void ImGui_Renderer::BackBufferResizing()
{
    if(imgui_nvrhi) imgui_nvrhi->backbufferResizing();
}

void ImGui_Renderer::BeginFullScreenWindow()
{
    int width, height;
    GetDeviceManager()->GetWindowDimensions(width, height);
    ImGui::SetNextWindowPos(ImVec2(0.f, 0.f), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(float(width), float(height)), ImGuiCond_Always);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
    ImGui::SetNextWindowBgAlpha(0.f);
    ImGui::Begin(" ", 0, ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar);
}

void ImGui_Renderer::DrawScreenCenteredText(const char* text)
{
    int width, height;
    GetDeviceManager()->GetWindowDimensions(width, height);
    ImVec2 textSize = ImGui::CalcTextSize(text);
    ImGui::SetCursorPosX((float(width) - textSize.x) * 0.5f);
    ImGui::SetCursorPosY((float(height) - textSize.y) * 0.5f);
    ImGui::TextUnformatted(text);
}

void ImGui_Renderer::EndFullScreenWindow()
{
    ImGui::End();
    ImGui::PopStyleVar();
}
