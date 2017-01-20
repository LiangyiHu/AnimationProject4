#pragma once
// Std. Includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;
// GL Includes
#include <GL/glew.h> // Contains all the necessery OpenGL includes
// GLM Mathemtics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SOIL.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Mesh.h"
#include "MatrixTransformation.h"
#include "Shader.h"  //create and compile shader program

GLint TextureFromFile(const char* path, string directory);

class Model
{
public:
	/*  Functions   */
	// Constructor, expects a filepath to a 3D model.
	
	GLuint objectID;
	bool gravitational;
	float radius,scale;
	

	Model(GLchar* path, GLuint objectID)
	{
		this->objectID = objectID;
		this->loadModel(path);
	}


	//static void collisionDetect(Model* x, Model* y, collisionDetectType detectMethod) {
	//	if (detectMethod == groundDetect) {
	//		if (y->radius > y->currentPosition.y) {
	//			//collide
	//			y->speed.y = glm::abs(glm::sqrt(ENERGY_RESERVED_RATE)*glm::abs(y->speed.y));
	//			y->speed.x= glm::sqrt(ENERGY_RESERVED_RATE)*glm::abs(y->speed.x);
	//			y->speed.z = glm::sqrt(ENERGY_RESERVED_RATE)*glm::abs(y->speed.z);
	//			
	//				//printThisVector(y->speed);
	//				y->angularSpeed = glm::length(glm::vec3(y->speed.x, 0.0f, y->speed.z)) / y->radius;
	//				//std::cout << y->angularSpeed << std::endl;
	//				y->currentRotationAxle = glm::normalize(getPerpendicularXZ(y->speed));
	//			
	//		}
	//	}

	//	else if (glm::length(x->currentPosition - y->currentPosition) <= (x->radius + y->radius)) {
	//		glm::vec3 n = glm::normalize(x->currentPosition - y->currentPosition);
	//		GLfloat a1 = glm::dot(x->speed, n);
	//		GLfloat a2 = glm::dot(y->speed, n);
	//		GLfloat b1 = glm::length(x->speed - a1*n);
	//		GLfloat b2 = glm::length(y->speed - a2*n);
	//		GLfloat oldEnergy = 0.5*(x->mass*glm::pow(a1, 2) + x->mass*glm::pow(b1, 2) + y->mass*glm::pow(a2, 2) + y->mass*glm::pow(b2, 2));
	//		GLfloat minEnergy = 0.5*(x->mass + y->mass)*glm::pow(glm::length((x->mass*x->speed + y->mass*y->speed) / (x->mass + y->mass)), 2);
	//		GLfloat a = (x->mass + y->mass) / (x->mass*y->mass);
	//		GLfloat b = 2 * (a2 - a1);
	//		GLfloat c, optimizedP;
	//		if (oldEnergy*ENERGY_RESERVED_RATE > minEnergy) {
	//			c = (1 - ENERGY_RESERVED_RATE)*2*oldEnergy;
	//			//std::cout <<( glm::pow(b, 2) - 4 * a*c) << std::endl;
	//			optimizedP = glm::min((-b - glm::sqrt(glm::pow(b, 2) - 4 * a*c)) / (2 * a), (-b + glm::sqrt(glm::pow(b, 2) - 4 * a*c)) / (2 * a));
	//		}
	//		else {
	//			// energy reserve rate have to be greater than min energy, just lost arbitrary amount
	//			GLfloat newEnergyReserveRate = (2*minEnergy + oldEnergy) / (3 * oldEnergy);
	//			c = (1 - newEnergyReserveRate) * 2 * oldEnergy;
	//			std::cout << (glm::pow(b, 2) - 4 * a*c) << std::endl;
	//			optimizedP = glm::min((-b-glm::sqrt(glm::abs(glm::pow(b,2)-4*a*c)))/(2*a), (-b + glm::sqrt(glm::abs(glm::pow(b, 2) - 4 * a*c))) / (2 * a));
	//		}
	//		x->speed = x->speed - optimizedP/x->mass*n;
	//		y->speed = y->speed + optimizedP/y->mass*n;
	//	}
	//}

