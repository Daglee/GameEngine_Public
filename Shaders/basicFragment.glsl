#version 150 core

in Vertex	{
	vec4 colour;
	vec3 worldPos;
} IN;

out vec4 gl_FragColor;

void main(void)	{
	gl_FragColor = IN.colour;
}