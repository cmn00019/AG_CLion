# Memoria Técnica — Proyecto Final: Operaciones Booleanas con CGAL

## 1. Introducción y objetivos

El objetivo de este proyecto es integrar un módulo de **operaciones booleanas 3D** (unión, intersección y diferencia simétrica/asimétrica) dentro del visor geométrico existente de la asignatura *Algoritmos Geométricos*. El núcleo computacional se apoya en **CGAL** (Computational Geometry Algorithms Library), concretamente en el paquete `Polygon_mesh_processing`, que implementa algoritmos robustos sobre mallas de triángulos (`Surface_mesh`).

Además del núcleo booleano, se han realizado mejoras significativas en:
- **Rendimiento**: eliminación de cuellos de botella en la validación de mallas y en la exportación de resultados.
- **Robustez**: manejo de mallas degeneradas, orientación incorrecta, modelos idénticos y formatos de entrada variados.
- **Usabilidad**: rediseño completo de la interfaz gráfica (ImGui) y mejora de los controles de cámara para facilitar la inspección de modelos.

---

## 2. Stack tecnológico y dependencias

| Capa | Tecnología / Librería | Uso |
|------|----------------------|-----|
| Lenguaje | C++20 | Núcleo de la aplicación |
| Compilador | MSVC (Visual Studio 2022+) | Windows, CMake + NMake |
| Geometría | CGAL 5.x (EPICK kernel) | Operaciones booleanas, `Surface_mesh`, reparación de mallas |
| Álgebra | GLM | Vectores, matrices, transformaciones 3D |
| Gráficos | OpenGL 4.5 Core | Renderizado de geometría |
| Ventana / Input | GLFW + GLEW | Contexto OpenGL y eventos de entrada |
| GUI | Dear ImGui + ImGuizmo | Paneles, diálogos, gizmos de transformación |
| Carga de modelos | Open Asset Import Library (Assimp) | Carga de OBJ, OFF, FBX, GLTF… |
| Caché binaria | `std::filesystem` + ficheros `_tm.bin` | Evita re-parsear modelos en cada ejecución |

El *kernel* elegido en CGAL es `Exact_predicates_inexact_constructions_kernel` (EPICK): ofrece predicados robustos (evita errores de clasificación de puntos, orientación, etc.) sin el coste computacional de construcciones exactas, lo cual es adecuado para un visor interactivo.

---

## 3. Arquitectura general

El proyecto sigue una arquitectura por capas:

```
┌─────────────────────────────────────────────┐
│  GUI (ImGui)  ─  InputManager (GLFW)        │  ← Interacción usuario
├─────────────────────────────────────────────┤
│  SceneContent  ─  Camera  ─  Renderer       │  ← Lógica de escena y render
├─────────────────────────────────────────────┤
│  Model3D / DrawMesh / DrawBooleanResult     │  ← Representación visual
├─────────────────────────────────────────────┤
│  CgalBooleanOperations                      │  ← Núcleo CGAL (booleanas)
├─────────────────────────────────────────────┤
│  CGAL :: Surface_mesh / PMP / SMS           │  ← Librería externa
└─────────────────────────────────────────────┘
```

La comunicación entre la capa visual y CGAL se realiza a través de conversiones bidireccionales entre `TriangleModel` (formato interno basado en Assimp) y `CGAL::Surface_mesh<Kernel::Point_3>`.

---

## 4. Clases y métodos implementados / modificados

### 4.1 `CgalBooleanOperations` (núcleo booleano)

**Ubicación:** `clion/Geometry/CgalBooleanOperations.h` / `.cpp`

Esta clase es el envoltorio (*wrapper*) de todas las operaciones de CGAL. Mantiene internamente dos mallas de entrada (`_meshA`, `_meshB`), sus copias originales (`_meshA_orig`, `_meshB_orig`) y la malla resultado (`_result`).

#### Miembros principales

