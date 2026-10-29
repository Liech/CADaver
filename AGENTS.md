# AI Guidelines for Volumetric Workspace

Guidelines for AI agents and developers working on this codebase.

Give regular feedback about your thought process. At max after 10 Toolcalls.
Prefer to ask what to do next instead of tunnelvision fixing.

## 1. Comments
* **As little comments as possible**: Code should be clean, concise, and self-explanatory.
* Do not add obvious, redundant, or conversational comments.
* Avoid boilerplate Doxygen/docstrings unless explicitly requested.
* **Class description exception**: A very short class description (classical `//` comment) containing purpose and rules is permitted immediately above the class declaration.

## 2. Naming Conventions
* **Variables**: Always use `camelCase`.
  * **No ALL_CAPS**: Do not use uppercase for constants (e.g. use `minResolution`, `maxResolution`, not `MIN_RESOLUTION`).
  * **No snake_case**: Do not use snake_case for variables (e.g. use `voxelCount`, not `voxel_count`).
  * Member variables, local variables, and constants all follow `camelCase`.
* **Types and Functions**:
  * Classes, structs, enums, and functions/methods use `PascalCase` (e.g. `VoxelResolutionComboBox`, `rebuildSVOMesher`).

## 3. String Interfaces
* **`std::string` everywhere**: Use `const std::string&` (or `std::string`) for all public and internal interfaces and function signatures.
* **`const char*` is forbidden**: Never use `const char*` unless it is strictly unavoidable (e.g. invoking third-party C APIs like ImGui directly at the callsite).

## 4. Header vs Source Separation
* **No implementations in `.h`**: Header files must contain only declarations.
* **Exception**: Strictly necessary template definitions.
* All non-template functions, constructors, and destructors must be implemented in `.cpp`.
* Template implementations are not allowed inside the class declaration. put them below the class declaration.

## 5. Language Features, Constructors & Destructors
* **Always add a virtual destructor**: Every class must have a virtual destructor (`virtual ~ClassName();` in `.h`).
* **No `= default`**: The use of `= default` is strictly forbidden. Write explicit bodies in `.cpp`:
  ```cpp
  // In .h:
  VoxelResolutionComboBox();
  virtual ~VoxelResolutionComboBox();

  // In .cpp:
  VoxelResolutionComboBox::VoxelResolutionComboBox()
  {
  }

  VoxelResolutionComboBox::~VoxelResolutionComboBox()
  {
  }
  ```

## 6. Class & Struct Layout & Empty Lines
* never use using or typedef
* **Variables to the bottom**:
  * Member functions and the public interface must appear at the top.
  * Private helper functions follow.
  * Member variables, static constants, and data members must be placed at the very bottom of the class/struct declaration.
* **Empty lines serve a communication purpose**:
  * Use empty lines intentionally to separate functional groups.
  * Related methods (e.g. getters and setters) must be grouped together without empty lines between them.
  * Distinct responsibilities (e.g. constructors/destructor, operational methods like `render()`, getters/setters, queries, private helpers) must be separated by a single empty line:
  ```cpp
  class VoxelResolutionComboBox
  {
    public:
      VoxelResolutionComboBox();
      virtual ~VoxelResolutionComboBox();

      bool render();

      int  getResolution() const;
      void setResolution(int resolution);
  ...
  ```

## 7. Component Design & Clean Responsibility
* **General, clean components**: Design classes that own and manage their own information cleanly.
* Do not add redundant override overloads (e.g. label overrides or legacy parameter pass-throughs) just to accommodate callers.
* The usecase must adapt to the clean class API, not the class to legacy usecases.
* **Keep APIs minimal**: Do not add unnecessary getters/setters. Only expose what the component fundamentally requires (e.g. `getResolution()` / `setResolution(int)`).
* **Headless Project**: The `Project` class has to work fully headless including all recursive members (such as `Asset` and its derived classes). No rendering, UI, or backend-graphics dependencies (e.g. Vulkan, Vulcant, ImGui, or `Renderer`) may be included or referenced. All rendering functionality must be decoupled into separate asset renderers and factories.

## 8. Formatting & Clang-Format
* Strictly adhere to the project's `.clang-format` rules:
  * **Brace style**: `BreakBeforeBraces: Allman` (opening brace on a new line for functions, classes, control structures).
  * **Pointer alignment**: `PointerAlignment: Left` (e.g. `Type* ptr`, `Type& ref`).
  * **Indentation**: 4 spaces, no tabs (`UseTab: Never`).
  * **Line width**: `ColumnLimit: 200`.
  * **Declaration alignment**: `AlignConsecutiveDeclarations: Enabled: true`.
  * **Short functions**: `AllowShortFunctionsOnASingleLine: Empty` (only empty bodies `{}` on a single line).
  * **Line endings**: LF (`LineEnding: LF`).
* **Clang-format pass**: Always run a `clang-format -i` pass on any modified `.h` and `.cpp` files to guarantee compliance with the project's `.clang-format`.
* never use --i; use i--;

## 9. Modularity & File Structure
* **Single Responsibility**: If something can be isolated into a separate class that performs a specific task, extract it into a dedicated class.
* **One file per class / enum / entity**:
  * Each class, enum, or struct must reside in its own dedicated header (`.h`) and source (`.cpp`) file.
  * Do not bundle multiple classes or standalone enums into a single file.
* **No nested subclasses or enums**:
  * Never define a class, struct, or enum inside another class.
  * Every entity must be a standalone top-level declaration in its own dedicated file.
* **Nothing class-unrelated in `.cpp` files**:
  * No loose, free-standing constants, helper classes, helper functions, or anonymous namespaces in `.cpp`.
  * All constants, lookup tables, and helper functions must be actual members of the class defined in its header.

## 10. Unit Testing
* **Location**: Tests belong in the **same `.cpp` file** as the implementation being tested, placed at the bottom.
* **Guard**: Must be guarded by the test project preprocessor define:
  ```cpp
  #ifdef ISTESTPROJECT
  #include <catch2/catch_test_macros.hpp>

  TEST_CASE("ClassName/FeatureDescription")
  {
      ...
  }
  #endif
  ```
* **Catch2 without Sections**: Do not use `SECTION` blocks inside `TEST_CASE`. Write flat, independent `TEST_CASE` blocks instead.

## 11. Interface Barriers & Stability
* **Never break interface barriers**: Never bypass abstractions or reach across architectural layers (e.g. never call GLFW, raw Vulkan, or backend-internal functions from higher-level application/UI classes). Respect existing abstraction layers strictly.
* **Never change interfaces without asking**: Never modify existing public class interfaces, method signatures, or architectural contracts without explicit user approval.

## 12. Decrement Operators
* **Never use `--i`**: Always use postfix decrement `i--` instead of prefix decrement `--i`.

## 13. Misc
* When adding a nullptr check, make sure the variable can be a nullptr in any circumstances
* Do not add unecessary checks. Let it crash if it needs to crash.
* Avoid defensive programming or checking for empty/valid values when variables should already be valid (e.g. do not check `if (guid.toString().empty())` before returning it). Keep code minimal and direct.
