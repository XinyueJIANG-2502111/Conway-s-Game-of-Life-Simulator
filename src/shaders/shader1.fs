#version 330 core
out vec4 FragColor;

in float CellStatus;
in vec2 CellLocalPosition;

void main()
{
    float radius = length(CellLocalPosition) * 2.0;
    float edgeWidth = clamp(fwidth(radius), 0.02, 0.35);

    if (CellStatus > 0.5)
    {
        float coverage = 1.0 - smoothstep(1.0 - edgeWidth, 1.0, radius);
        float edgeFade = 1.0 - smoothstep(0.68, 1.0, radius);
        float coreGlow = 1.0 - smoothstep(0.05, 0.92, radius);

        vec3 edgeColor = vec3(0.02, 0.31, 0.1775);
        vec3 bodyColor = vec3(0.06, 0.78, 0.42);
        vec3 coreColor = vec3(0.49, 1.00, 0.7425);
        vec3 litCell = mix(edgeColor, bodyColor, edgeFade);
        vec3 glowColor = mix(litCell, coreColor, coreGlow);
        FragColor = vec4(glowColor * coverage, coverage);
    }
    else
    {
        // 死亡しているセル: 暗い灰色（背景に薄くグリッドが見えるような色）
        FragColor = vec4(0.18f, 0.18f, 0.18f, 1.0f);
    }
}
