#version 330 core
out vec4 FragColor;

in float CellStatus;

void main()
{
    if (CellStatus > 0.5)
    {
        // 生存しているセル: 緑色
        FragColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);
    }
    else
    {
        // 死亡しているセル: 暗い灰色（背景に薄くグリッドが見えるような色）
        FragColor = vec4(0.18f, 0.18f, 0.18f, 1.0f);
    }
}
