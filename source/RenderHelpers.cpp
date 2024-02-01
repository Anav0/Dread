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



void AddTurnUI(LabelHandle& ui_date, ButtonHandle& ui_button_in_buffer) {
	UI.Stack(Direction::Vertical, 10, { 120, GetScreenSize().y - 300 });
		auto initial_date = GetDateStr();
		ui_date             = UI.DrawLabel(initial_date);
		ui_button_in_buffer = UI.DrawBtn("Next Turn", 36, []() { GoToNextPhase(); });
	UI.EndLayout();
}

void AddResources(u8 font_size, LabelHandle& ui_popular_support, LabelHandle& ui_reserve)
{
    IconParams sword_icon, megaphone_icon;
    sword_icon.size = v2(64);
    sword_icon.scale = 0.6;
    sword_icon.padding = 25;
    megaphone_icon = sword_icon;

    megaphone_icon.index = MEGAPHONE;
    sword_icon.index    = SWORD;

	auto y = STATE.window.screen_size.y - top_offset;
	auto x = STATE.window.screen_size.x - 600;

	UI.Stack(Direction::Horizontal, 10, { x, y });
		ui_reserve         = UI.DrawIconAndLabel(sword_icon, std::format("Reserve: {}", STATE.reserve), { 200, 400 }, font_size);
		ui_popular_support = UI.DrawIconAndLabel(megaphone_icon, std::format("Popular support: {}%", STATE.popular_support), { 200, 500 }, font_size);
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

void AddSupportingCountries(u8 font_size) {
	STATE.countries.push_back(Country(CountryCode::FR, "France", 0.7, 1));
	STATE.countries.push_back(Country(CountryCode::DE, "Germany", 0.856, 2));
	STATE.countries.push_back(Country(CountryCode::PL, "Poland", 1.0, 3));
	STATE.countries.push_back(Country(CountryCode::USA, "United States", 1.0, 4));
	STATE.countries.push_back(Country(CountryCode::UK, "United Kingdom", 0.95, 0));

	auto y = STATE.window.screen_size.y - top_offset;
	auto x = 20;

	UI.Stack(Direction::Horizontal, 20, { x, y });
		for (auto& country : STATE.countries) {
			IconParams icon;
			icon.size = v2(64);
			icon.scale = 0.6;
			icon.padding = 25;

			icon.index = country.index_in_atlas;

			UI.DrawIconAndLabel(icon, std::format("{}: {}%", country.name, country.support*100), { 200, 400 }, font_size);
		}
	UI.EndLayout();
}

void AddMap() {
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
