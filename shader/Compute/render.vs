#version 430

layout(std140, binding=4) buffer Position
{
    vec4 Positions[];
};

uniform mat4 uProjection;
uniform mat4 uView;

out float vId;

void main()
{
    vId = gl_VertexID;
    gl_Position = uProjection * uView * vec4(Positions[gl_VertexID].xyz, 1.0);
}