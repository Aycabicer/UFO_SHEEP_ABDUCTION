#version 330 core



uniform vec3 vectorToLight;	//dirction to the light (directional light) in world space
uniform vec3 sceneAmbient;
uniform float alpha;



in vec3 fragmentNormal;
in vec3 fragmentColor;



layout(location = 0) out vec4 finalColor;



void main() {
	/*
	Lambert's cosine law states that the reflected energy from a small surface
	area in a particular direction is proportional to the cosine of the angle
	between that direction and the surface normal.

	nDotL is 0 if a vector to the light points away from the normal
	*/
    float nDotL = max(0.0, dot(normalize(fragmentNormal), vectorToLight));
    finalColor = vec4(
		sceneAmbient * fragmentColor  +  nDotL * fragmentColor,
		alpha);
}