#pragma once

enum class EShaderType
{
	Vertex,
	Pixel,
};

enum class EViewMode
{
	Lit,
	Unlit,
	Wireframe,
	DepthView,
	WorldNormal,
};

enum class ELightingModel
{
	Gouraud,
	Lambert,
	BlinnPhong,
};

struct FViewModeSettings
{
	EViewMode Mode = EViewMode::Lit;
	ELightingModel LightingModel = ELightingModel::BlinnPhong;
};
