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

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "PointCloud3d.h"
#include "Triangle3d.h"
#include "Vect2d.h"
#include "Vect3d.h"

#define BINARY_EXTENSION ".bin"

class TriangleModel 
{    
private:
    std::vector<Vect3d>     _vertices; 
    std::vector<Vect3d>     _normals;
    std::vector<Vect2d>     _textCoordinates;
    std::vector<unsigned>   _indices;  

protected:
    Assimp::Importer        _assimpImporter;

protected:
    void loadModelBinaryFile(const std::string& path);
    void processMesh(aiMesh* mesh, const aiScene* scene, const std::string& folder);
    void processNode(aiNode* node, const aiScene* scene, const std::string& folder);
    void writeBinaryFile(const std::string& path);
    
public:
    TriangleModel(const std::string& pathfile);
    TriangleModel(const TriangleModel& orig) = delete;
    virtual ~TriangleModel();

    PointCloud3d getCloud();
    Triangle3d getFace(unsigned index);
    std::vector<Triangle3d> getFaces();
    std::vector<Vect3d>* getVertices() { return &_vertices; }
    std::vector<Vect3d>* getNormals() { return &_normals; }
    std::vector<Vect2d>* getTextureCoordinates() { return &_textCoordinates; }
    std::vector<unsigned>* getIndices() { return &_indices; }
    size_t numTriangles();
};

