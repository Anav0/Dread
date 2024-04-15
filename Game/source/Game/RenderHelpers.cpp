#include "Engine/Gui.h"
#include "Engine/Misc.h"
#include "Engine/Model.h"
#include "Engine/Renderer.h"
#include "Engine/ResourceManager.h"
#include "Engine/TextRenderer.h"
#include <Engine/Collision.h>

#include "Atlas.h"
#include "Entities.h"
#include "EntityManager.h"
#include "RenderHelpers.h"

#include "stb_truetype.h"

std::vector<MeshInBuffer> AddModel(v3 position, const std::string& model_name, v4 color, f32 rotation, f32 scale)
{

    Model* model = RM.GetModel(model_name);

    std::vector<MeshInBuffer> meshes;

    u32 i = 0;
    for (auto& mesh : model->meshes) {
        InstancedMeshBuffer* buffer;
        buffer = R.GetBuffer(mesh.id);
        if (buffer == nullptr) {
            buffer = R.CreateBuffer(1, mesh);
        }

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
    v2 header_pos = v2(0, window->screen_size.y - HEADER_H);

    u32 i = static_cast<u32>(STATE.phase);

    CenterChildInParentY(&header_pos, &header_size, &pos, &size);
    UI.Stack(Direction::Horizontal, 10, { window->screen_size.x - 430, pos.y });
    UI.DrawLabel(STATE.game_date_str);
    UI.Spacer(30);
    UI.DrawLabel(PHASES[i]);
    UI.EndLayout();
    if (UI.DrawIcon(END_TURN_ICON, { window->screen_size.x - 100, pos.y - 45 })) {
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
    sword_icon.index = SWORD;

    u32 y = CenterChildInParentY(window->screen_size.y - HEADER_H, HEADER_H, sword_icon.size.y);
    u32 x = 10;

    UI.Stack(Direction::Horizontal, 10, { x, y });
    UI.DrawIcon(UKRAINE_EMBLEM, { 0, 0 }, { 0, -75 });
    UI.DrawIconAndLabel(UI_BUFFER_INFO, sword_icon, std::format("Reserve: {}", STATE.reserve));
    UI.DrawIconAndLabel(UI_BUFFER_INFO, megaphone_icon, std::format("Popular support: {}%", STATE.popular_support));
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
            buffer = R.CreateBuffer(1, mesh);

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

void DrawWeaponsInReserve(WindowManager* window)
{
    if (STATE.weapons_in_reserve.size() == 0)
        return;

    UI.Stack(Direction::Vertical, 20);
    for (Delivery& d : STATE.weapons_in_reserve) {
        // UI.DrawLabel(std::format("{}x {} {}/{}", d.n, d.system.name, d.system.attack, d.system.defence));
    }
    UI.EndLayout();
}

void DrawDeliveriesUI(WindowManager* window)
{
    u32 i = 0;
    UI.Stack(Direction::Vertical, 40);
    for (SupportPackage& package : STATE.promised_support) {
        if (package.fully_delivered)
            continue;
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
        UI.DrawIconAndLabel(UI_BUFFER_INFO, icon, std::format("{}: {}%", country.name, country.support * 100));
    }
    UI.EndLayout();
}

// TODO: move to STATE init();
void AddSupportingCountries()
{
    STATE.countries.push_back(Country(CountryCode::FR, "France", 0.7, 1));
    STATE.countries.push_back(Country(CountryCode::DE, "Germany", 0.856, 2));
    STATE.countries.push_back(Country(CountryCode::PL, "Poland", 1.0, 3));
    STATE.countries.push_back(Country(CountryCode::USA, "United States", 1.0, 4));
    STATE.countries.push_back(Country(CountryCode::UK, "United Kingdom", 0.95, 0));
}

void AddWeapons(Unit& unit, std::string weapon_name, u32 n)
{
    auto weapon_index = STATE.armory.GetWeaponIndexByName(weapon_name);
    assert(weapon_index.has_value());

    u32 index = 0;
    bool found_same_weapon = false;
    for (auto existing_weapon_index : unit.weapons) {
        if (existing_weapon_index  == weapon_index) {
            found_same_weapon = true;
            break;
        }
        index++;
    }

    if (found_same_weapon) {
        unit.weapons_counter[index] += n;
    } else {
        unit.weapons.push_back(*weapon_index);
        unit.weapons_counter.push_back(n);
    }

    unit.morale.push_back(1.0f);
}

void AddMap()
{
    int i = 0;
    Model* map_model = RM.GetModel("map");

    v3 position = { 0, 0, 0 };
    for (auto& mesh : map_model->meshes) {
        if (i > NUMBER_OF_OBLASTS - 1)
            continue;

        // Create entity
        auto code = static_cast<OblastCode>(i);
        auto control = INITIAL_CONTROL.at(code);
        std::string name = OBLAST_NAMES.GetValue(code);
        ID id = E.CreateOblast(Oblast(static_cast<OblastCode>(i), position, name.c_str(), control));

        // Create buffer for mesh or use existing one
        InstancedMeshBuffer* buffer;
        buffer = R.GetBuffer(mesh.id);
        if (buffer == nullptr)
            buffer = R.CreateBuffer(1, mesh);

        // Add instance to instanced mesh rendering
        auto mesh_in_buffer = buffer->AddMesh(position, GREY, static_cast<i32>(id));
        mesh_in_buffer.buffer_index = R.buffers.size() - 1;

        E.GetEntityById(id)->oblast.SetMesh(mesh_in_buffer);

        i++;
    }
}

void DrawOblastInfo()
{
    Oblast* oblast = GetOblast(STATE.selected_oblast);
    UI.DrawLabel(std::format("{} - {:.2f}%", oblast->name, oblast->ukrainian_control * 100));
}

void DrawUI(WindowManager* window)
{
    u32 y = window->screen_size.y - top_offset - HEADER_H - 120;
    constexpr u32 x = 20;

    DrawSupportingCountries(window);

    UI.Stack(Direction::Vertical, 100, { x, y });
    DrawDeliveriesUI(window);
    DrawWeaponsInReserve(window);
    DrawOblastInfo();
    UI.EndLayout();

    DrawResources(window);
    DrawTurnUI(window);
}

void DrawUnitToken(Oblast* oblast, Unit& unit)
{
    static v2 pos = { 50, 50 };
    static v2 size = { 115, 50 };

    static const Gradient gradient {
        GradientType::Radial,
        v2(0.0, 0.0), // Middle
        { 250.0, 60.0 }, // Radial pos
        v2(0.0, 1.0), // Smoothing
        5.8, // Factor
        { YELLOW, BLACK },
    };

    IconParams icon;
    icon.size = INSIGNIA_SIZE;
    icon.scale = 0.2;
    icon.padding = 0;
    icon.index = AIR_92;

    v2 icon_pos = { pos.x + 5, 0 };
    CenterChildInParentY(&pos, &size, &icon_pos, &icon.size);

    R.gradient_buffer.AddGradient(pos, size, gradient);
    UI.DrawIconAndLabel(INSIGNIA_BUFFER_INFO, icon, unit.name);
}

void DrawDeployedUnits()
{
    u32 i = 0;
    for (auto& c : STATE.troops_deployment.ukr_assigned) {
        Unit& unit = STATE.troops_deployment.ukr_units.at(i);
        Oblast* oblast = GetOblast(c);

        if (STATE.selected_oblast == oblast->code) {
            DrawUnitToken(oblast, unit);
        }

        i++;
    }
}
