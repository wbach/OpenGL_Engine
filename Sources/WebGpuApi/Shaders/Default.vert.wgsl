struct FrameData {
    viewProj: mat4x4<f32>,
};

@group(0) @binding(1) var<uniform> u_frame: FrameData;

struct VertexInput {
    @location(0) position: vec3<f32>,
    @location(1) uv: vec2<f32>,
};

struct VertexOutput {
    @builtin(position) position: vec4<f32>,
    @location(0) uv: vec2<f32>,
};

@vertex
fn vs_main(input: VertexInput) -> VertexOutput {
    var output: VertexOutput;
    output.position = u_frame.viewProj * vec4<f32>(input.position, 1.0);
    output.uv = input.uv;
    return output;
}
