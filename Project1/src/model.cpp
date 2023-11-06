#include "model.h"
#include "mesh.h"

#define STB_IMAGE_IMPLEMENTATION
#include "GLFW/stb_image.h"

unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma = false);
aiMesh* g_mesh;

Model::Model()
{

}

Model::Model( std::string const& path, bool gamma = false,bool isFlip = false) 
            : gammaCorrection(gamma), size(size)
{

    this->modelPath = path;
    this->isTextureFlipped = isFlip;
   
    loadModel(path);
}

Model::Model(Model* copyModel)
{
    this->meshes = copyModel->meshes;
    this->modelName = copyModel->modelName;
    this->isVisible = copyModel->isVisible;
    this->isWireFrame = copyModel->isWireFrame;
}



void Model::Draw(Shader& shader)
{
  
    if (!isVisible)
    {
        return;
    }
    glm::mat4 matModel = transform.getModelMatrix();
    shader.setMat4("model", matModel);
    for (unsigned int i = 0; i < meshes.size(); i++)
    {
        meshes[i]->isWireFrame = this->isWireFrame;
        meshes[i]->meshDraw(shader);
    }
      
}

void Model::loadModel(std::string const& path)
{
    this->modelPath = path;
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
   
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
    {
        std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return;
    }
    directory = path.substr(0, path.find_last_of('/'));

    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
       
        g_mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(g_mesh, scene));
    }
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }

}

 Mesh* Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
     std::vector<Vertex> vertices;
     std::vector<unsigned int> indices;
     std::vector<Texture> textures;

    

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        glm::vec3 vector; 
        // positions
        vector.x = mesh->mVertices[i].x ;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;

       

        // normals
        if (mesh->HasNormals())
        {
          
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normals = vector;
        }
        // texture coordinates
        if (mesh->mTextureCoords[0]) 
        {
            glm::vec2 vec;
            // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;

            // tangent
            //vector.x = mesh->mTangents[i].x;
            //vector.y = mesh->mTangents[i].y;
            //vector.z = mesh->mTangents[i].z;
            //vertex.Tangent = vector;
            //// bitangent
            //vector.x = mesh->mBitangents[i].x;
            //vector.y = mesh->mBitangents[i].y;
            //vector.z = mesh->mBitangents[i].z;
            //vertex.Bitangent = vector;
        }
        else
        {
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);

        }

        if (mesh->HasVertexColors(0))
        {
            aiColor4D color = mesh->mColors[0][i];
            vertex.vRGB = glm::vec4(color.r, color.g, color.b,color.a);
        }
        else
        {
            vertex.vRGB = glm::vec4(1.0f, 1.0f, 1.0f,1.0f);
        }

        vertices.push_back(vertex);
    }

    



    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    // process materials
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
   
    // 1. diffuse maps
    std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "material.diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    // 2. specular maps
    std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "material.specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

    /*std::vector<Texture> emissiveMaps = loadMaterialTextures(material, aiTextureType_EMISSIVE, "material.emission");
    textures.insert(textures.end(), emissiveMaps.begin(), emissiveMaps.end());*/
    
    //// 3. normal maps
    //std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
    //textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    //// 4. height maps
    //std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
    //textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
    
    // return a mesh object created from the extracted mesh data
    return new Mesh(vertices, indices, textures);
 }


 std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
     std::vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;   
        mat->GetTexture(type, i, &str);

        if (this->isTextureFlipped)
        {
            stbi_set_flip_vertically_on_load(true);

        }
        else
        {
            stbi_set_flip_vertically_on_load(false);

        }

        bool skip = false;
        for (unsigned int j = 0; j < textures_loaded.size(); j++)
        {
            if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
            {
                textures.push_back(textures_loaded[j]);
                skip = true;
                break;
            }
        }
        if (!skip)
        {   
            Texture texture;
            texture.id = TextureFromFile(str.C_Str(), this->directory);
            texture.type = typeName;
            std::cout << "Texture Loaded: " << texture.type << std::endl;
            texture.path = str.C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(texture); 
        }
    }

    return textures;
}


unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma)
{
    std::string filename = std::string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {

        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        std::cout << "           " << std::endl;
        std::cout << "TEXTURES LOADING......... :" << std::endl;
        std::cout << path << directory << std::endl;
       

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }


    return textureID;
}

