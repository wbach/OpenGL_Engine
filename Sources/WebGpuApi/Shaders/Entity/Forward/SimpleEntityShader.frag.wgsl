struct MaterialData {
    color: vec4<f32>,
};

@group(0) @binding(1) var<uniform> u_material: MaterialData;

@fragment
fn fs_main(@location(0) uv: vec2<f32>) -> @location(0) vec4<f32> {
    let baseColor = u_material.color.rgb * (0.7 + uv.x * 0.3);
    return vec4<f32>(baseColor, u_material.color.a);
}
