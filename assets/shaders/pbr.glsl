@ctype mat4 HMM_Mat4

@vs vs
in vec3 aPos;
in vec2 aTexCoord;

flat out ivec4 inst;

#define MAX_BATCH_ELEMENTS 128

uniform vs_params {
    mat4 view;
    mat4 projection;
    
    mat4 models[MAX_BATCH_ELEMENTS];
    ivec4 batch[MAX_BATCH_ELEMENTS];
};

void main() {
    mat4 model = models[gl_InstanceIndex];
    inst = batch[gl_InstanceIndex];
    vec3 position = aPos;

    // Transform the position with model, view, and projection matrices
    gl_Position = projection * view * model * vec4(position, 1.0);
}
@end

@fs fs
flat in ivec4 inst;

out vec4 FragColor;

uniform fs_params {
    int i;
};

void main() {
    uint idx = inst.x;
    uint test1 = i+1;

    FragColor = vec4(1.0f,idx,test1,1.0f);
}
@end

@program pbr vs fs