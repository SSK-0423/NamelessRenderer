#pragma once
#include "SphereGeometry.h"

struct SkySphereData {
	unsigned int stackNum;
	unsigned int sectorNum;
	float radius;
	const std::wstring& texturePath;
};

class SkySphere {
public:
	SkySphere();
	~SkySphere();

private:
	SphereGeometry sphere;
	Texture texture;
	MYRESULT CreateTexture(const std::wstring& texturePath);

public:
	MYRESULT Create(ID3D12Device& device, SkySphereData& data);

	void Draw(RenderingContext& renderContext);
};