```cpp
typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
typedef Kernel::Point_3 Point_3;
typedef CGAL::Surface_mesh<Point_3> SurfaceMesh;

SurfaceMesh _meshA, _meshB, _meshA_orig, _meshB_orig, _result;
bool _hasA, _hasB, _hasResult;
bool _validA, _validB;          // Cache de validez (evita re-comprobar cada frame)
```

#### Métodos públicos clave

| Método | Descripción |
|--------|-------------|
| `loadModelA(path)` / `loadModelB(path)` | Carga un modelo desde disco. Para `.off` usa el lector nativo de CGAL. Para `.obj` intenta primero `CGAL::IO::read_OBJ`; si falla, hace *fallback* a `TriangleModel` (Assimp) que triangula y une vértices idénticos automáticamente. Finalmente aplica `repairAndOrientMesh()` y evalúa `isValidForBoolean()`. |
| `setModelA(TriangleModel&)` / `setModelB(...)` | Convierte directamente un `TriangleModel` ya cargado (útil para integración con la capa visual sin releer de disco). |
| `applyModelMatrixToMeshes(mat4, mat4)` | **Crítico**: antes de ejecutar una operación booleana, transforma los vértices de `_meshA_orig` y `_meshB_orig` aplicando las matrices de modelo actuales de los objetos visuales. De este modo, el usuario puede trasladar/rotar/escalar los modelos con el gizmo y la operación booleana refleja esa disposición. El transformador respeta el orden **column-major** de GLM. |
| `computeUnion()` | Calcula `A ∪ B`. Usa internamente `PMP::corefine_and_compute_boolean_operations()`. Extrae la malla de salida del índice `[0]` del array de resultados. |
| `computeIntersection()` | Calcula `A ∩ B`. Extrae el índice `[1]`. |
| `computeDifferenceAB()` | Calcula `A − B`. Extrae el índice `[2]`. |
| `computeDifferenceBA()` | Calcula `B − A`. Extrae el índice `[3]`. |
| `simplifyResult(ratio)` | Aplica `SMS::edge_collapse` con `Edge_count_ratio_stop_predicate(ratio)`. Colapsa aristas preservando la topología hasta alcanzar la fracción de aristas indicada. Útil para reducir la complejidad del resultado antes de exportarlo. |
| `exportResultToTriangleModel()` | Exporta `_result` a un `TriangleModel*` mediante un **constructor rápido por vectores** (`std::vector<Vect3d>&&`, `std::vector<unsigned>&&`). Esta ruta evita la deduplicación O(n²) que colgaba la aplicación con modelos grandes (p. ej. *Ajax*). |
| `saveResultToOBJ(path)` / `saveResultToOFF(path)` | Guarda el resultado en formato Wavefront OBJ u OFF con índices 1-based y recorriendo la `Surface_mesh` de forma indexada. |
| `isValidForBoolean(mesh)` | Verifica tres condiciones imprescindibles para CGAL: `number_of_faces() > 0`, `CGAL::is_triangle_mesh()` y `CGAL::is_closed()` (*watertight*). |

#### Funciones estáticas / helpers privados

- `triangleModelToSurfaceMesh(TriangleModel&)`: iteración directa sobre los vectores de vértices e índices del `TriangleModel`, añadiendo vértices y caras a una `Surface_mesh` vacía.
- `surfaceMeshToTriangleModel(...)`: conversión inversa (utilizada en otras partes del proyecto).
- `loadOBJToSurfaceMesh(...)`: envoltorio de `CGAL::IO::read_OBJ` con `PMP::triangulate_faces` como post-proceso si la malla contiene polígonos.
- `loadOFFToSurfaceMesh(...)`: lector nativo de OFF mediante `operator>>` de CGAL.
- `transformPoint(const Point_3&, const mat4&)`: aplica una matriz 4×4 de GLM a un punto CGAL, teniendo en cuenta que GLM almacena matrices en **column-major**.

#### Funciones libres en el `.cpp`

- `repairAndOrientMesh(mesh)`:
  1. `PMP::remove_isolated_vertices(mesh)` — elimina vértices no referenciados por ninguna cara.
  2. Si la malla es cerrada y triangular, verifica `PMP::is_outward_oriented(mesh)`. Si es *inward*, invoca `PMP::reverse_face_orientations(mesh)`.
