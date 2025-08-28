# TitanPC: Advanced Unreal Engine 5 Cross-Platform Game Framework

This is a **comprehensive, production-ready** Unreal Engine 5 C++ game framework designed for **large-scale, cross-platform games** supporting both **PC and Android** platforms.

## 🚀 Major Features & Upgrades

### ✅ Cross-Platform Foundation
- **Android Build Support**: Complete Android target configuration with optimized build settings
- **Platform Detection**: Automatic platform detection with adaptive settings
- **Cross-Platform Plugin Support**: Enhanced plugin compatibility for PC/Android
- **Platform-Specific Optimizations**: Tailored performance settings for each platform

### ✅ Advanced Gameplay Systems

#### **Enhanced Movement System**
- **Wall Running**: Physics-based wall running with stamina consumption
- **Sliding**: Momentum-based sliding with deceleration
- **Parkour System**: Advanced climbing and traversal mechanics
- **Dash System**: Multi-directional dashing with cooldowns
- **Stamina Integration**: All enhanced movements consume stamina realistically

#### **Comprehensive Combat System**
- **Multi-Weapon Support**: Rifles, Pistols, Shotguns, SMGs, Snipers, Melee weapons
- **Advanced Firing Modes**: Single fire, burst fire, and full-auto
- **Realistic Ammo System**: Magazine-based reloading with reserve ammo tracking
- **Weapon Customization**: Data-driven weapon configuration system
- **Hitscan & Projectile Support**: Both instant-hit and ballistic weapons

#### **Gameplay Ability System (GAS) Integration**
- **16+ Gameplay Attributes**: Health, stamina, shield, attack power, defense, critical hit stats
- **Character Progression**: Levels, experience points, and skill points
- **Network Replication**: Full multiplayer-ready attribute replication
- **Effect System**: Damage, healing, and buff/debuff support

#### **Advanced Health & Shield System**
- **Multi-Layer Protection**: Health, shield, and armor systems
- **Regeneration Systems**: Configurable health and stamina regeneration
- **Status Effects**: Invulnerability, damage reduction, and critical hit resistance
- **Death & Revival**: Complete death handling with revival mechanics

### ✅ AI & NPC System
- **Smart AI Characters**: Advanced AI with perception, behavior trees
- **Combat AI**: Intelligent enemy AI with weapon usage and tactics
- **State Machine**: Comprehensive AI state management (Passive, Patrolling, Combat, Fleeing)
- **Personality System**: Configurable AI personality traits (Aggressiveness, Alertness, Courage)

### ✅ Cross-Platform UI & Controls

#### **Mobile Touch Controls**
- **Virtual Joysticks**: Custom-built touch joysticks for movement and camera
- **Touch Buttons**: Fire, jump, reload, sprint, aim, and melee buttons
- **Adaptive UI**: Automatically shows/hides based on platform
- **Touch-Optimized**: Larger UI elements and proper touch responses

#### **Platform-Adaptive HUD**
- **Health/Stamina Bars**: Real-time status monitoring with color coding
- **Ammo Display**: Current and reserve ammo with reload progress
- **Weapon Information**: Current weapon stats and icons
- **Character Progression**: Level, experience, and skill point tracking

### ✅ Advanced Subsystem Architecture

#### **Inventory System**
- **Item Management**: Add, remove, and organize items with stack limits
- **Data-Driven Items**: Configurable item properties via data tables
- **Event System**: Item add/remove/use events for UI updates
- **Tag-Based Filtering**: GameplayTag-based item categorization

#### **Save/Load System**
- **Multiple Save Slots**: Support for multiple save games
- **Auto-Save**: Configurable automatic saving with intervals
- **Comprehensive Data**: Player stats, inventory, progress, and world state
- **Platform Storage**: Cross-platform save game compatibility

#### **Settings System**
- **Graphics Settings**: Resolution, quality, VSync, frame rate limits
- **Audio Settings**: Master, music, SFX, voice, and UI volume controls
- **Input Settings**: Mouse/touch sensitivity, invert controls
- **Platform Optimization**: Automatic settings based on platform capabilities

#### **Audio System**
- **Multi-Channel Audio**: Separate volume controls for different audio types
- **3D Positional Audio**: Spatial audio for immersive gameplay
- **Music Management**: Background music with fade in/out
- **Voice System**: Character voice playback with interruption control

### ✅ Performance & Optimization

#### **Platform-Specific Performance**
- **Mobile Optimization**: Reduced quality settings, optimized rendering pipeline
- **PC Enhancement**: High-quality settings with advanced features enabled
- **Dynamic Quality Scaling**: Automatic quality adjustment based on performance
- **Memory Management**: Optimized asset loading and garbage collection

#### **Rendering Enhancements**
- **Lumen Integration**: Advanced global illumination for PC
- **Nanite Support**: Virtualized geometry for high-detail models
- **Platform Scaling**: Appropriate feature enabling based on platform capabilities
- **LOD Systems**: Distance-based level of detail for performance

### ✅ Network Architecture Foundation
- **Multiplayer Ready**: Network replication setup for all major systems
- **Ability System Replication**: GAS attributes and effects network ready
- **Movement Replication**: Enhanced movement system with network support
- **Modular Design**: Easy to extend for full multiplayer implementation

## 📁 Project Structure

