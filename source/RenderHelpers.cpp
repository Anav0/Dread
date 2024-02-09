#include "RenderHelpers.h"
#include "Base.h"
#include "BoundingBox.h"
#include "Collision.h"
#include "EntityManager.h"
#include "Gui.h"
#include "Line.h"
#include "Model.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "TextRenderer.h"

#include "stb_truetype.h"

std::vector<MeshInBuffer> AddModel(v3 position, std::string model_name, v4 color, f32 rotation, f32 scale)
{
    Model* model = RM.GetModel(model_name);

    std::vector<MeshInBuffer> meshes;

    for (auto& mesh : model->meshes) {
        InstancedMeshBuffer* buffer;
        buffer = R.GetBuffer(mesh.id);
        if (buffer == nullptr)
            buffer = R.CreateBuffer(mesh);

        auto mesh_in_buffer = buffer->AddMesh(position, color, rotation, scale);
        mesh_in_buffer.buffer_index = R.buffers.size() - 1;
        meshes.push_back(mesh_in_buffer);
    }

    assert(meshes.size() > 0);

    return meshes;
}

constexpr u16 top_offset = 80;

void DrawTurnUI()
{
    UI.Stack(Direction::Horizontal, 10, { GetScreenSize().x - 350, GetScreenSize().y - 125 });
    	auto initial_date = GetDateStr();
    	UI.DrawLabel(initial_date, 24);
		if (UI.DrawButton("Next Turn")) {
			GoToNextPhase();
		}
    UI.EndLayout();
}

void DrawResources()
{
    IconParams sword_icon, megaphone_icon;
    sword_icon.size = v2(64);
    sword_icon.scale = 0.6;
    sword_icon.padding = 25;
    megaphone_icon = sword_icon;

    megaphone_icon.index = MEGAPHONE;
    sword_icon.index = SWORD;

    auto y = STATE.window.screen_size.y - top_offset;
    auto x = STATE.window.screen_size.x - 600;

    UI.Stack(Direction::Horizontal, 10, { x, y });
		UI.DrawIconAndLabel(sword_icon, std::format("Reserve: {}", STATE.reserve));
		UI.DrawIconAndLabel(megaphone_icon, std::format("Popular support: {}%", STATE.popular_support));
    UI.EndLayout();
}

std::vector<MeshInBuffer> AddModel(v3 position, v3 size, std::string model_name, v4 color = { 0.0f, 0.0f, 0.0f, 1.0f }, f32 rotation = 0.0f, f32 scale = 1.0f)
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

void DrawSupportPackage(SupportPackage& package, u32 package_index)
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

void DrawWeaponsInReserve() {
	if (STATE.weapons_in_reserve.size() == 0) return;

	UI.Stack(Direction::Vertical, 20);
		for (Delivery& d : STATE.weapons_in_reserve) {
			UI.DrawLabel(std::format("{}x {} {}/{}", d.n, d.system.name, d.system.attack, d.system.defence));
		}
	UI.EndLayout();
}

void DrawDeliveriesUI()
{
    u32  i = 0;
    UI.Stack(Direction::Vertical, 60);
		for (SupportPackage& package : STATE.promised_support) {
			if (package.fully_delivered) continue;
			DrawSupportPackage(package, i);
			i++;
		}
    UI.EndLayout();
}

void DrawSupportingCountries()
{
    auto y = STATE.window.screen_size.y - top_offset;
    auto x = 20;

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
    for (auto& mesh : AddModel({ 0, 0, 0 }, "map", GREY, 0, 1)) {
        if (i > NUMBER_OF_OBLASTS - 1)
            continue;

        auto code = static_cast<OblastCode>(i);
        auto control = INITIAL_CONTROL.at(code);

        ID id = E.CreateOblast(Oblast(mesh, static_cast<OblastCode>(i), OBLAST_NAMES.at(code), control));
        AddBoundingBox(mesh, id);
        i++;
    }
}

void DrawUI() {
	auto y = STATE.window.screen_size.y - top_offset - 200;
    auto x = 20;

	DrawSupportingCountries();
	UI.Stack(Direction::Vertical, 100, {x, y});
		DrawDeliveriesUI();
		DrawWeaponsInReserve();
	UI.EndLayout();
	DrawResources();
	DrawTurnUI();
}