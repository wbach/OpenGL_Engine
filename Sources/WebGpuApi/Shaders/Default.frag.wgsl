struct MaterialData {
    color: vec4<f32>,
};

@group(0) @binding(1) var<uniform> u_material: MaterialData;

@fragment
fn fs_main(@location(0) uv: vec2<f32>) -> @location(0) vec4<f32> {
    let color = u_material.color.rgb * (0.75 + uv.x * 0.25);
    return vec4<f32>(color, u_material.color.a);
}
