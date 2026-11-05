struct PerObjectUpdate {
    transformationMatrix: mat4x4<f32>,
};

@group(0) @binding(3) var<uniform> perObjectUpdate: PerObjectUpdate;

struct VertexOutput {
    @builtin(position) position: vec4<f32>,
    @location(0) uv: vec2<f32>,
};

@vertex
fn vs_main(@builtin(vertex_index) vertexIndex: u32) -> VertexOutput {
    let positions = array<vec2<f32>, 6>(
        vec2<f32>(-1.0,  1.0),
        vec2<f32>(-1.0, -1.0),
        vec2<f32>( 1.0,  1.0),
        vec2<f32>( 1.0,  1.0),
        vec2<f32>(-1.0, -1.0),
        vec2<f32>( 1.0, -1.0)
    );

    let position = positions[vertexIndex];

    var output: VertexOutput;
    output.position = perObjectUpdate.transformationMatrix * vec4<f32>(position, 0.0, 1.0);
    output.uv = vec2<f32>((position.x + 1.0) / 2.0, 1.0 - (position.y + 1.0) / 2.0);
    return output;
}
