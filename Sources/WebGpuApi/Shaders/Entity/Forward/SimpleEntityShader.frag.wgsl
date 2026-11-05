struct PerMaterial {
    baseColor: vec4<f32>,
    params: vec4<f32>,
    params2: vec4<f32>,
    hasTextures: vec4<f32>,
    hasTextures2: vec4<f32>,
};

@group(0) @binding(6) var<uniform> u_material: PerMaterial;

struct FragmentInput {
    @location(0) texCoord: vec2<f32>,
    @location(1) textureOffset: vec2<f32>,
    @location(2) normal: vec3<f32>,
    @location(3) outOfViewRange: f32,
    @location(4) visibility: f32,
};

fn convertValueFromRange(value: f32, sourceRange: vec2<f32>, targetRange: vec2<f32>) -> f32 {
    let oldRange = sourceRange.y - sourceRange.x;
    let newRange = targetRange.y - targetRange.x;
    return (((value - sourceRange.x) * newRange) / max(oldRange, 0.0001)) + targetRange.x;
}

@fragment
fn fs_main(input: FragmentInput) -> @location(0) vec4<f32> {
    if (input.outOfViewRange > 0.5) {
        discard;
    }

    var color = u_material.baseColor;
    let dummyDiffuseFactor = max(dot(normalize(vec3<f32>(0.5773502691896258, 0.5773502691896258, 0.5773502691896258)), normalize(input.normal)), 0.0);
    let ambientFactor = 0.1;
    let diffRange = vec2<f32>(0.0, 1.0 - ambientFactor);
    let baseColor = color.rgb * (dummyDiffuseFactor + ambientFactor);
    let alpha = input.visibility * color.a;
    return vec4<f32>(baseColor, alpha);
}
