#include "menu.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx9.h"

#include <stdexcept>
#include <thread>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


LRESULT CALLBACK WindowProcess(HWND window, UINT message, WPARAM wideParam, LPARAM longParam)
{
	if (GetAsyncKeyState(VK_INSERT) & 1)
	{
		menu::show_menu = !menu::show_menu;
	}

	if (menu::show_menu)
	{
		if (menu::clickable != true)
			menu::change_click(true);
	}
	else
	{
		if (menu::clickable != false)
			menu::change_click(false);
	}

	if (menu::show_menu && ImGui_ImplWin32_WndProcHandler(window, message, wideParam, longParam))
		return 1L;

	return CallWindowProc(menu::originalWindowProcess, window, message, wideParam, longParam);
}

bool menu::SetupWindowClass(const char* windowClassName) noexcept
{
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = DefWindowProc;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = GetModuleHandle(NULL);
	windowClass.hIcon = NULL;
	windowClass.hCursor = NULL;
	windowClass.hbrBackground = NULL;
	windowClass.lpszMenuName = NULL;
	windowClass.lpszClassName = windowClassName;
	windowClass.hIconSm = NULL;

	if (!RegisterClassEx(&windowClass))
		return false;

	return true;
}

void menu::DestroyWindowClass() noexcept
{
	UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
}

bool menu::SetupWindow(const char* windowName) noexcept
{
	window = CreateWindow(windowClass.lpszClassName, windowName, WS_OVERLAPPEDWINDOW, 0, 0, 100, 100, 0, 0, windowClass.hInstance, 0);

	if (!window)
		return false;

	return true;
}

void menu::DestroyWindow() noexcept
{
	if (window)
		DestroyWindow(window);
}

bool menu::SetupDirectX() noexcept
{
	const auto handle = GetModuleHandle("d3d9.dll");

	if (!handle)
		return false;

	using CreateFn = LPDIRECT3D9(__stdcall*)(UINT);

	const auto create = reinterpret_cast<CreateFn>(GetProcAddress(handle, "Direct3DCreate9"));

	if (!create)
		return false;

	d3d9 = create(D3D_SDK_VERSION);

	if (!d3d9)
		return false;

	D3DPRESENT_PARAMETERS params = { };
	params.BackBufferWidth = 0;
	params.BackBufferHeight = 0;
	params.BackBufferFormat = D3DFMT_UNKNOWN;
	params.BackBufferCount = 0;
	params.MultiSampleType = D3DMULTISAMPLE_NONE;
	params.MultiSampleQuality = NULL;
	params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	params.hDeviceWindow = window;
	params.Windowed = 1;
	params.EnableAutoDepthStencil = 0;
	params.AutoDepthStencilFormat = D3DFMT_UNKNOWN;
	params.Flags = NULL;
	params.FullScreen_RefreshRateInHz = 0;
	params.PresentationInterval = 0;

	if (d3d9->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_NULLREF,
		window,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_DISABLE_DRIVER_MANAGEMENT,
		&params,
		&device
	) < 0) return false;

	return true;
}

void menu::DestroyDirectX() noexcept
{
	if (device) {
		device->Release();
		device = NULL;
	}

	if (d3d9) {
		d3d9->Release();
		d3d9 = NULL;
	}
}

void menu::Setup()
{
	if (!SetupWindowClass("odinClass902"))
		throw std::runtime_error("Failed to create window class.");

	if (!SetupWindow("Iw4X Window"))
		throw std::runtime_error("Failed to create window.");

	if (!SetupDirectX())
		throw std::runtime_error("Failed to create device.");

	DestroyWindow();
	DestroyWindowClass();
}

void menu::SetupMenu(LPDIRECT3DDEVICE9 device) noexcept
{
	auto params = D3DDEVICE_CREATION_PARAMETERS{ };
	device->GetCreationParameters(&params);

	window = params.hFocusWindow;

	originalWindowProcess = reinterpret_cast<WNDPROC>(
		SetWindowLongPtr(window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WindowProcess))
	);

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::StyleColorsDark();
	menu::MenuStyle();

	ImFontConfig arialConfig;
	arialConfig.FontDataOwnedByAtlas = false;
	ImFont* m_font = io.Fonts->AddFontFromFileTTF(("c:\\Windows\\Fonts\\Bahnschrift.ttf"), 14.0f, &arialConfig);

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX9_Init(device);

	setup = true;

}

