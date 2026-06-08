#version 330 core



uniform mat4 modelViewProjMatrix;
uniform mat3 normalMatrix;



layout(location = 0) in vec3 position; 
layout(location = 1) in vec3 color; 
layout(location = 2) in vec3 normal;



out vec3 fragmentNormal;
out vec3 fragmentColor;



void main() {
	fragmentNormal = normalize(normalMatrix * normal); //move to world space
	fragmentColor = color;
	gl_Position = modelViewProjMatrix * vec4(position, 1.0);
}