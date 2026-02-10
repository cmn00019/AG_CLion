#include "stdafx.h"
#include "DrawVoxelization.h"

// Public methods

AlgGeom::DrawVoxelization::DrawVoxelization(vec3* positions, size_t numVoxels, const vec3& voxelScale, vec3* voxelColor): Model3D()
{
    Component* component = this->getVoxel();
    _numVoxels = numVoxels;
    _voxelScale = voxelScale;

    // Define instances
    {
        this->buildVao(component);
        component->_vao->defineMultiInstancingVBO(VAO::VBO_MULTI_POSITION, vec3(.0f), .0f, GL_FLOAT);
        component->_vao->setVBOData(VAO::VBO_MULTI_POSITION, positions, numVoxels);
        if (voxelColor)
        {
            component->_vao->defineMultiInstancingVBO(VAO::VBO_MULTI_COLOR, vec3(.0f), .0f, GL_FLOAT);
            component->_vao->setVBOData(VAO::VBO_MULTI_COLOR, voxelColor, numVoxels);
        }
    }

    this->_components.push_back(std::unique_ptr<Component>(component));
}

AlgGeom::DrawVoxelization::~DrawVoxelization()
{
}

void AlgGeom::DrawVoxelization::draw(RenderingShader* shader, MatrixRenderInformation* matrixInformation, ApplicationState* appState, GLuint primitive)
{
    Component* component = _components[0].get();

    shader->setSubroutineUniform(GL_VERTEX_SHADER, "instanceUniform", "multiInstanceUniform");
    shader->applyActiveSubroutines();
    shader->setUniform("globalScale", _voxelScale);

    if (component->_enabled && component->_vao)
    {
        VAO::IBO_slots rendering = VAO::IBO_TRIANGLE;

        switch (primitive)
        {
        case GL_TRIANGLES:
            if (component->_material._useUniformColor)
            {
                shader->setUniform("Kd", component->_material._kdColor);
                shader->setSubroutineUniform(GL_FRAGMENT_SHADER, "kadUniform", "getUniformColor");
            }
            else
            {
                Texture* checkerPattern = TextureList::getInstance()->getTexture(CHECKER_PATTERN_PATH);
                checkerPattern->applyTexture(shader, 0, "texKdSampler");
                shader->setSubroutineUniform(GL_FRAGMENT_SHADER, "kadUniform", "getTextureColor");
            }

            shader->setUniform("Ks", component->_material._ksColor);
            shader->setUniform("metallic", component->_material._metallic);
            shader->setUniform("roughnessK", component->_material._roughnessK);
            shader->setUniform("mModelView", matrixInformation->multiplyMatrix(MatrixRenderInformation::VIEW, this->_modelMatrix));

            break;
        case GL_LINES:
            rendering = VAO::IBO_LINE;
            shader->setUniform("lineColor", component->_material._lineColor);
            glLineWidth(component->_lineWidth);

            break;
        case GL_POINTS:
            rendering = VAO::IBO_POINT;
            shader->setUniform("pointColor", component->_material._pointColor);
            shader->setUniform("pointSize", component->_pointSize);

            break;
        }

        if (!component->_activeRendering[rendering]) return;

        shader->setUniform("mModelViewProj", matrixInformation->multiplyMatrix(MatrixRenderInformation::VIEW_PROJECTION, this->_modelMatrix));
        shader->applyActiveSubroutines();

        component->_vao->drawObject(rendering, primitive, static_cast<GLuint>(component->_indices[rendering].size()), _numVoxels);

        matrixInformation->undoMatrix(MatrixRenderInformation::VIEW);
        matrixInformation->undoMatrix(MatrixRenderInformation::VIEW_PROJECTION);
    }
}