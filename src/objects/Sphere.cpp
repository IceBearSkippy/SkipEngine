#pragma once
#include <objects/Sphere.h>
#include <cmath>

namespace Skip {

    Sphere::Sphere()
        : SkipObject() {

    }

    Sphere::Sphere(glm::vec3 position, int precision, std::string texturePath)
        : SkipObject(position, texturePath) {
		_precision = precision;

    }

    Sphere::~Sphere() {
    }

	void Sphere::loadObject() {
		//calculate triangle vertices
		for (int i = 0; i < _precision; i++) {
			for (int j = 0; j < _precision; j++) {
				float y = (float)cos(toRadians(180.0f - i * 180.0f / _precision));
				float x = -(float)cos(toRadians(j * 360.0f / _precision)) * (float)abs(cos(asin(y)));
				float z = (float)sin(toRadians(j * 360.0f / _precision)) * (float)abs(cos(asin(y)));

				Vertex vertex{};
				vertex.pos = glm::vec3(x, y, z);
				vertex.texCoord = glm::vec2(((float)j / _precision), ((float)i / _precision));
				vertex.normal = glm::vec3(x, y, z);
				vertex.color = glm::vec3(1.0f, 1.0f, 1.0f);

				// calculate tangent vector
				if (((x == 0) && (y == 1) && (z == 0)) || ((x == 0) && (y == -1) && (z == 0))) {
					// if north or south pole,
					// set tangent to -Z axis
					vertex.tangent = glm::vec3(0.0f, 0.0f, -1.0f);
				} else {
					// otherwise, calculate tangent
					vertex.tangent = glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(x, y, z));
				}

				if (_uniqueVertices.count(vertex) == 0) {
					_uniqueVertices[vertex] = static_cast<uint32_t>(_vertices.size());
					_vertices.push_back(vertex);
				}
				_indices.push_back(_uniqueVertices[vertex]);
			}
		}


	}

}
