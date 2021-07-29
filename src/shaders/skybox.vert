#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 texCoords;

uniform mat4 camMatrix;

void main()
{
    vec4 pos = camMatrix * vec4(aPos, 1.0f);

    // z = w results in depth always being 1.0f, thus never getting any closer
    gl_Position = vec4(pos.x, pos.y, pos.w, pos.w);

    // flip the z axis due to the different coordinate systems (left hand vs right hand)
    texCoords = vec3(aPos.x, aPos.y, -aPos.z);
}    