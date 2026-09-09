# Lumi-Render

A custom purpose, lightweight C++ OpenGL renderer

---

## Shader API

### Vertex Attributes

| Location | Attribute | Type   |
|:--------:|-----------|--------|
| 0        | position  | `vec3` |
| 1        | normal    | `vec3` |
| 2        | tangent   | `vec4` |
| 3        | color     | `vec3` |
| 4        | texCoord  | `vec2` |

### Instance Attributes

| Location | Attribute | Type   |
|:--------:|-----------|--------|
| 5–8      | model     | `mat4` |
| 9        | color     | `vec3` |

### Uniforms

| Attribute         | Type   |
|-------------------|--------|
| `viewProjection`  | `mat4` |