void menu::Destroy() noexcept
{
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	SetWindowLongPtr(
		window,
		GWLP_WNDPROC,
		reinterpret_cast<LONG_PTR>(originalWindowProcess)
	);

	DestroyDirectX();
}

inline static bool onetime = false;
void menu::Render() noexcept
{

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	if (!onetime) {
		Beep(340, 500);
		MessageBox(0, "Cheat Injection Sucess!", "", MB_OK);
		onetime = true;
	}

	switch (settings::aimbot::current_aimkey)
	{
	case 0:
		settings::aimbot::current_key = VK_LBUTTON;
		break;
	case 1:
		settings::aimbot::current_key = VK_RBUTTON;
		break;
	case 2:
		settings::aimbot::current_key = VK_XBUTTON1;
		break;
	case 3:
		settings::aimbot::current_key = VK_XBUTTON2;
		break;
	case 4:
		settings::aimbot::current_key = VK_SHIFT;
		break;
	case 5:
		settings::aimbot::current_key = VK_CONTROL;
		break;
	case 6:
		settings::aimbot::current_key = VK_MENU;
		break;
	case 7:
		settings::aimbot::current_key = VK_CAPITAL;
		break;
	default:
		settings::aimbot::current_key = VK_LBUTTON; // default to left mouse button
		break;
	}

	if (show_menu)
	{
		ImGui::SetNextWindowSize({ 800, 450 });
		ImGui::Begin("IW4x Interal by odin1337", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);
		{
			if (ImGui::BeginTabBar("MainTabBar"))
			{
				if (ImGui::BeginTabItem("Aimbot"))
				{
					ImGui::Columns(2, nullptr, false);
					ImGui::SetColumnWidth(2, 550.0f / 2);
					float width = ImGui::GetColumnWidth() - 10.0f - ImGui::GetStyle().ChildBorderSize * 2;
					ImGui::BeginChild("##aimbot", ImVec2(width, 0), true);
					{
						ImGui::Text("Aimbot Toggles");
						ImGui::Separator();
						ImGui::Dummy(ImVec2(0, 1.0f));

						ImGui::Checkbox("Enable Aimbot", &settings::aimbot::enable);
						if (settings::aimbot::enable)
						{
							ImGui::Checkbox("Memory Aim", &settings::aimbot::kernelAim);
							ImGui::Checkbox("Silent Aim", &settings::aimbot::silentAim);
							ImGui::Checkbox("Crosshair", &settings::aimbot::crosshair);
							ImGui::Checkbox("Show FOV", &settings::aimbot::drawFov);
						}
					}
					ImGui::EndChild();
					ImGui::NextColumn();
					ImGui::BeginChild("##aimbotOptions", ImVec2(width, 0), true);
					{
						ImGui::Text("Aimbot Settings");
						ImGui::Separator();
						ImGui::Dummy(ImVec2(0, 1.0f));

						ImGui::SliderFloat("FOV", &settings::aimbot::fovSize, 50, 500, "%.f");
						ImGui::SliderFloat("Smoothness", &settings::aimbot::smoothness, 1, 25, "%.f%");
						ImGui::Combo("AimKey", &settings::aimbot::current_aimkey, settings::aimbot::aimkey, sizeof(settings::aimbot::aimkey) / sizeof(*settings::aimbot::aimkey));
						/*if (ImGui::BeginCombo("Hitbox", getHitboxTypeName(settings::aimbot::current_hitbox)))
						{
							for (auto hitboxType : hitboxValues)
							{
								if (ImGui::Selectable(getHitboxTypeName(hitboxType), settings::aimbot::current_hitbox == hitboxType)) settings::aimbot::current_hitbox = hitboxType;
							}
							ImGui::EndCombo();
						}*/
					}
					ImGui::EndChild();
					ImGui::Columns(1);
					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Visuals"))
				{
					ImGui::Columns(2, nullptr, false);
					ImGui::SetColumnWidth(2, 550.0f / 2);
					float width = ImGui::GetColumnWidth() - 10.0f - ImGui::GetStyle().ChildBorderSize * 2;
					ImGui::BeginChild("##visual", ImVec2(width, 0), true);
					{
						ImGui::Text("Visuals Toggles");
						ImGui::Separator();
						ImGui::Dummy(ImVec2(0, 1.0f));

						ImGui::Checkbox("Enable Visuals", &settings::visuals::enable);
						if (settings::visuals::enable)
						{
							ImGui::Checkbox("Box", &settings::visuals::drawBox);
							ImGui::Checkbox("Skeleton", &settings::visuals::drawSkeleton);
							ImGui::Checkbox("Snaplines", &settings::visuals::drawSnaplines);
							ImGui::Checkbox("Name", &settings::visuals::drawName);
							ImGui::Checkbox("Distance", &settings::visuals::drawDistance);
						}
					}
					ImGui::EndChild();
					ImGui::NextColumn();
					ImGui::BeginChild("##visualsOptions", ImVec2(width, 0), true);
					{
						ImGui::Text("Visuals Settings");
						ImGui::Separator();
						ImGui::Dummy(ImVec2(0, 1.0f));

						/*ImGui::SliderFloat("Render Distance", &settings::visuals::renderDistance, 100, 3000, "%.fm");
						if (ImGui::BeginCombo("Box Type", getBoxTypeName(settings::visuals::boxType)))
						{
							for (auto boxType : boxValues)
							{
								if (ImGui::Selectable(getBoxTypeName(boxType), settings::visuals::boxType == boxType)) settings::visuals::boxType = boxType;
							}
							ImGui::EndCombo();
						}*/
					}
					ImGui::EndChild();
					ImGui::Columns(1);
					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Exploits"))
				{
					ImGui::Columns(2, nullptr, false);
					ImGui::SetColumnWidth(2, 550.0f / 2);
					float width = ImGui::GetColumnWidth() - 10.0f - ImGui::GetStyle().ChildBorderSize * 2;
					ImGui::BeginChild("##visual", ImVec2(width, 0), true);
					{
						ImGui::Text("Exploits");
						ImGui::Separator();
						ImGui::Dummy(ImVec2(0, 1.0f));

						ImGui::Checkbox("Enable Exploits", &settings::exploits::enable);
						if (settings::exploits::enable)
						{
							ImGui::Checkbox("No Recoil", &settings::exploits::noRecoil);
							ImGui::Checkbox("No Spread", &settings::exploits::noSpread);
							ImGui::Checkbox("Steady Aim Pro Extra", &settings::exploits::noSway);
							ImGui::Checkbox("Mantle Lunge", &settings::exploits::mantleLunge);
						}
					}
					ImGui::EndChild();
					ImGui::NextColumn();
					ImGui::BeginChild("##visualsOptions", ImVec2(width, 0), true);
					{
						ImGui::Text("Exploits Settings");
						ImGui::Separator();
						ImGui::Dummy(ImVec2(0, 1.0f));
					}
					ImGui::EndChild();
					ImGui::Columns(1);
					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Info"))
				{
					ImGui::Columns(2, nullptr, false);
					ImGui::SetColumnWidth(2, 550.0f / 2);
					float width = ImGui::GetColumnWidth() - 10.0f - ImGui::GetStyle().ChildBorderSize * 2;
					ImGui::BeginChild("##info", ImVec2(width, 0), true);
					{
						ImGui::Text("Info");
						ImGui::Separator();
						ImGui::Dummy(ImVec2(0, 1.0f));

						ImGui::Text("Discord: ");
						ImGui::SameLine(0, 0);
						ImGui::TextColored(ImVec4(0.4980392158031464f, 0.5137255191802979f, 1.0f, 1), "discord.gg/apollocheats");
						ImGui::Text("Made By ");
						ImGui::SameLine(0, 0);
						ImGui::TextColored(ImVec4(0.4980392158031464f, 0.5137255191802979f, 1.0f, 1), "github.com/odin-1337");
					}
					ImGui::EndChild();
					ImGui::NextColumn();
					ImGui::BeginChild("##options", ImVec2(width, 0), true);
					{
						ImGui::Text("Options");
						ImGui::Separator();
						ImGui::Dummy(ImVec2(0, 1.0f));

						if (ImGui::Button("Unload Cheat", ImVec2(ImGui::GetContentRegionAvail().x, 32)))
						{
							exit(0);
						}
					}
					ImGui::EndChild();
					ImGui::Columns(1);
					ImGui::EndTabItem();
				}

				ImGui::EndTabBar();
			}
		}
		ImGui::End();
	}

	menu::cheatLoop();

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

}

void menu::MenuStyle()
{

	ImGuiStyle& style = ImGui::GetStyle();

	style.Alpha = 1.0f;
	style.FrameRounding = 3.0f;
	style.DisabledAlpha = 1.0f;
	style.WindowPadding = ImVec2(12.0f, 12.0f);
	style.WindowRounding = 8.0f;
	style.WindowBorderSize = 0.0f;
	style.WindowMinSize = ImVec2(20.0f, 20.0f);
	style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
	style.WindowMenuButtonPosition = ImGuiDir_None;
	style.ChildRounding = 0.0f;
	style.ChildBorderSize = 1.0f;
	style.PopupRounding = 0.0f;
	style.PopupBorderSize = 1.0f;
	style.FramePadding = ImVec2(6.0f, 6.0f);
	style.FrameRounding = 0.0f;
	style.FrameBorderSize = 0.0f;
	style.ItemSpacing = ImVec2(12.0f, 6.0f);
	style.ItemInnerSpacing = ImVec2(6.0f, 3.0f);
	style.CellPadding = ImVec2(12.0f, 6.0f);
	style.IndentSpacing = 20.0f;
	style.ColumnsMinSpacing = 6.0f;
	style.ScrollbarSize = 12.0f;
	style.ScrollbarRounding = 0.0f;
	style.GrabMinSize = 12.0f;
	style.GrabRounding = 0.0f;
	style.TabRounding = 3.0f;
	style.TabBorderSize = 0.0f;
	style.TabMinWidthForCloseButton = 0.0f;
	style.ColorButtonPosition = ImGuiDir_Right;
	style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
	style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

	style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.2745098173618317f, 0.3176470696926117f, 0.4509803950786591f, 1.0f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.0784313753247261f, 0.08627451211214066f, 0.1019607856869698f, 1.0f);
	style.Colors[ImGuiCol_ChildBg] = ImVec4(0.0784313753247261f, 0.08627451211214066f, 0.1019607856869698f, 1.0f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.0784313753247261f, 0.08627451211214066f, 0.1019607856869698f, 1.0f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.1568627506494522f, 0.168627455830574f, 0.1921568661928177f, 1.0f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0784313753247261f, 0.08627451211214066f, 0.1019607856869698f, 1.0f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.1568627506494522f, 0.168627455830574f, 0.1921568661928177f, 1.0f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.2352941185235977f, 0.2156862765550613f, 0.5960784554481506f, 1.0f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.0470588244497776f, 0.05490196123719215f, 0.07058823853731155f, 1.0f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.0470588244497776f, 0.05490196123719215f, 0.07058823853731155f, 1.0f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0784313753247261f, 0.08627451211214066f, 0.1019607856869698f, 1.0f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.09803921729326248f, 0.105882354080677f, 0.1215686276555061f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.0470588244497776f, 0.05490196123719215f, 0.07058823853731155f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.1568627506494522f, 0.168627455830574f, 0.1921568661928177f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.4980392158031464f, 0.5137255191802979f, 1.0f, 1.0f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.4980392158031464f, 0.5137255191802979f, 1.0f, 1.0f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.5372549295425415f, 0.5529412031173706f, 1.0f, 1.0f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.196078434586525f, 0.1764705926179886f, 0.5450980663299561f, 1.0f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.2352941185235977f, 0.2156862765550613f, 0.5960784554481506f, 1.0f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.196078434586525f, 0.1764705926179886f, 0.5450980663299561f, 1.0f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.2352941185235977f, 0.2156862765550613f, 0.5960784554481506f, 1.0f);
	style.Colors[ImGuiCol_Separator] = ImVec4(0.1568627506494522f, 0.1843137294054031f, 0.250980406999588f, 1.0f);
	style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.1568627506494522f, 0.1843137294054031f, 0.250980406999588f, 1.0f);
	style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.1568627506494522f, 0.1843137294054031f, 0.250980406999588f, 1.0f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.196078434586525f, 0.1764705926179886f, 0.5450980663299561f, 1.0f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.2352941185235977f, 0.2156862765550613f, 0.5960784554481506f, 1.0f);
	style.Colors[ImGuiCol_Tab] = ImVec4(0.0470588244497776f, 0.05490196123719215f, 0.07058823853731155f, 1.0f);
	style.Colors[ImGuiCol_TabHovered] = ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0f);
	style.Colors[ImGuiCol_TabActive] = ImVec4(0.09803921729326248f, 0.105882354080677f, 0.1215686276555061f, 1.0f);
	style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.0470588244497776f, 0.05490196123719215f, 0.07058823853731155f, 1.0f);
	style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.0784313753247261f, 0.08627451211214066f, 0.1019607856869698f, 1.0f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.5215686559677124f, 0.6000000238418579f, 0.7019608020782471f, 1.0f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.03921568766236305f, 0.9803921580314636f, 0.9803921580314636f, 1.0f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(1.0f, 0.2901960909366608f, 0.5960784554481506f, 1.0f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.9960784316062927f, 0.4745098054409027f, 0.6980392336845398f, 1.0f);
	style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.0470588244497776f, 0.05490196123719215f, 0.07058823853731155f, 1.0f);
	style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.0470588244497776f, 0.05490196123719215f, 0.07058823853731155f, 1.0f);
	style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0f);
	style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(0.09803921729326248f, 0.105882354080677f, 0.1215686276555061f, 1.0f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.2352941185235977f, 0.2156862765550613f, 0.5960784554481506f, 1.0f);
	style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.4980392158031464f, 0.5137255191802979f, 1.0f, 1.0f);
	style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.4980392158031464f, 0.5137255191802979f, 1.0f, 1.0f);
	style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.4980392158031464f, 0.5137255191802979f, 1.0f, 1.0f);
	style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.196078434586525f, 0.1764705926179886f, 0.5450980663299561f, 0.501960813999176f);
	style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.196078434586525f, 0.1764705926179886f, 0.5450980663299561f, 0.501960813999176f);

}

