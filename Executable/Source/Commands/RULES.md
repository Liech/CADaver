# Commands Architecture Rules

Rules governing all classes, interfaces, and implementations in `Source/Commands`.

## 1. Commands are glue Code
* Commands do not implement business logic, they only glue together existing components.

## 2. Mandatory Fields & Status Enums
* Every JSON field defined in a command request or response schema is strictly mandatory.
* Never use optional, conditional, or disappearing fields.
* Consumers must be able to read all schema fields unconditionally using `json.at("field")`.
* Operational outcomes are communicated via a mandatory `"status"` string (e.g. `"Success"`, `"NoInputStream"`, `"NoOutputStream"`, `"ParseFailed"`, `"AssetNotFound"`).
* On non-success status, payload fields still must be present populated with neutral/sentinel values (e.g. `index: -1`).

## 3. Unknown Origin Streams (Zero Filesystem)
* Commands must never include `<filesystem>` or interact with the local filesystem.
* Data input and output are performed strictly through abstract `std::istream` and `std::ostream`.
* Callers (UI file dialogs, network handlers, memory buffers, IPC) are responsible for opening streams.

## 4. Usable Answers (No Bloat)
* Return payloads must be concise and actionable (e.g. the resulting `index` of a loaded asset).
* Never return irrelevant bloat like global asset counts or duplicated state.

## 5. Coding & Naming Standards
* Conform to root `AGENTS.md` guidelines at all times.
* One file per class (`.h` declaration, `.cpp` implementation).
* Virtual destructor on all command classes; no `= default`.
* Member variables placed at the bottom of class declarations.
* CamelCase for variables, PascalCase for types and functions.
* Strictly formatted with the project's `.clang-format` (Allman braces, 4 spaces, pointer on left).
