# Lumi-Render

A custom purpose, lightweight C++ OpenGL renderer

---

## Shader API

- Vertex attributes:
  - `0 - position   vec3`
  - `1 - normal     vec3`
  - `2 - tangent    vec4`
  - `3 - texCoord   vec2`

- Instance attributes:
  - `4-7 - model    mat4`
  - `8   - color    vec3`

- Uniforms:
  - `viewProjection mat4`
