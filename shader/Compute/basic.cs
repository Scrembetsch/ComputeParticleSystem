#version 430

layout(std140, binding=4) buffer Position
{
    vec4 Positions[];
};

layout(std140, binding=5) buffer Velocity
{
    vec4 Velocities[];
};

layout(std140, binding=6) buffer Color
{
    vec4 Colors[];
};

layout(LOCAL_WORK_GROUP_SIZE) in;

void main()
{
    const vec3 cGravity = vec3(0.0, 0.0, 0.0);
    const float dt = 0.1;
    uint gid = gl_GlobalInvocationID.x;

    vec3 p = Positions[gid].xyz;
    vec3 v = Velocities[gid].xyz;

    vec3 pp = p + v * dt + 0.5 * dt * dt * cGravity;
    vec3 vp = v + cGravity * dt;

    Positions[gid].xyz = pp;
    Velocities[gid].xyz = vp;
}