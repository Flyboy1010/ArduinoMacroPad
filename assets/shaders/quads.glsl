#type vertex
#version 450 core

layout(location = 0) in vec3 position;
layout(location = 1) in float textureId;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec4 color;

uniform mat4 u_viewProjection;

out float v_textureId;
out vec2 v_uv;
out vec4 v_color;

void main() {
    v_textureId = textureId;
    v_uv = uv;
    v_color = color;

    gl_Position = u_viewProjection * vec4(position, 1.0);
}

#type fragment
#version 450 core

layout(location = 0) out vec4 outputColor;

uniform sampler2D u_textures[32];

in float v_textureId;
in vec2 v_uv;
in vec4 v_color;

void main() {
    int index = int(v_textureId);

    switch (index)
    {
        case 0 : outputColor = texture(u_textures[0],  v_uv) * v_color; break;
        case 1 : outputColor = texture(u_textures[1],  v_uv) * v_color; break;
        case 2 : outputColor = texture(u_textures[2],  v_uv) * v_color; break;
        case 3 : outputColor = texture(u_textures[3],  v_uv) * v_color; break;
        case 4 : outputColor = texture(u_textures[4],  v_uv) * v_color; break;
        case 5 : outputColor = texture(u_textures[5],  v_uv) * v_color; break;
        case 6 : outputColor = texture(u_textures[6],  v_uv) * v_color; break;
        case 7 : outputColor = texture(u_textures[7],  v_uv) * v_color; break;
        case 8 : outputColor = texture(u_textures[8],  v_uv) * v_color; break;
        case 9 : outputColor = texture(u_textures[9],  v_uv) * v_color; break;
        case 10: outputColor = texture(u_textures[10], v_uv) * v_color; break;
        case 11: outputColor = texture(u_textures[11], v_uv) * v_color; break;
        case 12: outputColor = texture(u_textures[12], v_uv) * v_color; break;
        case 13: outputColor = texture(u_textures[13], v_uv) * v_color; break;
        case 14: outputColor = texture(u_textures[14], v_uv) * v_color; break;
        case 15: outputColor = texture(u_textures[15], v_uv) * v_color; break;
        case 16: outputColor = texture(u_textures[16], v_uv) * v_color; break;
        case 17: outputColor = texture(u_textures[17], v_uv) * v_color; break;
        case 18: outputColor = texture(u_textures[18], v_uv) * v_color; break;
        case 19: outputColor = texture(u_textures[19], v_uv) * v_color; break;
        case 20: outputColor = texture(u_textures[20], v_uv) * v_color; break;
        case 21: outputColor = texture(u_textures[21], v_uv) * v_color; break;
        case 22: outputColor = texture(u_textures[22], v_uv) * v_color; break;
        case 23: outputColor = texture(u_textures[23], v_uv) * v_color; break;
        case 24: outputColor = texture(u_textures[24], v_uv) * v_color; break;
        case 25: outputColor = texture(u_textures[25], v_uv) * v_color; break;
        case 26: outputColor = texture(u_textures[26], v_uv) * v_color; break;
        case 27: outputColor = texture(u_textures[27], v_uv) * v_color; break;
        case 28: outputColor = texture(u_textures[28], v_uv) * v_color; break;
        case 29: outputColor = texture(u_textures[29], v_uv) * v_color; break;
        case 30: outputColor = texture(u_textures[30], v_uv) * v_color; break;
        case 31: outputColor = texture(u_textures[31], v_uv) * v_color; break;
    }
}