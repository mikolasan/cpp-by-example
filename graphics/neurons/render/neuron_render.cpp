#include "neuron_render.hpp"
#include "pos_color_vertex.h"
#include "t_phi_vertex.h"

bgfx::VertexLayout PosColorVertex::ms_layout;
bgfx::VertexLayout TPhiVertex::kTubeLayout;

bgfx::VertexBufferHandle NeuronRenderStrategy::m_vbh;
bgfx::IndexBufferHandle NeuronRenderStrategy::m_ibh;

std::vector<TPhiVertex> NeuronRenderStrategy::m_tube_vertices;
std::vector<uint16_t> NeuronRenderStrategy::m_tube_indices;
bgfx::VertexBufferHandle NeuronRenderStrategy::m_tube_vbh;
bgfx::IndexBufferHandle NeuronRenderStrategy::m_tube_ibh;

namespace
{

	// // cube

	PosColorVertex s_cubeVertices[8] =
	{
		{-1.0f,  1.0f,  1.0f, 0xff000000 },
		{ 1.0f,  1.0f,  1.0f, 0xff0000ff },
		{-1.0f, -1.0f,  1.0f, 0xff00ff00 },
		{ 1.0f, -1.0f,  1.0f, 0xff00ffff },
		{-1.0f,  1.0f, -1.0f, 0xffff0000 },
		{ 1.0f,  1.0f, -1.0f, 0xffff00ff },
		{-1.0f, -1.0f, -1.0f, 0xffffff00 },
		{ 1.0f, -1.0f, -1.0f, 0xffffffff },
	};

	const uint16_t s_cubeTriList[] =
	{
		0, 1, 2, // 0
		1, 3, 2,
		4, 6, 5, // 2
		5, 6, 7,
		0, 2, 4, // 4
		4, 2, 6,
		1, 5, 3, // 6
		5, 7, 3,
		0, 4, 1, // 8
		4, 5, 1,
		2, 3, 6, // 10
		6, 3, 7,
	};


}

void NeuronRenderStrategy::init_once() {


	// std::vector<PosColorVertex> vertices;
	// std::vector<uint16_t> indices;
	// generateSphereMesh(vertices, indices);

	// std::cout << "vertices" << std::endl;
	// for (const auto& v : vertices) {
	//   std::cout << "(" << v.m_x << ", "
	//     << v.m_y << ", "
	//     << v.m_z << ") ";
	// }
	// std::cout << std::endl;

	// std::cout << "indices" << std::endl;
	// for (const auto& i : indices) {
	//   std::cout << i << " ";
	// }
	// std::cout << std::endl;

	// // Create static vertex buffer.
	// m_vbh = bgfx::createVertexBuffer(
	//   bgfx::makeRef(vertices.data(), uint32_t(vertices.size() * sizeof(PosColorVertex)))
	//   , PosColorVertex::ms_layout
	// );

	// // Create static index buffer.
	// m_ibh = bgfx::createIndexBuffer(
	//   bgfx::makeRef(indices.data(), uint32_t(indices.size() * sizeof(uint16_t)))
	// );

	m_vbh = bgfx::createVertexBuffer(
		// Static data can be passed with bgfx::makeRef
		bgfx::makeRef(s_cubeVertices, sizeof(s_cubeVertices))
		, PosColorVertex::ms_layout
	);

	// Create static index buffer for triangle list rendering.
	m_ibh = bgfx::createIndexBuffer(
		// Static data can be passed with bgfx::makeRef
		bgfx::makeRef(s_cubeTriList, sizeof(s_cubeTriList))
	);

}