- `areMeshesIdentical(a, b)`: compara conteo de vértices, caras y bounding box con tolerancia `1e-9`. Si las mallas son idénticas, aborta la operación booleana para evitar la violación de aserción interna de CGAL (`enodes.size()`).
- `logBooleanException(...)`: traduce los mensajes de excepción de CGAL a diagnósticos legibles por el usuario (p. ej. *intersecciones no autorizadas*, *violación de aserción*).

---

### 4.2 `SceneContent` — gestión de la escena PR6

**Ubicación:** `clion/Rendering/SceneContent.h` / `.cpp`

Se han añadido miembros y métodos específicos para la práctica 6:

#### Miembros nuevos

```cpp
bool _isPr6Active = false;
CgalBooleanOperations* _cgalBool = nullptr;
Model3D* _drawBoolA = nullptr;      // Modelo visual A
Model3D* _drawBoolB = nullptr;      // Modelo visual B
Model3D* _drawBoolResult = nullptr; // Resultado visual
float _pr6SimplifyRatio = 1.0f;
std::string _pr6ModelPathA, _pr6ModelPathB;
```

#### Métodos nuevos

| Método | Función |
|--------|---------|
| `buildPr6()` | Activa la bandera `_isPr6Active` e imprime instrucciones en consola. |
| `loadBooleanModelA(path)` / `loadBooleanModelB(path)` | Crea (o reutiliza) la instancia de `CgalBooleanOperations`, carga el modelo en el motor booleano y simultáneamente añade un `DrawMesh` visual a la escena con el mismo path. El modelo visual recibe el nombre `PR6_ModeloA` / `PR6_ModeloB`. |
| `runBooleanUnion()` / `runBooleanIntersection()` / `runBooleanDifferenceAB()` / `runBooleanDifferenceBA()` | Flujo completo de ejecución: (1) valida que ambos modelos sean válidos, (2) invoca `applyModelMatrixToMeshes` con las matrices actuales de los objetos visuales, (3) ejecuta la operación booleana, (4) exporta el resultado a `TriangleModel*`, (5) crea un `DrawBooleanResult`, (6) oculta A y B y muestra el resultado. Cada operación asigna un color distintivo al resultado (naranja para unión, azul para intersección, etc.). |
| `simplifyBooleanResult(ratio)` | Delegado directo a `_cgalBool->simplifyResult(ratio)`. Permite reducir polígonos del resultado ya generado. |
| `clearBooleanScene(clearAll)` | Si `clearAll == true`, elimina A, B y resultado; si es `false`, solo elimina el resultado anterior (para permitir reejecutar operaciones sin volver a cargar modelos). |
| `showBooleanInputs()` | Vuelve a hacer visibles A y B y oculta el resultado, permitiendo al usuario reajustar las transformaciones antes de una nueva operación. |

#### Clase anidada `DrawBooleanResult`

Declarada directamente en `SceneContent.cpp`, hereda de `Model3D`. Su constructor recibe un `TriangleModel*` y construye un único `Component` con:
- Vértices de tipo `VAO::Vertex` (posición + normal + UV).
- Cálculo de normales por cara mediante `glm::cross(b-a, c-a)`.
- Índices de triángulos en `IBO_TRIANGLE`.

Esto evita depender del pipeline de carga de Assimp para la geometría resultado, que ya proviene de CGAL en forma puramente triangular.

---

### 4.3 `GUI` — interfaz de usuario

**Ubicación:** `clion/Rendering/GUI.cpp` / `GUI.h`

Se ha rediseñado completamente la ventana principal (`showModelMenu`) con un estilo *dark/orange* tipo Blender.

#### Cambios estructurales

- **Ventanas activas:** solo se muestran habitualmente dos ventanas ImGui:
  1. *Proyecto Final — Booleanas CGAL* (antes *Models*).
  2. *Rendering* (controles de color, gamma, etc.).
