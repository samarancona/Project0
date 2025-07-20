# Project0

Project0 is an Unreal Engine sample that demonstrates two different gameplay styles built into a single code base:

- **Strategy** &mdash; a top down real‑time strategy style experience.
- **Twin Stick Shooter** &mdash; a fast paced action variant.

The project is set up for **Unreal Engine 5.6** as referenced by the `EngineAssociation` value in `Project0.uproject`.

## Opening the project

1. Install Unreal Engine 5.6.
2. Clone or download this repository.
3. Open `Project0.uproject` with the Unreal Editor. The editor will generate project files as needed.
4. Build the C++ code from your IDE (e.g. Visual Studio) or let Unreal prompt to build when opening.
5. Press **Play** in the editor to run.

## Required Plugins / Dependencies

This project relies on several Unreal Engine plugins:

- **Enhanced Input** (built‑in)
- **StateTree** and **GameplayStateTree**
- **Niagara** and **UMG** (for effects and UI)
- **FlatNodes** and **ElectronicNodes** (optional visual scripting helpers)

Make sure these plugins are enabled when opening the project. Some sample input bindings are defined in `Config/DefaultInput.ini` using the Enhanced Input system.

