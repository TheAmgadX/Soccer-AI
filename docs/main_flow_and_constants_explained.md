# Soccer AI: `main.cc` Game Flow + `constants.h` Reference

This guide focuses on the game loop and data flow. Rendering internals are kept brief.

## 1) `main.cc` line-by-line (game-flow focused)

### Includes and setup
- `src/main.cc:1`: Includes SDL2 API.
- `src/main.cc:3`: Includes `std::sort` support.
- `src/main.cc:4`: Includes fixed-size arrays (`std::array`).
- `src/main.cc:5`: Includes math functions (`sqrt`, `atan2`, `sin`, `cos`, `lround`).
- `src/main.cc:7`: Includes your project constants.
- `src/main.cc:9`: Brings all names from `constants` namespace into this file.

### Data structures
- `src/main.cc:11`: Starts `Entity` type.
- `src/main.cc:12`: `x` position.
- `src/main.cc:13`: `y` position.
- `src/main.cc:14`: heading/direction x component.
- `src/main.cc:15`: heading/direction y component.
- `src/main.cc:16`: Ends `Entity`.
- `src/main.cc:18`: Starts `IntPoint` helper (integer 2D point for raster drawing).
- `src/main.cc:19`: point x.
- `src/main.cc:20`: point y.
- `src/main.cc:21`: Ends `IntPoint`.

### Main class and lifecycle
- `src/main.cc:23`: Starts `SoccerVisualizer` class (holds state + render methods).
- `src/main.cc:24`: Public API begins.

#### Initialization and shutdown
- `src/main.cc:25`: `Init()` starts.
- `src/main.cc:26`: Initializes SDL video subsystem.
- `src/main.cc:27`: Fails early if SDL init fails.
- `src/main.cc:30-32`: Creates window using screen constants.
- `src/main.cc:33`: Creates hardware-accelerated renderer.
- `src/main.cc:35`: Initializes default team/ball state.
- `src/main.cc:36`: Returns success only if window + renderer exist.
- `src/main.cc:39`: `Cleanup()` starts.
- `src/main.cc:40`: Destroys renderer.
- `src/main.cc:41`: Destroys window.
- `src/main.cc:42`: Shuts down SDL.

#### State update hooks (this is where your AI plugs in)
- `src/main.cc:45`: `UpdateRedPlayer(...)` starts.
- `src/main.cc:46-48`: Guards invalid index.
- `src/main.cc:49`: Writes red player position + heading.
- `src/main.cc:52`: `UpdateBluePlayer(...)` starts.
- `src/main.cc:53-55`: Guards invalid index.
- `src/main.cc:56`: Writes blue player position + heading.
- `src/main.cc:59`: `UpdateBall(...)` starts.
- `src/main.cc:60`: Writes ball x.
- `src/main.cc:61`: Writes ball y.

#### Per-frame render entrypoint
- `src/main.cc:64`: `RenderFrame()` starts.
- `src/main.cc:65`: Clears previous frame.
- `src/main.cc:66`: Draws pitch.
- `src/main.cc:68`: Loops exactly `PLAYERS_PER_TEAM` times.
- `src/main.cc:69`: Draws red player, treating `GOALKEEPER_INDEX` specially.
- `src/main.cc:70`: Draws blue player, same goalkeeper rule.
- `src/main.cc:73`: Draws ball.
- `src/main.cc:74`: Presents frame to screen.

#### Demo/default state
- `src/main.cc:77`: `ResetDemoState()` starts.
- `src/main.cc:78`: Ball starts at `BALL_START_X/Y`.
- `src/main.cc:80`: Loop fills initial teams.
- `src/main.cc:81-82`: Red players: start + spacing constants, facing right.
- `src/main.cc:83-84`: Blue players: mirrored start + spacing, facing left.

### Private members (actual game state storage)
- `src/main.cc:88`: Private section begins.
- `src/main.cc:89`: Window handle.
- `src/main.cc:90`: Renderer handle.
- `src/main.cc:92`: Array of red-team entities.
- `src/main.cc:93`: Array of blue-team entities.
- `src/main.cc:94`: Ball entity.

