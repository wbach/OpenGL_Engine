struct ElementBuffer {
    transformationMatrix: mat4x4<f32>,
    textureColor: vec4<f32>,
    backgroundColor: vec4<f32>,
    params: vec4<f32>,
};

@group(0) @binding(6) var<uniform> elementBuffer: ElementBuffer;
@group(0) @binding(0) var guiTexture: texture_2d<f32>;
@group(0) @binding(1) var guiSampler: sampler;

@fragment
fn fs_main(@location(0) uv: vec2<f32>) -> @location(0) vec4<f32> {
    var sampledColor = vec4<f32>(0.0, 0.0, 0.0, 0.0);

    if (elementBuffer.params.x > 0.5) {
        sampledColor = textureSample(guiTexture, guiSampler, uv) * elementBuffer.textureColor;
    }

    let bgColor = elementBuffer.backgroundColor;
    let bgWeight = bgColor.a * (1.0 - sampledColor.a);
    let finalRGB = (sampledColor.rgb * sampledColor.a) + (bgColor.rgb * bgWeight);
    let finalAlpha = clamp(sampledColor.a + bgColor.a, 0.0, 1.0);

    if (finalAlpha > 0.0) {
        return vec4<f32>(finalRGB / finalAlpha, finalAlpha);
    }

    return vec4<f32>(0.0, 0.0, 0.0, 0.0);
}
