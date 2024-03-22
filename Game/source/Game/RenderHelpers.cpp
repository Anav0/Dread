#include "Engine/Gui.h"
#include "Engine/Model.h"
#include "Engine/Renderer.h"
#include "Engine/ResourceManager.h"
#include "Engine/TextRenderer.h"
#include <Engine/Collision.h>

#include "RenderHelpers.h"
#include "EntityManager.h"
#include "Entities.h"
#include "Atlas.h"

#include "stb_truetype.h"

std::vector<MeshInBuffer> AddModel(v3 position, const std::string& model_name, v4 color, f32 rotation, f32 scale)
{
    Model* model = RM.GetModel(model_name);

    std::vector<MeshInBuffer> meshes;

		u32 i=0;
    for (auto& mesh : model->meshes) {
        InstancedMeshBuffer* buffer;
        buffer = R.GetBuffer(mesh.id);
        if (buffer == nullptr)
            buffer = R.CreateBuffer(mesh);

        auto mesh_in_buffer = buffer->AddMesh(position, color, rotation, scale);
        mesh_in_buffer.buffer_index = R.buffers.size() - 1;
        meshes.push_back(mesh_in_buffer);
				i++;
    }

    assert(meshes.size() > 0);

    return meshes;
}

constexpr u16 top_offset = 120;

void DrawTurnUI(WindowManager* window)
{
	GetDateStr(STATE.game_date_str);
	v2 size = TR.GetTextSize(STATE.game_date_str.c_str(), default_style.font_size);
	v2 pos;
	v2 header_size = v2(0, HEADER_H);
	v2 header_pos  = v2(0, window->screen_size.y - HEADER_H);
	CenterChildInParentY(&header_pos, &header_size, &pos, &size);
    UI.Stack(Direction::Horizontal, 10, { window->screen_size.x - 265, pos.y });
    	UI.DrawLabel(STATE.game_date_str);
    UI.EndLayout();

	if (UI.DrawIcon(END_TURN_ICON, {window->screen_size.x - 100, pos.y - 45})) {
		GoToNextPhase();
	}
}

void DrawResources(WindowManager* window)
{
    IconParams sword_icon, megaphone_icon;
    sword_icon.size = v2(64);
    sword_icon.scale = 0.6;
    sword_icon.padding = 25;
    megaphone_icon = sword_icon;

    megaphone_icon.index = MEGAPHONE;
    sword_icon.index     = SWORD;

    u32 y = CenterChildInParentY(window->screen_size.y - HEADER_H, HEADER_H, sword_icon.size.y);
    u32 x = 10;

    UI.Stack(Direction::Horizontal, 10, { x, y });
		UI.DrawIcon(UKRAINE_EMBLEM, { 0, 0 }, {0, -75});
	    UI.DrawIconAndLabel(sword_icon, std::format("Reserve: {}", STATE.reserve));
	    UI.DrawIconAndLabel(megaphone_icon, std::format("Popular support: {}%", STATE.popular_support));
    UI.EndLayout();
}

std::vector<MeshInBuffer> AddModel(v3 position, v3 size, const std::string& model_name, v4 color = { 0.0f, 0.0f, 0.0f, 1.0f }, f32 rotation = 0.0f, f32 scale = 1.0f)
{
    Model* model = RM.GetModel(model_name);

    std::vector<MeshInBuffer> meshes;

    for (auto& mesh : model->meshes) {
        InstancedMeshBuffer* buffer;
        buffer = R.GetBuffer(mesh.id);
        if (buffer == nullptr)
            buffer = R.CreateBuffer(mesh);

        auto mesh_in_buffer = buffer->AddMesh(position, size, color, rotation, scale);
        mesh_in_buffer.buffer_index = R.buffers.size() - 1;
        meshes.push_back(mesh_in_buffer);
    }

    assert(meshes.size() > 0);

    return meshes;
}

