#type vertex
#version 450 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 uv;
layout(location = 2) in float thickness;
layout(location = 3) in vec4 color;

uniform mat4 u_viewProjection;

out vec2 v_uv;
out float v_thickness;
out vec4 v_color;

void main() {
    v_uv = (uv - vec2(0.5, 0.5)) * 2.0; // from -1 to 1
    v_thickness = thickness;
    v_color = color;

    gl_Position = u_viewProjection * position;
}

#type fragment
#version 450 core

layout(location = 0) out vec4 outputColor;

in vec2 v_uv;
in float v_thickness;
in vec4 v_color;

void main() {
    float distance = 1.0 - length(v_uv);
    const float fade = 0.05;

    float circleAlpha = smoothstep(0.0, fade, distance);
    circleAlpha *= smoothstep(v_thickness + fade, v_thickness, distance);

    outputColor = v_color;
    outputColor.a *= circleAlpha;
}