void menu::change_click(bool canclick)
{
	long style = ::GetWindowLong(window, GWL_EXSTYLE);
	if (canclick)
	{
		style &= ~WS_EX_LAYERED;
		SetWindowLong(window, GWL_EXSTYLE, style);
		SetFocus(window);
		menu::clickable = true;
	}
	else
	{
		style |= WS_EX_LAYERED;
		SetWindowLong(window, GWL_EXSTYLE, style);
		SetFocus(window);
		menu::clickable = false;
	}
}

void menu::cheatLoop()
{
	char Watermark[64];
	sprintf_s(Watermark, ("Cheat | FPS %.1f"), ImGui::GetIO().Framerate);
	float fontSize = 20;
	float textWidth = ImGui::CalcTextSize(("Cheat FPS %.1f")).x * (fontSize / ImGui::GetFontSize());
	ImVec2 textPosition(30, 20);
	ImColor color = ImColor(255, 255, 255);
	ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), fontSize, textPosition, color, Watermark);

	if (!settings::misc::show_console) {
		ShowWindow(GetConsoleWindow(), SW_HIDE);
	}
	else if (settings::misc::show_console) {
		ShowWindow(GetConsoleWindow(), SW_SHOW);
	}

	if (settings::misc::streamproof)
	{
		SetWindowDisplayAffinity(window, WDA_EXCLUDEFROMCAPTURE);

	}
	else if (!settings::misc::streamproof)
	{
		SetWindowDisplayAffinity(window, !WDA_EXCLUDEFROMCAPTURE);

	}


	if (settings::aimbot::drawFov)
	{
		ImGui::GetForegroundDrawList()->AddCircle(ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2), settings::aimbot::fovSize, ImColor(255, 0, 255), 120, 1.0f);
	}

	std::this_thread::sleep_for(std::chrono::microseconds(500));
}