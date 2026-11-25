#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aOffset;
layout (location = 2) in float aStatus;

out float CellStatus;
out vec2 CellLocalPosition;

uniform mat4 transform;

void main()
{
    CellStatus = aStatus;
    CellLocalPosition = aPos.xy;
    
    // Apply the instance offset before projecting the cell into clip space.
    gl_Position = transform * vec4(aPos.xy + aOffset, aPos.z, 1.0);
}
