#version 330 core
out vec4 FragColor;

in float CellStatus;
in vec2 CellLocalPosition;

void main()
{
    float radius = length(CellLocalPosition) * 2.0;

    // Adapt the edge fade to screen-space pixel size as the camera zooms.
    float edgeWidth = clamp(fwidth(radius), 0.02, 0.35);

    if (CellStatus > 0.5)
    {
        float coverage = 1.0 - smoothstep(1.0 - edgeWidth, 1.0, radius);
        float glow = pow(1.0 - smoothstep(0.0, 1.0, radius), 0.8);
        float brightness = mix(0.2, 1.0, glow);

        const vec3 aliveColor = vec3(0.0, 1.0, 0.6);
        const vec3 coreColor = vec3(0.49, 1.0, 0.7425);
        vec3 glowColor = mix(aliveColor, coreColor, glow) * brightness;
        
        // Premultiply by coverage to match the renderer's blend mode.
        FragColor = vec4(glowColor * coverage, coverage);
    }
    else
    {
        FragColor = vec4(vec3(0.05), 1.0);
    }
}
