#pragma once
#include <objects/Sphere.h>
#include <cmath>

namespace Skip {

    Sphere::Sphere()
        : SkipObject() {

    }

    Sphere::Sphere(glm::vec3 position, int precision, std::string texturePath, bool useIndexBuffer)
        : SkipObject(position, texturePath, useIndexBuffer) {
		_precision = precision;

    }

    Sphere::~Sphere() {
    }

	void Sphere::loadObject(float aspect) {
		int numVertices = (_precision + 1) * (_precision + 1);
		int numIndices = _precision * _precision * 6;
		std::vector<Vertex> temp_vertices;
		for (int i = 0; i < numIndices; i++) {
			_indices.push_back(0);
		}
		//calculate triangle indices
		for (int i = 0; i < _precision; i++) {
			for (int j = 0; j < _precision; j++) {
				_indices[6 * (i * _precision + j) + 0] = i * (_precision + 1) + j;
				_indices[6 * (i * _precision + j) + 1] = i * (_precision + 1) + j + 1;
				_indices[6 * (i * _precision + j) + 2] = (i + 1) * (_precision + 1) + j;
				_indices[6 * (i * _precision + j) + 3] = i * (_precision + 1) + j + 1;
				_indices[6 * (i * _precision + j) + 4] = (i + 1) * (_precision + 1) + j + 1;
				_indices[6 * (i * _precision + j) + 5] = (i + 1) * (_precision + 1) + j;
			}
		}

		for (int i = 0; i < numVertices; i++) {
			temp_vertices.push_back(Vertex{});
		}
		for (int i = 0; i <= _precision; i++) {
			for (int j = 0; j <= _precision; j++) {
				float y = (float)cos(toRadians(180.0f - i * 180.0f / _precision));
				float x = -(float)cos(toRadians(j * 360.0f / _precision)) * (float)abs(cos(asin(y)));
				float z = (float)sin(toRadians(j * 360.0f / _precision)) * (float)abs(cos(asin(y)));

				temp_vertices[i * (_precision + 1) + j].position = glm::vec3(x, y, z);
				temp_vertices[i * (_precision + 1) + j].texCoord = glm::vec2(((float)j / _precision), ((float)i / _precision));
				temp_vertices[i * (_precision + 1) + j].normal = glm::vec3(x, y, z);
				temp_vertices[i * (_precision + 1) + j].color = glm::vec3(1.0f, 1.0f, 1.0f);

				// calculate tangent vector
				if (((x == 0) && (y == 1) && (z == 0)) || ((x == 0) && (y == -1) && (z == 0))) {
					// if north or south pole,
					// set tangent to -Z axis
					temp_vertices[i * (_precision + 1) + j].tangent = glm::vec3(0.0f, 0.0f, -1.0f);
				} else {
					// otherwise, calculate tangent
					temp_vertices[i * (_precision + 1) + j].tangent = glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(x, y, z));
				}

			}
		}
		if (_useIndexBuffer) {
			_vertices = temp_vertices;
		} else {
			for (int i = 0; i < _indices.size(); i++) {
				_vertices.push_back(temp_vertices[_indices[i]]);
			}
		}
		
	}

}