```
TitanPC/
├── Source/TitanPC/
│   ├── TitanPCCharacter.h/.cpp         # Enhanced player character
│   ├── TitanPCGameInstance.h/.cpp      # Platform-aware game instance
│   ├── Components/                     # Modular component systems
│   │   ├── TitanPCMovementComponent    # Advanced movement
│   │   ├── TitanPCHealthComponent      # Health & shield system
│   │   └── TitanPCWeaponComponent      # Combat & weapon system
│   ├── Subsystems/                     # Game subsystems
│   │   ├── TitanPCInventorySubsystem   # Item management
│   │   ├── TitanPCSaveGameSubsystem    # Save/load system
│   │   ├── TitanPCSettingsSubsystem    # Settings management
│   │   └── TitanPCAudioSubsystem       # Audio management
│   ├── GAS/                           # Gameplay Ability System
│   │   └── TitanPCAttributeSet         # Character attributes
│   ├── AI/                            # AI & NPC systems
│   │   └── TitanPCAICharacter          # Smart AI characters
│   └── UI/                            # User interface
│       ├── TitanPCMobileHUD           # Cross-platform HUD
│       └── TitanPCVirtualJoystick     # Touch controls
├── TitanPC.uproject                   # Project configuration
├── TitanPCAndroid.Target.cs          # Android build target
└── README.md                         # This file
```

## 🛠 Setup & Build Instructions

### Prerequisites
- **Unreal Engine 5.3+**
- **Visual Studio 2022** with Desktop development with C++
- **Android Studio** (for Android builds)
- **Android NDK** and SDK tools

### PC Setup
1. Clone or download this repository
2. Extract to a folder outside `Program Files` (e.g., `C:\Dev\TitanPC`)
3. Double-click **TitanPC.uproject**
4. If prompted, click **Yes** to build missing modules
5. Wait for compilation to complete

### Android Setup
1. Install Android Studio and configure SDK/NDK
2. In Unreal Editor: **Edit → Project Settings → Android**
3. Configure your Android SDK/NDK paths
4. Set minimum SDK version to API 21 (Android 5.0)
5. Package for Android: **File → Package Project → Android**

### Configuration
1. **Edit → Project Settings → Maps & Modes**
   - Set **Default GameMode**: `TitanPCGameModeBase`
2. **Create Enhanced Input Assets** (or use provided ones):
   - Input Mapping Context for controls
   - Input Actions: Move, Look, Jump, Sprint, Fire, Aim, etc.
3. **Assign Input Assets** to `TitanPCCharacter` properties
4. **Create Weapon Data Table** for weapon configurations
5. **Press Play** to test the enhanced gameplay!

## 🎮 Controls & Gameplay

### PC Controls
- **WASD**: Movement
- **Mouse**: Look around
- **Space**: Jump / Wall jump
- **Shift**: Sprint
- **Ctrl**: Crouch
- **Alt**: Slide (while moving)
- **Q**: Wall run (near walls)
- **Mouse 1**: Fire weapon
- **Mouse 2**: Aim
- **R**: Reload
- **F**: Melee attack
- **E**: Interact
- **Tab**: Inventory

### Mobile Controls
- **Left Virtual Joystick**: Movement
- **Right Virtual Joystick**: Camera
- **Touch Buttons**: Fire, Jump, Sprint, Reload, Aim, Melee
- **Automatic UI**: Touch controls appear only on mobile platforms

### Advanced Gameplay Features
- **Wall Running**: Run along walls by approaching them at an angle
- **Sliding**: Build momentum and slide under obstacles
- **Parkour**: Climb ledges and traverse the environment
- **Combat**: Use multiple weapon types with realistic reloading
- **Character Progression**: Gain experience and level up
- **AI Enemies**: Face intelligent opponents with varied behaviors

## 🔧 Customization & Extension

### Adding New Weapons
1. Add entries to the **Weapon Data Table**
2. Configure damage, fire rate, ammo capacity, etc.
3. Assign weapon meshes, sounds, and effects
4. Use `WeaponComponent->AddWeapon(WeaponID)` in code

### Creating New Abilities
1. Create new `UGameplayAbility` subclass
2. Add to `StartupAbilities` array in character
3. Bind to input in `SetupPlayerInputComponent`
4. Use Gameplay Effects for damage/healing/buffs

### Extending AI Behavior
1. Create new Behavior Tree assets
2. Add custom Blackboard keys
3. Extend `TitanPCAICharacter` with new states
4. Configure AI perception and personality traits

### Platform Optimization
1. Edit `TitanPCGameInstance::OptimizeForPlatform()`
2. Add platform-specific quality settings
3. Configure mobile-specific UI layouts
4. Adjust performance targets per platform

## 🚀 Next Steps for Production

### Recommended Enhancements
- **World Partition**: Enable for massive open worlds
- **Networking**: Implement full multiplayer support
- **Content Pipeline**: Asset streaming and LOD systems
- **Localization**: Multi-language support
- **Analytics**: Player behavior tracking
- **Cloud Saves**: Cross-platform save synchronization
- **Monetization**: In-app purchases and progression systems

### Performance Profiling
- Use **Stat Unit** and **Stat FPS** for performance monitoring
- **Unreal Insights** for detailed performance analysis
- **Mobile Previewer** for Android optimization testing
- **Shipping Builds** for final performance validation

## 📄 License & Credits

This is an open-source game framework. Feel free to use, modify, and distribute for your projects.

**Built with:**
- Unreal Engine 5.3+
- Gameplay Ability System
- Enhanced Input System
- Common UI Plugin
- AI Perception System

---

## 💡 Tips for Development

1. **Always test on target platforms** - PC and Android have different performance characteristics
2. **Use the subsystem architecture** - Leverage the built-in inventory, save, and settings systems
3. **Extend don't replace** - The framework is designed to be extended rather than modified
4. **Profile early and often** - Performance optimization is easier when done incrementally
5. **Leverage GAS** - The Gameplay Ability System provides powerful, network-ready gameplay features

**This framework provides everything you need to create a professional, cross-platform action game with Unreal Engine 5!**
