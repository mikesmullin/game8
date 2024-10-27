@ctype mat4 HMM_Mat4

@vs vs
in vec3 aPos;
in vec2 aTexCoord;

out vec2 TexCoord;

uniform vs_params {
    mat4 model;
    mat4 view;
    mat4 projection;
};

void main() {
    // note that we read the multiplication from right to left
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}
@end

@fs fs
out vec4 FragColor;

in vec2 TexCoord;

uniform texture2D _texture1;
uniform sampler texture1_smp;
#define texture1 sampler2D(_texture1, texture1_smp)

uniform fs_params {
    int ts, tx, ty, tw, th, useMask, mask, color;
};

uint vec4ToU32(vec4 color) {
    // Convert each component from [0, 1] to [0, 255] and cast to uint
    uint r = uint(color.r * 255.0);
    uint g = uint(color.g * 255.0);
    uint b = uint(color.b * 255.0);
    uint a = uint(color.a * 255.0);

    // Pack the components into a single uint (RGBA format)
    return (a << 24) | (b << 16) | (g << 8) | r;
}

vec4 u32ToVec4(uint color) {
    // Extract each color component by shifting and masking
    float r = float(color & 0xFF) / 255.0;        // Red is in the lowest byte
    float g = float((color >> 8) & 0xFF) / 255.0; // Green is in the second byte
    float b = float((color >> 16) & 0xFF) / 255.0; // Blue is in the third byte
    float a = float((color >> 24) & 0xFF) / 255.0; // Alpha is in the highest byte

    // Return as a vec4 (r, g, b, a)
    return vec4(r, g, b, a);
}

vec4 alphaBlend(vec4 color1, vec4 color2) {
    float alpha = color1.a;
    return vec4(
        color1.rgb * alpha + color2.rgb * (1.0 - alpha), // Blend RGB
        color1.a + color2.a * (1.0 - alpha)               // Blend alpha
    );
}

void main() {
    // ts,tx,ty texture atlas {tw,th}
    // compute the normalized texture coordinates for the sub-rectangle
    uint xt = ts * tx, yt = ts * ty;
    vec2 rectMin = vec2(float(xt) / float(tw), float(yt) / float(th)); // Top-left of the rectangle
    vec2 rectMax = vec2(float(xt+ts) / float(tw), float(yt+ts) / float(th)); // Bottom-right of the rectangle
    // map TexCoord 0..1 where 0,0 is bl and 1,1 is tr
    // to the sub-rectangle within the atlas
    vec2 atlasCoord = mix(rectMin, rectMax, TexCoord);
    vec4 col = texture(texture1, atlasCoord);
    uint pixel = vec4ToU32(col);
    vec4 pixel2 = u32ToVec4(color);

    if (1 == useMask && pixel == mask) { // bitmask
        col = vec4(0,0,0,0); // transparent
    }
    else {
        // col = mix(col, pixel2, 0.2f); // apply alpha blending (for color tint effect)
        col = alphaBlend(pixel2, col);
    }

    FragColor = col;
}
@end

@program atlas vs fs