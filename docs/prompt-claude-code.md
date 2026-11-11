# Prompt para Claude Code · P.ChordAnalyzer

> Copia todo lo que hay debajo de la línea y pégalo en Claude Code.

---

Vamos a crear **P.ChordAnalyzer**, un plugin de audio para FL Studio (Windows) que detecta en tiempo real la **tonalidad/escala** de lo que le entra y la muestra en una ventana pequeña y oscura. Solo analiza: no modifica el audio.

## Forma de trabajar (obligatorio)

- Trabaja **por fases** (las de abajo). Al terminar cada fase: explícame qué has hecho, **cómo probarlo paso a paso en FL Studio** y **detente**. No empieces la siguiente hasta que te dé el visto bueno.
- Si tienes que tomar una decisión que no está aquí, elige la opción más sencilla, dímelo en una línea y sigue.
- Código limpio y comentado en español. Commits pequeños y con mensajes claros.

## Decisiones técnicas

- **Framework:** JUCE 8 con **CMake** (JUCE añadido como submódulo o con FetchContent). Licencia del proyecto: **AGPLv3** (compatible con JUCE).
- **Formatos:** **VST3** (para FL Studio) y **Standalone (.exe)** que escuche la entrada de audio del sistema. Windows x64.
- **Compilación:** **solo con GitHub Actions** (no tengo Visual Studio instalado). Workflow en `windows-latest` que compile en Release y suba como artefactos el `.vst3` y el `.exe`. Se lanza en cada push a `main` y manualmente.
- **Repositorio:** `P.ChordAnalyzer` en mi GitHub (público). Fabricante "P.P". Si en mi repo `P.Melodies` ya hay un código de fabricante de JUCE, reutiliza el mismo; el código de plugin, uno nuevo de 4 letras.
- **Canales:** entrada/salida estéreo (acepta también mono). Sin MIDI.

## Requisitos de funcionamiento

### Audio: cero latencia y ligero
- `processBlock` deja pasar el audio **intacto** y reporta **latencia 0**.
- En el hilo de audio solo se copia la mezcla mono (L+R)/2 a un **FIFO sin bloqueos** (`juce::AbstractFifo` o similar). **Nada de reservas de memoria, locks ni logs** en el hilo de audio.
- El análisis corre en un **hilo aparte** (o timer de fondo), unas **10 veces por segundo**.

### Motor de detección de escala
1. **Cromagrama:** FFT con ventana Hann (p. ej. 8192 muestras a 44,1/48 kHz, salto ~2048), rango ~55 Hz–5 kHz, energía de cada bin sumada en sus 12 clases de nota (todas las octavas juntas). Normalizar.
2. **Acumulación:** media exponencial del cromagrama con memoria de **~4–6 s**, para que refleje la sección que suena y no cada acorde suelto.
3. **Tonalidad:** correlación del cromagrama con los **24 perfiles** (12 mayores + 12 menores) usando perfiles tipo Krumhansl-Kessler o Temperley (elige los que mejor funcionen con música electrónica/hip-hop y justifícalo). Referencia de funcionamiento: KeyFinder / libKeyFinder (solo como referencia; implementación propia).
4. **Confianza (0–100 %):** a partir de la diferencia entre la mejor y la segunda mejor correlación.
5. **Suavizado / histéresis:** la escala mostrada **solo cambia** si la nueva candidata lleva **≥1,5 s** siendo la mejor. Nada de parpadeos.
6. **Silencio:** si el nivel está por debajo de ~-60 dBFS (FL parado, huecos entre loops), **se mantiene la última escala** y el cromagrama no se reinicia.
7. Resultado expuesto a la UI de forma thread-safe (atómicos o copia protegida fuera del hilo de audio): escala (tónica + modo), relativa, confianza, cromagrama actual de 12 valores.

### Controles
- **Congelar:** la UI deja de actualizarse y muestra la última lectura; el análisis sigue por detrás. Al reanudar, muestra el estado actual.
- **Bypass:** parámetro del plugin (expuesto al host como parámetro de bypass). Detiene el análisis por completo (CPU ≈ 0). El audio sigue pasando intacto. La UI conserva la última escala atenuada.
- **Cambiar de vista no reinicia nada**: las tres vistas leen el mismo resultado.
- **Guardar estado** con el proyecto de FL: vista actual, congelar, bypass.

## Aspecto visual (exacto)

