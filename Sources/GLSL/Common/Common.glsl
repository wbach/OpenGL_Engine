
vec3 CalcBumpedNormal(mat3 tbn, vec4 normalMapColor)
{
    vec3 bumpMapNormal = normalize(normalMapColor.rgb * 2.f - 1.f);
    return normalize(tbn * bumpMapNormal);
}

bool Is(float f)
{
    return f > .5f;
}
