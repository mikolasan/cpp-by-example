#include "neuron_render.hpp"

bgfx::VertexLayout PosColorVertex::ms_layout;

bgfx::VertexBufferHandle NeuronRenderStrategy::m_vbh;
bgfx::IndexBufferHandle NeuronRenderStrategy::m_ibh;

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