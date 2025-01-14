#include "Shader.h"

#include "glm/gtc/type_ptr.hpp"

#include "fstream"
#include "iostream"

#include "GL/glew.h"
Shader::Shader()
{
	shaderId = 0;
	uniformProjection = 0;
	uniformModel = 0;
}

string Shader::ReadFile(const char* fileLocation) const
{
	string content;
	ifstream fileStream(fileLocation, ios::in);

	if (!fileStream.is_open())
	{
		cout << "Failed to read " << fileLocation << ". File doesn't exist." << endl;
		return "";
	}

	string line;
	while (!fileStream.eof())
	{
		getline(fileStream, line);
		content.append(line + "\n");
	}
	return content;
}

void Shader::CreateFromFile(const string& vertexLocation, const string& fragmentLocation)
{
	const string vertexString = ReadFile(vertexLocation.c_str());
	const string fragmentString = ReadFile(fragmentLocation.c_str());
	const char* vertexCode = vertexString.c_str();
	const char* fragmentCode = fragmentString.c_str();
 	CompileShader(vertexCode, fragmentCode);
}

int Shader::GetProjectionLocation() const
{
	return uniformProjection;
}

int Shader::GetModelLocation() const
{
	return uniformModel;
}

int Shader::GetViewLocation() const
{
	return uniformView;
}

int Shader::GetAmbientColorLocation() const
{
	return uniformAmbientColor;
}

int Shader::GetDiffuseColorLocation() const
{
	return uniformDiffuseColor;
}

int Shader::GetSpecularColorLocation() const
{
	return uniformSpecularColor;
}

int Shader::GetLightDirectionLocation() const
{
	return uniformLightDirection;
}

int Shader::GetAmbientMaterialLocation() const
{
	return uniformAmbientMaterial;
}

int Shader::GetDiffuseMaterialLocation() const
{
	return uniformDiffuseMaterial;
}

int Shader::GetSpecularMaterialLocation() const
{
	return uniformSpecularMaterial;
}

int Shader::GetShininessMaterialLocation() const
{
	return uniformShininessMaterial;
}

int Shader::GetCameraPosLocation() const
{
	return uniformCameraPos;
}

void Shader::CreateFromString(const char* vertexCode, const char* fragmentCode)
{
	CompileShader(vertexCode, fragmentCode);
}

void Shader::SetTexture(unsigned textureUnit) const
{
	glUniform1i(uniformTexture, textureUnit);
}

int Shader::GetDirectionalLightTransformLocation() const
{
	return uniformDirectionalLightTransform;
}

int Shader::GetDirectionalShadowMapLocation() const
{
	return uniformDirectionalShadowMap;
}

int Shader::GetTextureLocation() const
{
	return uniformTexture;
}

void Shader::SetProjection(const mat4& projection) const
{
	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, value_ptr(projection));
}

void Shader::SetModel(const mat4& model) const
{
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, value_ptr(model));
}

void Shader::SetDirectionalShadowMap(const unsigned textureUnit) const
{
	glUniform1i(uniformDirectionalShadowMap, textureUnit);
}

void Shader::SetView(const mat4& view) const
{
	glUniformMatrix4fv(uniformView, 1, GL_FALSE, value_ptr(view));
}

void Shader::SetCameraPos(float x, float y, float z) const
{
	glUniform3f(uniformCameraPos, x, y, z);
}

void Shader::SetDirectionalLightTransform(const mat4 transform) const
{
	glUniformMatrix4fv(uniformDirectionalLightTransform, 1, GL_FALSE, value_ptr(transform));
}

void Shader::CompileShader(const char* vertexCode, const char* fragmentCode)
{
	shaderId = glCreateProgram();

	if (!shaderId)
	{
		cout << "Error creating shader program" << endl;
		return;
	}

	AddShader(shaderId, vertexCode, GL_VERTEX_SHADER);
	AddShader(shaderId, fragmentCode, GL_FRAGMENT_SHADER);

	int result = 0;
	char eLog[1024] = { 0 };

	glLinkProgram(shaderId);
	glGetProgramiv(shaderId, GL_LINK_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shaderId, sizeof(eLog), nullptr, eLog);
		cout << "Error linking program: " << eLog << endl;
		return;
	}

	glValidateProgram(shaderId);
	glGetProgramiv(shaderId, GL_VALIDATE_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shaderId, sizeof(eLog), nullptr, eLog);
		cout << "Error validating program: " << eLog << endl;
		return;
	}

	uniformModel = glGetUniformLocation(shaderId, "model");
	uniformProjection = glGetUniformLocation(shaderId, "projection");
	uniformView = glGetUniformLocation(shaderId, "view");
	uniformAmbientColor = glGetUniformLocation(shaderId, "light.ambient");
	uniformDiffuseColor = glGetUniformLocation(shaderId, "light.diffuse");
	uniformSpecularColor = glGetUniformLocation(shaderId, "light.specular");
	uniformLightDirection = glGetUniformLocation(shaderId, "light.direction");
	uniformAmbientMaterial = glGetUniformLocation(shaderId, "material.ambient");
	uniformDiffuseMaterial = glGetUniformLocation(shaderId, "material.diffuse");
	uniformSpecularMaterial = glGetUniformLocation(shaderId, "material.specular");
	uniformShininessMaterial = glGetUniformLocation(shaderId, "material.shininess");
	uniformCameraPos = glGetUniformLocation(shaderId, "cameraPos");
	uniformTexture = glGetUniformLocation(shaderId, "texture_diffuse1");
	uniformDirectionalLightTransform = glGetUniformLocation(shaderId, "directionalLightSpaceTransform");
	uniformDirectionalShadowMap = glGetUniformLocation(shaderId, "shadowMap");
}

void Shader::UseShader() const	
{
	glUseProgram(shaderId);
}

void Shader::ClearShader()
{
	if (shaderId != 0)
	{
		glDeleteProgram(shaderId);
		shaderId = 0;
	}
	uniformModel = 0;
	uniformProjection = 0;
}	

void Shader::AddShader(unsigned int theProgram, const char* shaderCode, unsigned int shaderType)
{
	const unsigned int theShader = glCreateShader(shaderType);

	const int codeLength = static_cast<int>(strlen(shaderCode));

	glShaderSource(theShader, 1, &shaderCode, &codeLength);
	glCompileShader(theShader);

	int result = 0;
	char eLog[1024] = { 0 };

	glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(theShader, sizeof(eLog), nullptr, eLog);
		cout << "Error compiling the " << shaderType << " shader: " << eLog << endl;
		return;
	}

	glAttachShader(theProgram, theShader);
}