- Los botones de PR1–PR5 se han preservado en código pero están ocultos tras `#if 0`.
- Se añadieron botones específicos para PR6 dentro del panel principal:
  - *Cargar Modelo A / B*
  - *Unión / Intersección / Diferencia A-B / Diferencia B-A*
  - *Volver a modelos* (alterna visibilidad entre inputs y resultado)
  - *Slider de ratio de simplificación* + *Aplicar reducción*
  - *Guardar resultado .obj*
- **Diagnóstico visual en tiempo real:** bajo los botones de carga se muestra el estado de cada modelo (vértices, caras, válido/no válido) con colores semáforo (verde/rojo).

#### Inspector de componentes (lista lateral)

En la parte inferior de la ventana *Booleanas CGAL* aparece una lista de todos los componentes de los modelos cargados. Al seleccionar uno, el panel derecho muestra:
- Checkbox de visibilidad (`Enabled`).
- Botón **Centrar cámara en modelo**: invoca `Camera::track(model)` para situar el `lookAt` en el centro del AABB del modelo y reposicionar el ojo.
- Controles de material (`Kd`, `Ks`, `metallic`, `roughness`, colores de línea/punto).
- Modos de topología (Point Cloud, Wireframe, Triangle Mesh).
- **Transformación numérica** (nuevo):
  - `Traslación` — `InputFloat3` sobre la traslación extraída de la matriz.
  - `Rotación (grados)` — conversión automática `glm::degrees` / `glm::radians`.
  - `Escala` — `InputFloat3` sobre los ejes escalados.
  - Al modificar cualquier campo se reconstruye la matriz modelo con `glm::translate * mat4_cast(quat) * glm::scale`.

#### FileDialog

Se mantienen los diálogos de `ImGuiFileDialog` para:
- `OPEN_BOOL_A`, `OPEN_BOOL_B` — aceptan `.obj` y `.off`.
- `SAVE_BOOL_RESULT` — exporta a `.obj`.

---

### 4.4 `InputManager` — entrada de teclado y ratón

**Ubicación:** `clion/Rendering/InputManager.cpp` / `.h`

#### Cambios realizados

1. **Desactivación de teclas cuando ImGui tiene el foco:**
   ```cpp
   void processPressedKeyEvent(int key, int mods)
   {
       if (ImGui::GetIO().WantCaptureKeyboard)
           return;
       // ... resto de manejo de teclas
   }
   ```
   Esto evita que al escribir números en los `InputFloat3` de transformación se disparen las teclas de topología (`0`, `1`, `2`).

2. **Reasignación de teclas de órbita:**
   - `X` → órbita vertical (`orbitY`).
   - `Y` → desactivada (`ivec2(0)`).
   - `Q`/`E` → boom / crane (movimiento vertical de cámara).

3. **Ratón:**
   - Clic izquierdo o derecho + arrastrar invoca `pan` (horizontal) y `tilt` (vertical) con velocidades de `0.002` rad/píxel.
   - Rueda del ratón → `zoom` (FOV).

---

### 4.5 `Camera` — corrección de inversión de ejes

**Ubicación:** `clion/Rendering/Camera.cpp`

Los métodos de rotación (`orbitXZ`, `orbitY`, `pan`, `tilt`) fueron refactorizados para evitar la deriva numérica y la inversión brusca de controles al cruzar el zenit/nadir.

#### Mecanismo implementado

1. **Recálculo consistente de ejes:** en lugar de acumular `_u`, `_v`, `_n` manualmente (lo que provocaba que `cross(_n, _u)` cambiara de signo), se recalculan siempre mediante `CameraProjection::computeAxes(n, u, v)`.
2. **Detección de inversión:** después de `computeAxes`, se compara el nuevo `_u` con el anterior mediante el producto escalar:
   ```cpp
   vec3 oldU = this->_properties._u;
   this->_properties.computeAxes(...);
   if (glm::dot(oldU, this->_properties._u) < 0.0f)
   {
       this->_properties._u = -this->_properties._u;
       this->_properties._v = -this->_properties._v;
   }
   ```
   Si `dot < 0`, significa que `computeAxes` ha producido un salto de 180° (gimbal lock). Se invierten `_u` y `_v` para mantener la continuidad del sistema de referencia.
