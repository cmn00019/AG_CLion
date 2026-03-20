#include "stdafx.h"
#include "TriangleModel.h"
#include "Octree.h"

TriangleModel::TriangleModel(const std::string& pathfile) : _octree(nullptr)
{
    std::string binaryFile = pathfile.substr(0, pathfile.find_last_of('.')) + BINARY_EXTENSION;

    if (std::filesystem::exists(binaryFile))
    {
        this->loadModelBinaryFile(binaryFile);
    }
    else
    {
        const aiScene* scene = _assimpImporter.ReadFile(pathfile, aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            std::cout << "ERROR::ASSIMP::" << _assimpImporter.GetErrorString() << std::endl;
            return;
        }

        std::string shortName = scene->GetShortFilename(pathfile.c_str());
        std::string folder = pathfile.substr(0, pathfile.length() - shortName.length());

        this->processNode(scene->mRootNode, scene, folder);
        this->writeBinaryFile(binaryFile);
    }
}

TriangleModel::~TriangleModel() 
{
}

/*
PointCloud3d TriangleModel::getCloud()
{
    return PointCloud3d(_vertices);

}
 */

Triangle3d TriangleModel::getFace(unsigned index) 
{
    if (index > this->numTriangles()) 
        return Triangle3d();  
    
    Vect3d a = _vertices[_indices[index * 3]];
    Vect3d b = _vertices[_indices[index * 3 + 1]];
    Vect3d c = _vertices[_indices[index * 3 + 2]];
    
    return Triangle3d(a, b, c);
}

std::vector<Triangle3d*> TriangleModel::getFacesPtrs(){
    
    std::vector<Triangle3d*> result;
    if (_faces.empty() && numTriangles() > 0) {
        buildFaces();
    }
    for (size_t index = 0; index < _faces.size(); index++)
    {
        result.push_back(&_faces[index]);
    }

    return result;
}

void TriangleModel::buildFaces() {
    _faces.clear();
    for (size_t i = 0; i < numTriangles(); i++)
    {
        _faces.push_back(getFace(i));
    }
}

std::vector<Triangle3d*> TriangleModel::rayTravesal(Ray3d &r)
{
    std::vector<Triangle3d*> result;
    std::vector<Triangle3d*> allFaces = getFacesPtrs();
    
    for (auto* tri : allFaces) {
        Vect3d p;
        if (tri->ray_tri(r, p)) {
            result.push_back(tri);
        }
    }
    return result;
}

bool TriangleModel::pointIntoMeshOct(Vect3d &p)
{
    if (!_octree) return false;
    
    NodeOctree* leaf = _octree->findLeaf(p);
    if (!leaf) return false;
    
    if (leaf->color == BLACK) return true;
    if (leaf->color == WHITE) return false;
    
    // If GREY, we are at the border. Shoot a ray to decide exactly.
    Vect3d dest(p.getX() + 1.0, p.getY(), p.getZ());
    Ray3d ray(p, dest);
    auto hits = rayTravesal(ray);
    return (hits.size() % 2) != 0; 
}

size_t TriangleModel::numTriangles()
{
    return _indices.size() / 3;
}

 // Protected methods

void TriangleModel::loadModelBinaryFile(const std::string& path)
{
    std::ifstream fin(path, std::ios::in | std::ios::binary);
    if (!fin.is_open())
    {
        std::cout << "Failed to open the binary file " << path << "!" << std::endl;
        return;
    }

    size_t numVertices, numTextCoords, numIndices;

    fin.read((char*)&numVertices, sizeof(size_t));
    fin.read((char*)&numTextCoords, sizeof(size_t));
    this->_vertices.resize(numVertices);
    this->_normals.resize(numVertices);
    this->_textCoordinates.resize(numTextCoords);

    fin.read((char*)this->_vertices.data(), sizeof(Vect3d) * numVertices);
    fin.read((char*)this->_normals.data(), sizeof(Vect3d) * numVertices);
    if (numTextCoords)
        fin.read((char*)this->_textCoordinates.data(), sizeof(Vect2d) * numVertices);

    fin.read((char*)&numIndices, sizeof(size_t));
    if (numIndices)
    {
        this->_indices.resize(numIndices);
        fin.read((char*)this->_indices.data(), sizeof(GLuint) * numIndices);
    }
}

void TriangleModel::processMesh(aiMesh* mesh, const aiScene* scene, const std::string& folder)
{
    // Vertices
    unsigned baseIndex = this->_vertices.size();
    int numVertices = static_cast<int>(mesh->mNumVertices);

    for (int i = 0; i < numVertices; i++)
    {
        this->_vertices.push_back(Vect3d(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z));
        this->_normals.push_back(Vect3d(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z));
        if (mesh->mTextureCoords[0]) this->_textCoordinates.push_back(Vect2d(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y));
    }

    // Indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            this->_indices.push_back(face.mIndices[j] + baseIndex);
    }
}

void TriangleModel::processNode(aiNode* node, const aiScene* scene, const std::string& folder)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        this->processMesh(mesh, scene, folder);
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        this->processNode(node->mChildren[i], scene, folder);
    }
}

void TriangleModel::writeBinaryFile(const std::string& path)
{
    std::ofstream fout(path, std::ios::out | std::ios::binary);
    if (!fout.is_open())
    {
        std::cout << "Failed to write the binary file!" << std::endl;
    }

    size_t numVertices = this->_vertices.size(), numTextCoordinates = this->_textCoordinates.size();

    fout.write((char*)&numVertices, sizeof(size_t));
    fout.write((char*)&numTextCoordinates, sizeof(size_t));
    fout.write((char*)this->_vertices.data(), numVertices * sizeof(Vect3d));
    fout.write((char*)this->_normals.data(), numVertices * sizeof(Vect3d));
    if (numTextCoordinates)
        fout.write((char*)this->_textCoordinates.data(), numTextCoordinates * sizeof(Vect2d));

    size_t numIndices = this->_indices.size();
    fout.write((char*)&numIndices, sizeof(size_t));
    if (numIndices)
        fout.write((char*)this->_indices.data(), numIndices * sizeof(GLuint));

    fout.close();
}
