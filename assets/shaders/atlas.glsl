@ctype mat4 m4

@vs vs
in vec3 aPos;
in vec2 aTexCoord;

flat out ivec4 inst;
out vec2 TexCoord;
out vec4 FragPos;
flat out mat4 proj;

#define MAX_BATCH_ELEMENTS 128

uniform vs_params {
    mat4 view;
    mat4 projection;
    int billboard;
    vec3 camPos;
    
    mat4 models[MAX_BATCH_ELEMENTS];
    ivec4 batch[MAX_BATCH_ELEMENTS];
};

void main() {
    mat4 model = models[gl_InstanceIndex];
    inst = batch[gl_InstanceIndex];
    vec3 position = aPos;

    // Check if billboarding is enabled
    if (billboard == 1) {
        vec3 modelPos = vec3(-model[3].xyz);
        // Compute direction from camera to the model's position
        vec3 toCamera = normalize(camPos - modelPos);
        
        // Lock the Y-axis by zeroing out the Y component
        toCamera.y = 0.0;
        toCamera = normalize(toCamera);
        
        // Calculate right and up vectors for the billboard rotation matrix
        vec3 right = normalize(cross(vec3(0.0, 1.0, 0.0), toCamera));
        vec3 up = vec3(0.0, 1.0, 0.0);
        
        // Billboard rotation matrix aligned to camera's position
        mat4 billboardRotation = mat4(
            vec4(right, 0.0),
            vec4(up, 0.0),
            vec4(toCamera, 0.0),
            vec4(0.0, 0.0, 0.0, 1.0)
        );

        // Apply billboard rotation to the position
        position = (billboardRotation * vec4(position, 1.0)).xyz;
    }

    // Transform the position with model, view, and projection matrices
    gl_Position = projection * view * model * vec4(position, 1.0);
    TexCoord = aTexCoord;
    FragPos = gl_Position;
    proj = projection;
}
@end

@fs fs
flat in ivec4 inst;
in vec2 TexCoord;
in vec4 FragPos;
flat in mat4 proj;

out vec4 FragColor;

uniform texture2D _texture1;
uniform sampler texture1_smp;
#define texture1 sampler2D(_texture1, texture1_smp)

uniform fs_params {
    int pi, tw, th, aw, ah, useMask, mask, fog;
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

vec4 getColor(uint i, vec2 coord) {
    float xt = i*tw%aw, yt = (i*tw/aw)*th;
    vec2 rectMin = vec2(float(xt) / float(aw), float(yt) / float(ah)); // Top-left of the rectangle
    vec2 rectMax = vec2(float(xt+tw) / float(aw), float(yt+th) / float(ah)); // Bottom-right of the rectangle
    // map TexCoord 0..1 where 0,0 is tl and 1,1 is br
    // to the sub-rectangle within the atlas
    vec2 atlasCoord = mix(rectMin, rectMax, coord*0.99f);
    vec4 col = texture(texture1, atlasCoord);
    return col;
}

void main() {
    // texture atlas
    // compute the normalized texture coordinates for the sub-rectangle
    uint idx = inst.x, color = inst.y, po = inst.z;
    vec4 col = getColor(idx, TexCoord);
    uint pixel = vec4ToU32(col);
    vec4 pixel2 = u32ToVec4(color);

    if (1u == useMask && pixel == mask) { // bitmask
        col = vec4(0,0,0,0); // transparent
    }
    else {
        if (po > 0) {
            // translate color via indexed palette
            for (float x=0; x<8; x++) {
                float fx = x/float(tw-1);
                if (col == getColor(pi, vec2(fx, 0))) {
                    float fy = float(po)/float(th-1);
                    col = getColor(pi, vec2(fx,  fy));
                    break;
                }
            }
        }

        col = alphaBlend(pixel2, col); // apply alpha blending (for color tint effect)
    }

    if (1u == fog) {
        // apply fog
        float depth = (proj * FragPos).z / FragPos.w;
        depth = depth * 0.5f + 0.5f; // normalize
        float fogFar = 0.9f;
        float fogNear = 0.8f;
        vec4 fogColor = vec4(0.0f,0.0f,0.0f,1.0f);

        // cat eyes glow in the dark
        pixel = vec4ToU32(col);
        if (0xff1de6b5 == pixel) depth = 0.f;
        if (0xff00f2f4 == pixel) depth = 0.f;

        float fogFactor = clamp((fogFar - depth) / (fogFar - fogNear), 0.0f, 1.0f);

        // Interpolate between the original color and fog color
        FragColor = mix(fogColor, col, fogFactor);


        FragColor.a = col.a;
    }
    else {
        FragColor = col;
    }
}
@end

@program atlas vs fs