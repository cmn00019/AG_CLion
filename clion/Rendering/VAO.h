/*   Copyright (C) 2023 Lidia Ortega Alvarado, Alfonso Lopez Ruiz
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see https://github.com/AlfonsoLRz/AG2223.
*/

#pragma once

#define RESTART_PRIMITIVE_INDEX 0xFFFFFFFF

namespace AlgGeom
{
	class VAO
	{
	public:
		enum VBO_slots
		{
			VBO_POSITION, VBO_NORMAL, VBO_TEXT_COORD, VBO_TANGENT, VBO_BITANGENT,
			VBO_MULTI_POSITION,
			VBO_MULTI_COLOR,
			NUM_VBOS
		};

		enum IBO_slots
		{
			IBO_POINT,
			IBO_LINE,
			IBO_TRIANGLE,
			NUM_IBOS
		};

		struct Vertex
		{
			vec3		_position, _normal;
			vec2		_textCoord;
		};

	private:
		GLuint				_vao;
		std::vector<GLuint> _vbos;
		std::vector<GLuint> _ibos;

	private:
		static void defineInterleavedVBO(GLuint vboId);
		static void defineNonInterleaveVBO(GLuint vboId, size_t structSize, GLuint elementType, uint8_t slot);

	public:
		VAO(bool interleaved = true);
		virtual ~VAO();

		void drawObject(IBO_slots ibo, GLuint openGLPrimitive, GLuint numIndices) const;
		void drawObject(IBO_slots ibo, GLuint openGLPrimitive, GLuint numIndices, GLuint numInstances) const;
		template<typename T, typename Z>
		int defineMultiInstancingVBO(VBO_slots vbo, const T dataExample, const Z dataPrimitive, const GLuint openGLBasicType);

		template<typename T>
		void setVBOData(VBO_slots vbo, T* geometryData, GLuint size, GLuint changeFrequency = GL_STATIC_DRAW);
		void setVBOData(const std::vector<Vertex>& vertices, GLuint changeFrequency = GL_STATIC_DRAW) const;
		void setIBOData(IBO_slots ibo, const std::vector<GLuint>& indices, GLuint changeFrequency = GL_STATIC_DRAW) const;
	};

	template<typename T, typename Z>
	inline int VAO::defineMultiInstancingVBO(VBO_slots vbo, const T dataExample, const Z dataPrimitive, const GLuint openGLBasicType)
	{
		glBindVertexArray(_vao);
		glGenBuffers(1, &_vbos[vbo]);
		glBindBuffer(GL_ARRAY_BUFFER, _vbos[vbo]);
		glEnableVertexAttribArray(vbo);
		glVertexAttribPointer(vbo, sizeof(dataExample) / sizeof(dataPrimitive), openGLBasicType, GL_FALSE, sizeof(dataExample), (GLubyte*)nullptr);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glVertexAttribDivisor(vbo, 1);

		return vbo;
	}

	template<typename T>
	inline void VAO::setVBOData(VBO_slots vbo, T* geometryData, GLuint size, GLuint changeFrequency)
	{
		glBindVertexArray(_vao);
		glBindBuffer(GL_ARRAY_BUFFER, _vbos[vbo]);
		glBufferData(GL_ARRAY_BUFFER, size * sizeof(T), geometryData, changeFrequency);
	}
}