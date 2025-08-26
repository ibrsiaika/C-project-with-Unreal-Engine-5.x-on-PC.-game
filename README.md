# TitanPC: Unreal Engine 5 C++ Starter

This is a clean starter intended for a **PC-first, large-scale** Unreal Engine 5 C++ game.

## Open & Build
1. Install **Unreal Engine 5.3+** and a supported C++ toolchain (Visual Studio 2022 with Desktop development with C++).
2. Extract this zip somewhere outside `Program Files` (e.g., `C:\Dev\TitanPC`).
3. Double-click **TitanPC.uproject**. If prompted, click **Yes** to build missing modules.
4. In the editor, go to **Edit → Project Settings → Maps & Modes** and set:
   - **Default GameMode**: `TitanPCGameModeBase`
5. Create **Enhanced Input** assets (Mapping Context + Actions) and assign them to the `ATitanPCCharacter` properties:
   - `IA_Move` (Axis2D), `IA_Look` (Axis2D), `IA_Jump` (Digital), `IA_Sprint` (Digital)
6. Press **Play**. You have movement, jump, and sprint.

## Code Highlights
- `UTitanPCGameInstance` sets target framerate via console var `t.MaxFPS`.
- `ATitanPCCharacter` uses **Enhanced Input** and a spring-arm third-person camera.
- `ATitanPCPlayerController` sets up basic input mode.
- `ATitanPCGameModeBase` wires default pawn/controller.

## Next Steps for a "Big" PC Game
- **Architecture**: Create `Subsystems` (GameInstance & World) for saving, inventory, progression.
- **Networking**: Use `Gameplay Ability System` for abilities, attributes, and replication.
- **World Partition**: Enable in Project Settings for massive worlds.
- **Performance**: Use **Nanite** meshes, profile with **Stat Unit**, **Unreal Insights**.
- **Build**: Package for Windows using **Development** then **Shipping** targets.

- download game open source zip link : https://github.com/cyberxtechofficial/C-project-with-Unreal-Engine-5.x-on-PC.-game/archive/refs/heads/main.zip