3. **Sincronización de `_up`:** `_up = _v` garantiza que `glm::lookAt` reciba un vector "arriba" coherente con la orientación actual de la cámara.

#### `Camera::track(Model3D* model)`

Nuevo método (ya existía en la base pero se expuso para la GUI). Calcula el centro del AABB del modelo (`aabb.center()`) y lo establece como `lookAt`. La posición del ojo se sitúa ligeramente por encima y alejada, escalada por el tamaño del modelo:
```cpp
setLookAt(aabb.center());
setPosition(aabb.min() + vec3(0, extent.y, 1.0f) - vec3(extent.x, 0, 0) * (1 + 4.0f / maxSize));
```

---

### 4.6 Colores por defecto (`ApplicationState` y `Model3D`)

**Ubicaciones:** `clion/Rendering/ApplicationState.h`, `clion/Rendering/Model3D.h`

Se modificaron los valores por defecto para ofrecer un tema oscuro coherente:

| Elemento | Valor anterior | Valor nuevo |
|----------|---------------|-------------|
| Fondo (`_backgroundColor`) | `vec3(0.6f)` (gris claro) | `vec3(0.15f)` (gris oscuro) |
| Material / triángulos (`_globalTriangleColor`) | Amarillo dorado `(1.0, 0.81, 0.29, 1.0)` | Naranja `(1.0, 0.5, 0.0, 1.0)` |
| Nube de puntos (`_globalPointColor`) | Azul `(0.0, 0.5, 1.0)` | Blanco `(1.0, 1.0, 1.0)` |
| Material por componente (`Model3D::Material`) | Amarillo dorado / puntos negros | Naranja / puntos blancos |

---

## 5. Algoritmos utilizados

### 5.1 Operaciones booleanas: `corefine_and_compute_boolean_operations`

**Referencia CGAL:** `CGAL::Polygon_mesh_processing::corefine_and_compute_boolean_operations`

Este algoritmo realiza simultáneamente:
1. **Corefinement:** inserta en ambas mallas los vértices y aristas de intersección, de modo que las regiones solapadas queden perfectamente alineadas topológicamente.
2. **Clasificación de componentes conexas:** determina qué porciones de cada malla pertenecen al interior o exterior de la otra.
3. **Construcción de resultados:** a partir de la clasificación, ensambla hasta cuatro mallas de salida (unión, intersección, diferencia AB, diferencia BA) en una sola pasada.

Ventaja frente a llamadas individuales: el costoso paso de *corefinement* solo se ejecuta una vez. La firma empleada es:
```cpp
std::array<bool,4> results = PMP::corefine_and_compute_boolean_operations(
    meshA, meshB,
    { &out_union, &out_inter, &out_diff_ab, &out_diff_ba }
);
```

### 5.2 Simplificación de mallas: `edge_collapse`

**Referencia CGAL:** `CGAL::Surface_mesh_simplification::edge_collapse`

Se utiliza la política de parada `Edge_count_ratio_stop_predicate<double>(ratio)`. El algoritmo colapsa aristas iterativamente mientras el coste de colapso (medido como variación de forma local) sea aceptable, deteniéndose cuando el número de aristas restantes alcanza la fracción `ratio` respecto al original.

### 5.3 Reparación y orientación

- `PMP::triangulate_faces`: convierte caras poligonales (>3 lados) en triángulos mediante ear-clipping.
- `PMP::remove_isolated_vertices`: limpia vértices huérfanos que no aportan geometría.
- `PMP::is_outward_oriented` + `PMP::reverse_face_orientations`: garantiza que las normales apunen hacia fuera, requisito para que `is_closed` sea consistente con el sentido físico de la malla.

### 5.4 Validación de entrada

`isValidForBoolean` impone tres restricciones fundamentales derivadas de las precondiciones de CGAL:
1. **Malla triangular:** cada cara debe tener exactamente 3 vértices.
2. **Malla cerrada (*watertight*):** cada arista debe ser compartida por exactamente 2 caras (ningún borde libre).
3. **No vacía:** al menos una cara.

