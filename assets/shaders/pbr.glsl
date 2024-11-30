// pbr.glsl
@ctype mat4 m4

@vs vs
// Physically Based Rendering
// Copyright (c) 2017-2018 Michał Siejak

// Physically Based shading model: Vertex program.

// MIT License
// 
// Copyright (c) 2017-2018 Michał Siejak
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

in vec3 position;
in vec3 normal;
in vec3 tangent;
in vec3 bitangent;
in vec2 texcoord;

#define MAX_BATCH_ELEMENTS 128

uniform TransformUniforms
{
	mat4 viewMatrix;
	mat4 projectionMatrix;

    mat4 models[MAX_BATCH_ELEMENTS];
    ivec4 batch[MAX_BATCH_ELEMENTS];
};

out Vertex
{
	vec3 position;
	vec2 texcoord;
	mat3 tangentBasis;
    flat uint color;
} vout;

void main()
{
	mat4 modelMatrix = models[gl_InstanceIndex];
    vout.color = batch[gl_InstanceIndex].x;

    // vout.position = vec3(modelMatrix * vec4(position, 1.0f));
    vout.position = position;
    vout.texcoord = vec2(texcoord.x, 1.0f-texcoord.y);

    // Pass tangent space basis vectors (for normal mapping).
    vout.tangentBasis = mat3(modelMatrix) * mat3(tangent, bitangent, normal);

	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1.0f);
}
@end

@fs fs
// Physically Based Rendering
// Copyright (c) 2017-2018 Michał Siejak

// Physically Based shading model: Lambertian diffuse BRDF + Cook-Torrance microfacet specular BRDF + IBL for ambient.

// This implementation is based on "Real Shading in Unreal Engine 4" SIGGRAPH 2013 course notes by Epic Games.
// See: http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf

// MIT License
// 
// Copyright (c) 2017-2018 Michał Siejak
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

const float PI = 3.141592;
const float Epsilon = 0.00001;

const int NumLights = 3;

// Constant normal incidence Fresnel factor for all dielectrics.
const vec3 Fdielectric = vec3(0.04);

in Vertex
{
	vec3 position;
	vec2 texcoord;
	mat3 tangentBasis;
    flat uint color;
} vin;

out vec4 FragColor;

uniform ShadingUniforms
{
	vec4 lights_direction[NumLights];
	vec4 lights_radiance[NumLights];
	vec3 eyePosition;
};

uniform texture2D _albedoTexture;
uniform sampler albedoTexture_smp;
#define albedoTexture sampler2D(_albedoTexture, albedoTexture_smp)

uniform texture2D normalTexture_tex;
uniform sampler normalTexture_smp;
#define normalTexture sampler2D(normalTexture_tex, normalTexture_smp)

uniform texture2D metalnessTexture_tex;
uniform sampler metalnessTexture_smp;
#define metalnessTexture sampler2D(metalnessTexture_tex, metalnessTexture_smp)

uniform texture2D roughnessTexture_tex;
uniform sampler roughnessTexture_smp;
#define roughnessTexture sampler2D(roughnessTexture_tex, roughnessTexture_smp)

uniform texture2D specularBRDF_LUT_tex;
uniform sampler specularBRDF_LUT_smp;
#define specularBRDF_LUT sampler2D(specularBRDF_LUT_tex, specularBRDF_LUT_smp)

uniform textureCube specularTexture_tex;
uniform sampler specularTexture_smp;
#define specularTexture samplerCube(specularTexture_tex, specularTexture_smp)

uniform textureCube irradianceTexture_tex;
uniform sampler irradianceTexture_smp;
#define irradianceTexture samplerCube(irradianceTexture_tex, irradianceTexture_smp)

// GGX/Towbridge-Reitz normal distribution function.
// Uses Disney's reparametrization of alpha = roughness^2.
float ndfGGX(float cosLh, float roughness)
{
	float alpha   = roughness * roughness;
	float alphaSq = alpha * alpha;

	float denom = (cosLh * cosLh) * (alphaSq - 1.0) + 1.0;
	return alphaSq / (PI * denom * denom);
}

// Single term for separable Schlick-GGX below.
float gaSchlickG1(float cosTheta, float k)
{
	return cosTheta / (cosTheta * (1.0 - k) + k);
}

// Schlick-GGX approximation of geometric attenuation function using Smith's method.
float gaSchlickGGX(float cosLi, float cosLo, float roughness)
{
	float r = roughness + 1.0;
	float k = (r * r) / 8.0; // Epic suggests using this roughness remapping for analytic lights.
	return gaSchlickG1(cosLi, k) * gaSchlickG1(cosLo, k);
}