void NeuronRenderStrategy::generateSphereMesh(
	std::vector<PosColorVertex>& vertices,
	std::vector<uint16_t>& indices,
	int stacks, // vertical segments (horizontal lines make stack of pancakes)
	int slices, // horizontal segments (vertical lines make slices like with apples or oranges)
	float radius)
{
	vertices.clear();
	vertices.reserve((stacks + 1) * (slices + 1));
	indices.clear();

	for (int i = 0; i <= stacks; ++i)
	{
		// going from top to bottom

		float v = float(i) / stacks;
		float phi = v * bx::kPi; // 0 to π

		for (int j = 0; j <= slices; ++j)
		{
			float u = float(j) / slices;
			float theta = u * bx::kPi * 2.0f; // 0 to 2π

			float x = sinf(phi) * cosf(theta);
			float y = cosf(phi);
			float z = sinf(phi) * sinf(theta);

			PosColorVertex vtx;
			vtx.m_x = x * radius;
			vtx.m_y = y * radius;
			vtx.m_z = z * radius;

			// Color based on position for debugging
			uint8_t r = uint8_t((x * 0.5f + 0.5f) * 255);
			uint8_t g = uint8_t((y * 0.5f + 0.5f) * 255);
			uint8_t b = uint8_t((z * 0.5f + 0.5f) * 255);
			vtx.m_abgr = (r) | (g << 8) | (b << 16) | 0xff000000;

			vertices.push_back(vtx);
		}
	}

	// Generate indices

	//   V03----------V02----------V01---------V00 
	//    |            |        __/ |        __/|
	//    |            |     __/    |  c  __/   |
	//    |            |  __/       |  __/      |
	//    |            | /        f | /     d   |
	//   V13----------V12----------V11----------V10 
	//    |            |        __/ | e      __/|
	//    |            |  a  __/    |     __/   |
	//    |            |  __/       |  __/      |
	//    |            | /      b   | /         |
	//   V23----------V22----------V21----------V20 
	//    |            |            |           |
	//    |            |            |           |
	//    |            |            |           |
	//    |            |            |           |
	//   V33----------V32----------V31---------V30 

	for (int i = 0; i < stacks; ++i)
	{
		// going from top to bottom

		for (int j = 0; j < slices; ++j)
		{
			int row1 = i * (slices + 1);
			int row2 = (i + 1) * (slices + 1);

			uint16_t a = row1 + j;
			uint16_t b = row2 + j;
			uint16_t c = row2 + j + 1;
			uint16_t d = row1 + j + 1;

			// // triangle 1
			// indices.push_back(uint16_t(row1 + j));
			// indices.push_back(uint16_t(row2 + j));
			// indices.push_back(uint16_t(row2 + j + 1));
			// // triangle 2
			// indices.push_back(uint16_t(row1 + j));
			// indices.push_back(uint16_t(row2 + j + 1));
			// indices.push_back(uint16_t(row1 + j + 1));
			// = one quad

			// // // Triangle 1 (a, b, c)
			// indices.push_back(a);
			// indices.push_back(b);
			// indices.push_back(c);

			// // Triangle 2 (a, c, d)
			// indices.push_back(a);
			// indices.push_back(c);
			// indices.push_back(d);

			// Triangle 1 (flip b, a, c)
			indices.push_back(b);
			indices.push_back(a);
			indices.push_back(c);

			// // Triangle 2 (flip c, a, d)
			indices.push_back(c);
			indices.push_back(a);
			indices.push_back(d);
		}
	}
}

void NeuronRenderStrategy::buildUnitTubeGrid(uint32_t segT, uint32_t segPhi)
{
	m_tube_vertices.reserve((segT + 1) * (segPhi + 1));
	for (uint32_t i = 0;i <= segT;++i) {
		float t = float(i) / float(segT);
		for (uint32_t j = 0;j <= segPhi;++j) {
			float phi = (2.0f * bx::kPi * float(j)) / float(segPhi);
			m_tube_vertices.push_back({ t, phi });
		}
	}

	auto row = segPhi + 1;
	for (uint32_t i = 0;i < segT;++i) {
		for (uint32_t j = 0;j < segPhi;++j) {
			uint16_t a = uint16_t(i * row + j);
			uint16_t b = uint16_t((i + 1) * row + j);
			uint16_t c = uint16_t((i + 1) * row + (j + 1));
			uint16_t d = uint16_t(i * row + (j + 1));

			// two triangles per cell
			m_tube_indices.push_back(a);
			m_tube_indices.push_back(b);
			m_tube_indices.push_back(c);

			m_tube_indices.push_back(a);
			m_tube_indices.push_back(c);
			m_tube_indices.push_back(d);
		}
	}

  bgfx::VertexLayout tubeLayout;
	tubeLayout.begin()
		.add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float) // (t, phi)
		.end();

	// m_tube_vbh = bgfx::createVertexBuffer(
	// 	// Static data can be passed with bgfx::makeRef
	// 	bgfx::makeRef(m_tube_vertices.data(), m_tube_vertices.size())
	// 	, tubeLayout
	// );

	// // Create static index buffer for triangle list rendering.
	// m_tube_ibh = bgfx::createIndexBuffer(
	// 	// Static data can be passed with bgfx::makeRef
	// 	bgfx::makeRef(m_tube_indices.data(), m_tube_indices.size())
	// );

	static TPhiVertex verts[4] = {
		{0.0f, 0.0f},
		{1.0f, 0.0f},
		{1.0f, 1.0f},
		{0.0f, 1.0f}
	};
	static const uint16_t indices[6] = {0,1,2, 0,2,3};

	m_tube_vbh = bgfx::createVertexBuffer(bgfx::makeRef(verts, sizeof(verts)), tubeLayout);
	m_tube_ibh = bgfx::createIndexBuffer(bgfx::makeRef(indices, sizeof(indices)));
	
}