### 5.5 Transformaciones visuales → CGAL

Antes de cada operación booleana, los vértices de las copias originales (`_meshA_orig`, `_meshB_orig`) se restauran y se transforman mediante `transformPoint`. La matriz aplicada es la misma que usa OpenGL/GLM para el objeto visual, garantizando coherencia entre lo que el usuario ve y lo que CGAL computa. La corrección **column-major** es esencial: GLM almacena `m[col][row]`, por lo que el acceso a la matriz en el transformador usa esa convención.

---

## 6. Problemas encontrados y soluciones

### 6.1 Exportación colgaba en modelos grandes (O(n²))
**Síntoma:** Al exportar resultados grandes (p. ej. *Ajax*), la aplicación se congelaba.
**Causa:** La conversión a `TriangleModel` iteraba sobre `Triangle3d` y buscaba vértices duplicados con búsqueda lineal.
**Solución:** Se añadió un constructor `TriangleModel(std::vector<Vect3d>&&, std::vector<unsigned>&&)` que acepta directamente los vectores de vértices e índices ya indexados, evitando toda deduplicación.

### 6.2 Violación de aserción con modelos idénticos
**Síntoma:** CGAL abortaba con `assertion violation: enodes.size()`.
**Causa:** Si A y B son exactamente la misma malla (mismos vértices y caras), el corefinement entra en un caso degenerado.
**Solución:** `areMeshesIdentical()` compara bounding boxes, número de vértices y caras. Si detecta identidad, la operación booleana se aborta con un mensaje explicativo.

### 6.3 Pérdida de rendimiento por `is_closed` cada frame
**Síntoma:** ~12 FPS de caída al tener modelos cargados.
**Causa:** `is_closed` se evaluaba en cada frame para colorear el indicador de validez.
**Solución:** Cachear el resultado en `_validA` / `_validB` booleanos; solo se recalculan al cargar o reparar la malla.

### 6.4 `clearBooleanScene(false)` cerraba el panel
**Síntoma:** Al ejecutar una operación booleana, el panel PR6 desaparecía.
**Causa:** `clearBooleanScene(false)` ponía `_isPr6Active = false`.
**Solución:** Separar la limpieza de objetos de la desactivación del modo; `_isPr6Active` solo cambia con `clearBooleanScene(true)` o al salir manualmente del modo.

### 6.5 Carga de OBJ no estándar
**Síntoma:** Modelos descargados de internet (quads, ngons, líneas de cara irregulares) fallaban con `CGAL::IO::read_OBJ`.
**Solución:** Estrategia de doble intento: primero CGAL nativo (rápido, preserva topología exacta); si falla, fallback a Assimp vía `TriangleModel` (robusto, auto-triangula, une vértices).

### 6.6 Inversión de controles de cámara
**Síntoma:** Al pasar la cámara por encima del objeto, mover el ratón a la derecha iba a la izquierda.
**Causa:** Acumulación de errores numéricos en los ejes `_u`, `_v`, `_n` al recalcular `_up` con `cross(_n, _u)` manualmente.
**Solución:** Recalcular siempre los ejes con `computeAxes()` y detectar saltos de 180° mediante `dot(oldU, newU) < 0`.

---

## 7. Manual de usuario

### 7.1 Inicio y flujo general

1. Ejecuta `AG_CLion.exe` (o desde CLion en modo Debug).
2. Aparecerá la ventana principal con fondo gris oscuro.
3. En la ventana **Proyecto Final — Booleanas CGAL**, pulsa el botón **PROYECTO FINAL** para activar el modo.

### 7.2 Carga de modelos

- Pulsa **Cargar Modelo A** y selecciona un archivo `.obj` o `.off` (recomendado: mallas cerradas sin agujeros: cubo, esfera, toro, etc.).
- Repite con **Cargar Modelo B**.
- En la ventana aparecerá un diagnóstico en tiempo real:
  - **Verde:** modelo válido (cerrado y triangulado).
  - **Rojo:** modelo no válido. La operación booleana no se habilitará hasta que ambos sean verdes.