// Shlick's approximation of the Fresnel factor.
vec3 fresnelSchlick(vec3 F0, float cosTheta)
{
	return F0 + (vec3(1.0) - F0) * pow(1.0 - cosTheta, 5.0);
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

void main()
{
    // Sample input textures to get shading model params.
    
    vec3 albedo;
    if (0 != vin.color) {
        albedo = u32ToVec4(vin.color).rgb;
    }
    else {
        albedo = texture(albedoTexture, vin.texcoord).rgb;
    }
    float metalness = texture(metalnessTexture, vin.texcoord).r;
    float roughness = texture(roughnessTexture, vin.texcoord).r;

    // Outgoing light direction (vector from world-space fragment position to the "eye").
    vec3 Lo = normalize(eyePosition - vin.position);

    // Get current fragment's normal and transform to world space.
    vec3 N = normalize(2.0 * texture(normalTexture, vin.texcoord).rgb - 1.0);
    N = normalize(vin.tangentBasis * N);
    
    // Angle between surface normal and outgoing light direction.
    float cosLo = max(0.0, dot(N, Lo));
        
    // Specular reflection vector.
    vec3 Lr = 2.0 * cosLo * N - Lo;

    // Fresnel reflectance at normal incidence (for metals use albedo color).
    vec3 F0 = mix(Fdielectric, albedo, metalness);

    // Direct lighting calculation for analytical lights.
    vec3 directLighting = vec3(0);
    for(int i=0; i<NumLights; ++i)
    {
        vec3 Li = -lights_direction[i].xyz;
        vec3 Lradiance = lights_radiance[i].xyz;

        // Half-vector between Li and Lo.
        vec3 Lh = normalize(Li + Lo);

        // Calculate angles between surface normal and various light vectors.
        float cosLi = max(0.0, dot(N, Li));
        float cosLh = max(0.0, dot(N, Lh));

        // Calculate Fresnel term for direct lighting. 
        vec3 F  = fresnelSchlick(F0, max(0.0, dot(Lh, Lo)));
        // Calculate normal distribution for specular BRDF.
        float D = ndfGGX(cosLh, roughness);
        // Calculate geometric attenuation for specular BRDF.
        float G = gaSchlickGGX(cosLi, cosLo, roughness);

        // Diffuse scattering happens due to light being refracted multiple times by a dielectric medium.
        // Metals on the other hand either reflect or absorb energy, so diffuse contribution is always zero.
        // To be energy conserving we must scale diffuse BRDF contribution based on Fresnel factor & metalness.
        vec3 kd = mix(vec3(1.0) - F, vec3(0.0), metalness);

        // Lambert diffuse BRDF.
        // We don't scale by 1/PI for lighting & material units to be more convenient.
        // See: https://seblagarde.wordpress.com/2012/01/08/pi-or-not-to-pi-in-game-lighting-equation/
        vec3 diffuseBRDF = kd * albedo;

        // Cook-Torrance specular microfacet BRDF.
        vec3 specularBRDF = (F * D * G) / max(Epsilon, 4.0 * cosLi * cosLo);

        // Total contribution for this light.
        directLighting += (diffuseBRDF + specularBRDF) * Lradiance * cosLi;
    }

    // Ambient lighting (IBL).
    vec3 ambientLighting;
    {
        // Sample diffuse irradiance at normal direction.
        vec3 irradiance = texture(irradianceTexture, N).rgb;

        // Calculate Fresnel term for ambient lighting.
        // Since we use pre-filtered cubemap(s) and irradiance is coming from many directions
        // use cosLo instead of angle with light's half-vector (cosLh above).
        // See: https://seblagarde.wordpress.com/2011/08/17/hello-world/
        vec3 F = fresnelSchlick(F0, cosLo);

        // Get diffuse contribution factor (as with direct lighting).
        vec3 kd = mix(vec3(1.0) - F, vec3(0.0), metalness);

        // Irradiance map contains exitant radiance assuming Lambertian BRDF, no need to scale by 1/PI here either.
        vec3 diffuseIBL = kd * albedo * irradiance;

        // Sample pre-filtered specular reflection environment at correct mipmap level.
        int specularTextureLevels = textureQueryLevels(specularTexture);
        vec3 specularIrradiance = textureLod(specularTexture, Lr, roughness * specularTextureLevels).rgb;

        // Split-sum approximation factors for Cook-Torrance specular BRDF.o
        vec2 specularBRDF = texture(specularBRDF_LUT, vec2(cosLo, roughness)).rg;

        // Total specular IBL contribution.
        vec3 specularIBL = (F0 * specularBRDF.x + specularBRDF.y) * specularIrradiance;

        // Total ambient lighting contribution.
        ambientLighting = diffuseIBL + specularIBL;
    }

    // Final fragment color.
    FragColor = vec4(directLighting + ambientLighting, 1.0);
}
@end

@program pbr vs fs