Ventana fija **340 × 330 px**, no redimensionable. Fondo **#0E0E10**, borde 1 px **#26262B**, esquinas 6 px. Padding 12 px.

**Paleta:** texto principal #F5F5F7 · secundario #8A8A92 · terciario #C8C8CE · tecla/barra en escala #EDEDF0 · tónica #FFFFFF · fuera de escala #2E2E34 · teclas negras #08080A con borde #3A3A40 · botones #1C1C20 con borde #3A3A40.
**Tipografías (embebidas con BinaryData, licencia OFL):** Inter (textos) y JetBrains Mono (notas, porcentajes).
**Notación:** nombre en español grande ("La menor") + cifrado inglés pequeño ("Am"); notas en inglés con ♯/♭ (C, C♯, D…).

**Estructura de arriba abajo:**
1. **Cabecera (36 px):** a la izquierda "P.CHORDANALYZER" (10 px, seminegrita, espaciado ancho, #8A8A92); si hay bypass, al lado una etiqueta "BYPASS" (texto oscuro sobre fondo #F5F5F7). A la derecha dos botones de 36×36: **congelar** (icono de pausa; activo → fondo blanco e icono de play) y **bypass** (icono de encendido; activo → fondo blanco).
2. **Zona de vistas (230 px):** carrusel horizontal de 3 vistas. **Flechas** (chevrons, 32×32, sin fondo, #C8C8CE) centradas verticalmente a izquierda y derecha; se ocultan en el extremo que no lleva a ningún sitio.
3. **Selector de puntos (24 px):** 3 puntos centrados, sin texto; activo blanco 7 px, inactivos #3A3A40 6 px; clicables.

**Vistas** (orden izquierda → derecha; **por defecto se abre la del centro**):
- **Izquierda · Teclado:** fila con "La menor" (22 px, negrita) + "Am" (mono, gris) y confianza a la derecha. Debajo, una octava (7 blancas + 5 negras, ~237×112 px): las notas de la escala encendidas (#EDEDF0, las negras en escala también claras), las de fuera apagadas (#1C1C20 / #08080A); la tónica con un punto oscuro. Leyenda pequeña: "● tónica · teclas apagadas: fuera de escala".
- **Centro · Círculo de quintas (por defecto):** dos anillos finos (#26262B): exterior con las 12 mayores (C, G, D… F), interior con las 12 menores relativas. La escala detectada como "píldora" blanca con texto oscuro; su relativa en blanco; el resto #8A8A92. Debajo: "La menor" (20 px, negrita) y "relativa: Do mayor · 87 %".
- **Derecha · Cromagrama:** fila de nombre + confianza como en el teclado. Debajo 12 barras verticales (C → B) con la energía de cada nota: en escala #EDEDF0, tónica #FFFFFF, fuera de escala #2E2E34; etiqueta mono 9 px bajo cada barra. Las barras se mueven suavemente (interpolación), no a saltos.

**Animaciones:**
- Cambio de vista: **deslizamiento horizontal** del carrusel en **~380 ms** con curva de salida suave (ease-out). Solo se anima mientras dura; sin timer de 60 fps permanente.
- Bypass: la zona de vistas pasa a opacidad 25 % con un fundido de ~200 ms.
- Repintado de datos a ~30 fps como máximo y solo cuando algo cambia.

## Fases

1. **Esqueleto + compilación:** proyecto CMake/JUCE, VST3 + Standalone, audio en paso directo con latencia 0, ventana vacía 340×330 con el fondo, workflow de GitHub Actions funcionando y artefactos descargables. *Prueba: lo cargo en FL, el sonido no cambia y el .exe abre.*
2. **Motor de análisis:** FIFO, hilo de análisis, cromagrama, 24 perfiles, confianza, suavizado, silencio. Tests unitarios con señales sintéticas (escalas y acordes conocidos en varias tonalidades) que se ejecuten en el workflow. En la UI, de momento, solo el texto del resultado. *Prueba: reproduzco loops en escalas conocidas.*
3. **Interfaz base:** cabecera, botones de congelar y bypass funcionando, vista de círculo de quintas completa, tipografías y paleta.
4. **Teclado, cromagrama y carrusel:** las otras dos vistas, flechas, puntos y animación de deslizamiento.
5. **Pulido:** guardado de estado, revisión de CPU (objetivo < 1 % en un núcleo), README en español con capturas, instrucciones de instalación del .vst3 en FL Studio y licencia AGPL.
