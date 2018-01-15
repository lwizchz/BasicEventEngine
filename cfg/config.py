# Bind keys
bind("SDLK_F12", "screenshot()")
bind("SDLK_HOME", "pause()")

bind("SDLK_RETURN", "$RoomRestart")
bind("SDLK_w", "$MoveForward")
bind("SDLK_s", "$MoveBackward")
bind("SDLK_a", "$MoveLeft")
bind("SDLK_d", "$MoveRight")
bind("SDLK_q", "$MoveDown")
bind("SDLK_e", "$MoveUp")
bind("SDLK_z", "$NetworkStartSession")
bind("SDLK_x", "$NetworkJoinSession")
bind("SDLK_c", "$NetworkFindSessions")
bind("SDLK_1", "$StartSound")
bind("SDLK_2", "$StartSoundEcho")
bind("SDLK_n", "$Start3D")
bind("SDLK_m", "$End3D")

bind("SDLK_y", "$StartPath")
bind("SDLK_b", "$StartSerialize")
bind("SDLK_v", "$StartDeserialize")

# Configure default values
volume(0.2)

log("Loaded config.py") # Output status
