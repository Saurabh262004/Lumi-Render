# Lumi-Render

A custom purpose, lightweight C++ OpenGL renderer

---

## Shader API

### Vertex Attributes

| Location | Name      | Type |
|---------:|-----------|------|
| 0        | position  | `vec3` |
| 1        | normal    | `vec3` |
| 2        | tangent   | `vec4` |
| 3        | color     | `vec3` |
| 4        | texCoord  | `vec2` |

### Instance Attributes

| Location | Name   | Type |
|---------:|--------|------|
| 5–8      | model  | `mat4` |
| 9        | color  | `vec3` |

### Uniforms

| Name              | Type |
|-------------------|------|
| `viewProjection`  | `mat4` |