### Graphics helpers (brief; not game-flow critical)
- `src/main.cc:96-98`: Vector length helper.
- `src/main.cc:100-103`: Clear-screen helper.
- `src/main.cc:105-115`: Thick line raster helper.
- `src/main.cc:117-123`: Thick rectangle outline helper.
- `src/main.cc:125-150`: Circle outline raster helper.
- `src/main.cc:152-157`: Filled circle helper.
- `src/main.cc:159-198`: Triangle fill helper.
- `src/main.cc:200-211`: Goal net drawing helper.
- `src/main.cc:213-263`: Pitch drawing routine.
- `src/main.cc:265-306`: Player triangle drawing routine.
- `src/main.cc:308-320`: Ball drawing routine.

### Program entry point (`main`)
- `src/main.cc:323`: Program starts.
- `src/main.cc:324-325`: Marks CLI args unused.
- `src/main.cc:327`: Creates visualizer instance.
- `src/main.cc:328-330`: Initializes SDL/window/renderer or exits `-1`.
- `src/main.cc:332`: Game loop control flag.
- `src/main.cc:333`: SDL event container.
- `src/main.cc:335`: Main loop starts.
- `src/main.cc:336`: Poll events loop.
- `src/main.cc:337-339`: Quit when window close event is received.
- `src/main.cc:342-346`: Comment shows where your AI should call update methods each frame.
- `src/main.cc:348`: Renders one frame.
- `src/main.cc:349`: Frame cap delay using constant.
- `src/main.cc:352`: Cleanup on exit.
- `src/main.cc:353`: Successful exit code.
- `src/main.cc:354`: End of `main`.

## 2) `constants.h` reference (every constant)

### Screen and pitch bounds
- `include/utils/constants.h:5` `SCREEN_WIDTH`: Window width in pixels.
- `include/utils/constants.h:6` `SCREEN_HEIGHT`: Window height in pixels.
- `include/utils/constants.h:8` `PITCH_MARGIN`: Padding between screen edge and pitch.
- `include/utils/constants.h:9` `PITCH_LEFT`: Pitch left x coordinate.
- `include/utils/constants.h:10` `PITCH_TOP`: Pitch top y coordinate.
- `include/utils/constants.h:11` `PITCH_WIDTH`: Pitch width.
- `include/utils/constants.h:12` `PITCH_HEIGHT`: Pitch height.
- `include/utils/constants.h:13` `PITCH_RIGHT`: Pitch right x coordinate.
- `include/utils/constants.h:14` `PITCH_BOTTOM`: Pitch bottom y coordinate.

### Field center
- `include/utils/constants.h:16` `FIELD_CENTER_X`: Center x of the field.
- `include/utils/constants.h:17` `FIELD_CENTER_Y`: Center y of the field.

### Goal geometry
- `include/utils/constants.h:19` `GOAL_DEPTH`: Goal extends this many pixels outward from goal line.
- `include/utils/constants.h:20` `GOAL_HEIGHT`: Goal opening height.
- `include/utils/constants.h:21` `GOAL_TOP_Y`: Top y of goal mouth.
- `include/utils/constants.h:22` `GOAL_BOTTOM_Y`: Bottom y of goal mouth.

### Goal target coordinates for AI
- `include/utils/constants.h:25` `LEFT_GOAL_X`: X of left goal line (where scoring/targeting can be checked).
- `include/utils/constants.h:26` `RIGHT_GOAL_X`: X of right goal line.
- `include/utils/constants.h:29` `LEFT_GOAL_LEFT_Y`: Upper lane target y for left goal.
- `include/utils/constants.h:30` `LEFT_GOAL_CENTER_Y`: Center lane target y for left goal.
- `include/utils/constants.h:31` `LEFT_GOAL_RIGHT_Y`: Lower lane target y for left goal.
- `include/utils/constants.h:33` `RIGHT_GOAL_LEFT_Y`: Upper lane target y for right goal.
- `include/utils/constants.h:34` `RIGHT_GOAL_CENTER_Y`: Center lane target y for right goal.
- `include/utils/constants.h:35` `RIGHT_GOAL_RIGHT_Y`: Lower lane target y for right goal.

