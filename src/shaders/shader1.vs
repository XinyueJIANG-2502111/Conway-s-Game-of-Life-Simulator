#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aOffset;
layout (location = 2) in float aStatus;

out float CellStatus;

uniform mat4 transform;

void main()
{
    CellStatus = aStatus;
    // セルの基本形状位置に配置オフセット（セルの位置座標）を加算してMVP変換を実行
    gl_Position = transform * vec4(aPos.xy + aOffset, 0.0, 1.0);
}