void DrawSupportPackage(WindowManager* window, SupportPackage& package, u32 package_index)
{
    Country& country = GetCountryByCode(package.origin);
    IconParams icon;
    icon.index = country.index_in_atlas;
    icon.size = v2(64);
    icon.scale = 1.0;
    icon.padding = 0;

	UI.Stack(Direction::Horizontal, 20);
		UI.DrawLabel(std::format("Delivery in {} turns", package.delivered_on_turn - STATE.current_turn));
		for (Delivery& d : package.delivery) {
			UI.DrawLabel(std::format("{}x {}", d.n, d.system.name));
		}
		UI.DrawIcon(icon);
  UI.EndLayout();
}

void DrawWeaponsInReserve(WindowManager* window) {
	if (STATE.weapons_in_reserve.size() == 0) return;

	UI.Stack(Direction::Vertical, 20);
		for (Delivery& d : STATE.weapons_in_reserve) {
			UI.DrawLabel(std::format("{}x {} {}/{}", d.n, d.system.name, d.system.attack, d.system.defence));
		}
	UI.EndLayout();
}

void DrawDeliveriesUI(WindowManager* window)
{
    u32 i = 0;
    UI.Stack(Direction::Vertical, 40);
		for (SupportPackage& package : STATE.promised_support) {
			if (package.fully_delivered) continue;
			DrawSupportPackage(window, package, i);
			i++;
		}
    UI.EndLayout();
}

void DrawSupportingCountries(WindowManager* window)
{
    auto y = window->screen_size.y - top_offset;
    auto x = 120;

    UI.Stack(Direction::Horizontal, 20, { x, y });
		for (auto& country : STATE.countries) {
			IconParams icon;
			icon.size = v2(64);
			icon.scale = 0.6;
			icon.padding = 25;
			icon.index = country.index_in_atlas;
			UI.DrawIconAndLabel(icon, std::format("{}: {}%", country.name, country.support * 100));
		}
    UI.EndLayout();
}

//TODO: move to STATE init();
void AddSupportingCountries() {
	STATE.countries.push_back(Country(CountryCode::FR, "France", 0.7, 1));
  STATE.countries.push_back(Country(CountryCode::DE, "Germany", 0.856, 2));
  STATE.countries.push_back(Country(CountryCode::PL, "Poland", 1.0, 3));
  STATE.countries.push_back(Country(CountryCode::USA, "United States", 1.0, 4));
  STATE.countries.push_back(Country(CountryCode::UK, "United Kingdom", 0.95, 0));
}

void AddMap()
{
    int i = 0;
    Model* map_model = RM.GetModel("map2");

    for (auto& mesh : map_model->meshes) {
        if (i > NUMBER_OF_OBLASTS - 1)
            continue;

				//Create entity
        auto code = static_cast<OblastCode>(i);
        auto control = INITIAL_CONTROL.at(code);
        ID id = E.CreateOblast(Oblast(static_cast<OblastCode>(i), OBLAST_NAMES.at(code), control));

				// Create buffer for mesh or use existing one
				InstancedMeshBuffer* buffer;
        buffer = R.GetBuffer(mesh.id);
        if (buffer == nullptr)
            buffer = R.CreateBuffer(mesh);

				// Add instance to instanced mesh rendering
				auto mesh_in_buffer = buffer->AddMesh({0, 2, 0}, GREY, static_cast<i32>(id));
        mesh_in_buffer.buffer_index = R.buffers.size() - 1;

				E.GetEntityById(id)->oblast.SetMesh(mesh_in_buffer);

        i++;
    }
}

void DrawOblastInfo() {
	Oblast* oblast = GetOblast(STATE.selected_oblast);
	UI.DrawLabel(std::format("{} - {:.2f}%", oblast->name, oblast->ukrainian_control * 100));
}

void DrawUI(WindowManager* window) {
	u32 y = window->screen_size.y - top_offset - HEADER_H - 120;
  constexpr u32 x = 20;

	DrawSupportingCountries(window);
	
	UI.Stack(Direction::Vertical, 100, {x, y});
		DrawDeliveriesUI(window);
		DrawWeaponsInReserve(window);
		DrawOblastInfo();
	UI.EndLayout();

	DrawResources(window);
	DrawTurnUI(window);
}
