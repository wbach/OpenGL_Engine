const MAX_BONES = 512;
const MAX_WEIGHTS = 4;

struct PerApp {
    useTextures: vec4<f32>,
    viewDistance: vec4<f32>,
    shadowVariables: vec4<f32>,
    fogData: vec4<f32>,
};

struct PerFrame {
    projectionViewMatrix: mat4x4<f32>,
    cameraPosition: vec3<f32>,
    clipPlane: vec4<f32>,
    projection: vec4<f32>,
    time: vec4<f32>,
};

struct PerObjectConstants {
    useBoneTransform: f32,
    textureOffset: vec2<f32>,
    _pad0: vec2<f32>,
};

struct PerObjectUpdate {
    transformationMatrix: mat4x4<f32>,
};

struct PerPoseUpdate {
    bonesTransforms: array<mat4x4<f32>, MAX_BONES>,
};

struct VertexInput {
    @location(0) position: vec3<f32>,
    @location(1) texCoord: vec2<f32>,
    @location(2) normal: vec3<f32>,
    @location(3) tangent: vec3<f32>,
    @location(4) weights: vec4<f32>,
    @location(5) boneIds: vec4<i32>,
};

struct VertexOutput {
    @builtin(position) position: vec4<f32>,
    @location(0) texCoord: vec2<f32>,
    @location(1) textureOffset: vec2<f32>,
    @location(2) normal: vec3<f32>,
    @location(3) outOfViewRange: f32,
    @location(4) visibility: f32,
};

@group(0) @binding(0) var<uniform> u_app: PerApp;
@group(0) @binding(1) var<uniform> u_frame: PerFrame;
@group(0) @binding(2) var<uniform> u_objectConstants: PerObjectConstants;
@group(0) @binding(3) var<uniform> u_objectUpdate: PerObjectUpdate;
@group(0) @binding(4) var<uniform> u_poseUpdate: PerPoseUpdate;

fn isFlag(v: f32) -> bool {
    return v > 0.5;
}

@vertex
fn vs_main(input: VertexInput) -> VertexOutput {
    var output: VertexOutput;
    var worldPosition = vec4<f32>(0.0);
    var worldNormal = vec4<f32>(0.0);

    if (!isFlag(u_objectConstants.useBoneTransform)) {
        worldPosition = u_objectUpdate.transformationMatrix * vec4<f32>(input.position, 1.0);
        worldNormal = u_objectUpdate.transformationMatrix * vec4<f32>(input.normal, 0.0);
    } else {
        for (var i = 0; i < MAX_WEIGHTS; i = i + 1) {
            let boneIndex = input.boneIds[i];
            let boneTransform = u_poseUpdate.bonesTransforms[boneIndex];
            let posePosition = boneTransform * vec4<f32>(input.position, 1.0);
            worldPosition += posePosition * input.weights[i];

            let worldNormalVec = boneTransform * vec4<f32>(input.normal, 0.0);
            worldNormal += worldNormalVec * input.weights[i];
        }
        worldPosition = u_objectUpdate.transformationMatrix * worldPosition;
        worldNormal = u_objectUpdate.transformationMatrix * worldNormal;
    }

    let viewNormal = normalize(worldNormal.xyz);
    let cameraDelta = u_frame.cameraPosition - worldPosition.xyz;
    let distance = length(cameraDelta);
    let visibility = clamp(exp(-pow((distance * (3.0 / max(u_app.viewDistance.x, 0.0001))), u_app.fogData.w)), 0.0, 1.0);

    output.texCoord = input.texCoord;
    output.textureOffset = u_objectConstants.textureOffset;
    output.normal = viewNormal;
    output.outOfViewRange = 0.0;
    output.visibility = visibility;
    output.position = u_frame.projectionViewMatrix * worldPosition;
    return output;
}