	/*static glm::vec3 getPerpendicularXZ(glm::vec3 baseVector) {

		glm::vec3 reshapedBase = glm::vec3(baseVector.x, 0.0f, baseVector.z);
		if (glm::length(reshapedBase) < 0.5f) {
			return glm::vec3(0.0f, 0.0f, 1.0f);
		}
		glm::vec3 resultAxle = glm::vec3();
		if (reshapedBase.x == 0) {
			return glm::vec3(reshapedBase.z==0?1: reshapedBase.z,0.0f,0.0f);
		}
		else if (reshapedBase.z == 0) {
			return glm::vec3(0.0f, 0.0f, -reshapedBase.x);
		}
		resultAxle.x = 1;
		resultAxle.z = -reshapedBase.x / reshapedBase.z;
		if (glm::cross(reshapedBase, resultAxle).y < 0) {
			resultAxle = glm::vec3(-resultAxle.x, 0.0f, -resultAxle.z);
		}
		return resultAxle;
	}*/

	// Draws the model, and all its meshes
	void Draw(Shader* objShader)
	{
		for (GLuint i = 0; i < this->meshes.size(); i++)
			this->meshes[i].Draw(objShader);
	}


private:
	/*  Model Data  */
	vector<Mesh> meshes;
	string directory;
	vector<Texture> textures_loaded;	// Stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.

										/*  Functions   */
										// Loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
	void loadModel(string path)
	{
		// Read file via ASSIMP
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
		// Check for errors
		if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
		{
			cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
			return;
		}
		// Retrieve the directory path of the filepath
		this->directory = path.substr(0, path.find_last_of('/'));

		// Process ASSIMP's root node recursively
		this->processNode(scene->mRootNode, scene);
	}

	// Processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
	void processNode(aiNode* node, const aiScene* scene)
	{
		// Process each mesh located at the current node
		for (GLuint i = 0; i < node->mNumMeshes; i++)
		{
			// The node object only contains indices to index the actual objects in the scene. 
			// The scene contains all the data, node is just to keep stuff organized (like relations between nodes).
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			this->meshes.push_back(this->processMesh(mesh, scene));
		}
		// After we've processed all of the meshes (if any) we then recursively process each of the children nodes
		for (GLuint i = 0; i < node->mNumChildren; i++)
		{
			this->processNode(node->mChildren[i], scene);
		}

	}

	Mesh processMesh(aiMesh* mesh, const aiScene* scene)
	{
		// Data to fill
		vector<Vertex> vertices;
		vector<GLuint> indices;
		vector<Texture> textures;

		// Walk through each of the mesh's vertices
		for (GLuint i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			glm::vec3 vector; // We declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
							  // Positions
			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vertex.Position = vector;
			// Normals
			/*vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;*/
			vertex.Normal = glm::vec3();
			// Texture Coordinates
			if (mesh->mTextureCoords[0]) // Does the mesh contain texture coordinates?
			{
				glm::vec2 vec;
				// A vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
				// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.TexCoords = vec;
			}
			else
				vertex.TexCoords = glm::vec2(0.0f, 0.0f);
			vertices.push_back(vertex);
		}
		// Now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
		for (GLuint i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			// Retrieve all indices of the face and store them in the indices vector
			for (GLuint j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}
		// Process materials
		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			// We assume a convention for sampler names in the shaders. Each diffuse texture should be named
			// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
			// Same applies to other texture as the following list summarizes:
			// Diffuse: texture_diffuseN
			// Specular: texture_specularN
			// Normal: texture_normalN

			// 1. Diffuse maps
			vector<Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
			// 2. Specular maps
			vector<Texture> specularMaps = this->loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		}

		// Return a mesh object created from the extracted mesh data
		return Mesh(vertices, indices, textures, objectID);
	}

	// Checks all material textures of a given type and loads the textures if they're not loaded yet.
	// The required info is returned as a Texture struct.
	vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
	{
		vector<Texture> textures;
		for (GLuint i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);
			// Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
			GLboolean skip = false;
			for (GLuint j = 0; j < textures_loaded.size(); j++)
			{
				if (textures_loaded[j].path == str)
				{
					textures.push_back(textures_loaded[j]);
					skip = true; // A texture with the same filepath has already been loaded, continue to next one. (optimization)
					break;
				}
			}
			if (!skip)
			{   // If texture hasn't been loaded already, load it
				Texture texture;
				texture.id = TextureFromFile(str.C_Str(), this->directory);
				texture.type = typeName;
				texture.path = str;
				textures.push_back(texture);
				this->textures_loaded.push_back(texture);  // Store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
			}
		}
		return textures;
	}
};



GLint TextureFromFile(const char* path, string directory)
{
	//Generate texture ID and load texture data 
	string filename = string(path);
	filename = directory + '/' + filename;
	GLuint textureID;
	glGenTextures(1, &textureID);
	int width, height;
	unsigned char* image = SOIL_load_image(filename.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
	//std::cout << "width is: " << width << "   height is: " << height << std::endl;
	// Assign texture to ID
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	SOIL_free_image_data(image);
	return textureID;
}