> **Consejo:** puedes trasladar, rotar o escalar los modelos antes de la operación usando el gizmo 3D (aparece sobre el modelo seleccionado) o los campos numéricos del inspector.

### 7.3 Operaciones booleanas

Una vez ambos modelos son **válidos**:
- **Unión** (`A ∪ B`): combina ambos volúmenes.
- **Intersección** (`A ∩ B`): conserva solo la parte común.
- **Diferencia A-B**: resta B de A.
- **Diferencia B-A**: resta A de B.

Tras ejecutar una operación:
- Los modelos A y B se ocultan automáticamente.
- Aparece el **resultado** con un color distintivo.

### 7.4 Post-procesado del resultado

- **Simplificación:** ajusta el slider *Ratio simplificación* (1.0 = sin cambios, 0.1 = reduce drásticamente) y pulsa **Aplicar reducción**.
- **Exportación:** pulsa **Guardar resultado .obj** para salvar el mesh resultante a disco.

### 7.5 Volver a editar / re-ejecutar

- Pulsa **Volver a modelos** para ocultar el resultado y mostrar de nuevo A y B. Puedes moverlos con el gizmo y reejecutar otra operación sin necesidad de recargar.
- Pulsa **Limpiar Proyecto** para descargar todos los modelos y empezar de cero.

### 7.6 Controles de cámara

| Acción | Entrada |
|--------|---------|
| Orbitar vertical (arriba/abajo) | Tecla **`X`** (+ `Ctrl` para invertir sentido) |
| Orbitar horizontal (derecha/izquierda) | *Desactivada* (se usa ratón) |
| Pan (girar alrededor del punto de interés) | **Clic izquierdo / derecho** + arrastrar horizontalmente |
| Tilt (inclinar arriba/abajo) | **Clic izquierdo / derecho** + arrastrar verticalmente |
| Zoom | **Rueda del ratón** |
| Desplazamiento vertical | **`Q`** (subir) / **`E`** (bajar) |
| Dolly (adelante/atrás) | **`W`** / **`S`** con clic derecho pulsado |
| Truck (izquierda/derecha) | **`A`** / **`D`** con clic derecho pulsado |
| Centrar cámara en modelo | Selecciona el modelo en la lista de componentes y pulsa **Centrar cámara en modelo** |
| Reset de cámara | **`B`** |
| Cambiar topología visual | **`0`** (puntos), **`1`** (líneas), **`2`** (triángulos) |

> **Nota:** si un campo numérico de ImGui tiene el foco, las teclas de topología (`0`, `1`, `2`) no se activan para evitar conflictos.

### 7.7 Controles de renderizado (ventana *Rendering*)

- **Background color:** gris oscuro por defecto.
- **Wireframe / Triángulos / Nube de puntos:** colores globales aplicables a todos los modelos.
- **Grosor de línea** y **tamaño de punto** ajustables con sliders.
- **Z near / Z far:** útil si modelos muy grandes o muy pequeños se recortan.
- **Gamma:** corrección de gamma global.

### 7.8 Requisitos de los modelos de entrada

Para que CGAL pueda operar, los modelos deben cumplir:
1. Ser **mallas de triángulos** (todo polígono se triangula automáticamente en carga).
2. Ser **mallas cerradas** (*watertight*): cada arista debe pertenecer a exactamente dos triángulos.
3. Estar **orientadas hacia fuera** (el reparador automático intenta corregirlo).
4. **No ser idénticos** entre sí (mueve ligeramente uno con el gizmo si lo son).

---

## 8. Conclusión

Se ha construido un módulo de operaciones booleanas 3D robusto e integrado en el visor existente, resolviendo problemas de rendimiento, estabilidad y usabilidad. El uso de CGAL garantiza la corrección geométrica de los resultados, mientras que los mecanismos de fallback (Assimp), reparación automática y caché de validez hacen que la herramienta sea práctica para el usuario final. La interfaz rediseñada y los controles de cámara mejorados permiten una inspección fluida de los modelos antes y después de las operaciones.