### Box geometry
- `include/utils/constants.h:37` `PENALTY_BOX_DEPTH`: Penalty box depth from goal line.
- `include/utils/constants.h:38` `PENALTY_BOX_HEIGHT`: Penalty box height.
- `include/utils/constants.h:39` `GOAL_BOX_DEPTH`: Goal box depth from goal line.
- `include/utils/constants.h:40` `GOAL_BOX_HEIGHT`: Goal box height.

### Team composition
- `include/utils/constants.h:42` `PLAYERS_PER_TEAM`: Number of players each side has.
- `include/utils/constants.h:43` `GOALKEEPER_INDEX`: Which player index is treated as the goalkeeper.

### Frame timing
- `include/utils/constants.h:46` `TARGET_FPS`: Intended frames per second.
- `include/utils/constants.h:47` `FRAME_DELAY_MS`: Delay per frame (`1000 / TARGET_FPS`).

### Pitch visual tuning
- `include/utils/constants.h:50` `PITCH_STRIPE_COUNT`: Number of grass stripes.
- `include/utils/constants.h:51` `LINE_THICKNESS`: Thickness for white field lines.
- `include/utils/constants.h:52` `CENTER_CIRCLE_RADIUS`: Radius of center circle.
- `include/utils/constants.h:53` `CENTER_SPOT_RADIUS`: Radius of center spot.
- `include/utils/constants.h:54` `PENALTY_MARK_OFFSET_X`: Horizontal offset of penalty marks from each goal line.

### Goal net visual tuning
- `include/utils/constants.h:57` `GOAL_NET_INSET`: Margin inside goal rect before drawing net lines.
- `include/utils/constants.h:58` `GOAL_NET_SPACING`: Distance between net grid lines.
- `include/utils/constants.h:59` `GOAL_NET_START_OFFSET`: Initial offset for first net line.

### Player visual tuning
- `include/utils/constants.h:62` `PLAYER_SIZE_FIELD`: Triangle size for field players.
- `include/utils/constants.h:63` `PLAYER_SIZE_GOALKEEPER`: Triangle size for goalkeeper.
- `include/utils/constants.h:64` `PLAYER_WING_ANGLE_OFFSET`: Triangle side-point angle offset from heading.
- `include/utils/constants.h:65` `PLAYER_OUTLINE_THICKNESS`: Outline thickness on player triangles.

### Ball visual tuning
- `include/utils/constants.h:68` `BALL_RADIUS`: Ball radius in pixels.
- `include/utils/constants.h:69` `BALL_OUTLINE_THICKNESS`: Ball outline thickness.
- `include/utils/constants.h:70` `BALL_SHADOW_OFFSET_X`: Shadow x offset.
- `include/utils/constants.h:71` `BALL_SHADOW_OFFSET_Y`: Shadow y offset.

### Default/demo spawn positions
- `include/utils/constants.h:74` `BALL_START_X`: Initial/demo ball x.
- `include/utils/constants.h:75` `BALL_START_Y`: Initial/demo ball y.
- `include/utils/constants.h:77` `RED_TEAM_START_X`: X of first red player.
- `include/utils/constants.h:78` `BLUE_TEAM_START_X`: X of first blue player.
- `include/utils/constants.h:79` `TEAM_START_Y`: Y of first player row.
- `include/utils/constants.h:80` `TEAM_X_SPACING`: Horizontal spacing between player indices.
- `include/utils/constants.h:81` `TEAM_Y_SPACING`: Vertical spacing between player indices.

## 3) How frame flow works in practice

Each frame (`while (!quit)` in `src/main.cc:335`):
1. Read window/input events.
2. (Your AI step) call `UpdateRedPlayer`, `UpdateBluePlayer`, `UpdateBall` with new positions/headings.
3. Call `RenderFrame()`.
4. Delay to maintain target FPS.

This means your simulation/AI logic owns the world state updates; rendering just visualizes that state.
