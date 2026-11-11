# Tipografías

Se embeben en el plugin con `juce_add_binary_data` (ver `CMakeLists.txt`).

| Archivo | Tipografía | Versión | Licencia |
|---|---|---|---|
| `Inter-Regular.ttf`, `Inter-SemiBold.ttf`, `Inter-Bold.ttf` | [Inter](https://github.com/rsms/inter) | 4.0 | SIL OFL 1.1 (`Inter-OFL.txt`) |
| `JetBrainsMono-Regular.ttf` | [JetBrains Mono](https://github.com/JetBrains/JetBrainsMono) | 2.304 | SIL OFL 1.1 (`JetBrainsMono-OFL.txt`) |

Inter no incluye los símbolos ♯ y ♭: el plugin usa JetBrains Mono como fuente de respaldo para ellos.
