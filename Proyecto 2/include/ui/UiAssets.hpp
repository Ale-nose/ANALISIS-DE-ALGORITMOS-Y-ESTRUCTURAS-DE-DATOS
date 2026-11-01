// Copyright Alejandro, Ashley y Kevin
#pragma once

/**
 * @brief Paths of the images used by the game. They are embedded in the
 * executable through assets/assets.qrc, so the ":/assets/" prefix is the
 * resource prefix and not a folder on the disk.
 */

// Players: normal image and image with the shield aura (shieldCount > 0)
inline constexpr const char* ASSET_PLAYER_1 = ":/assets/player1.svg";
inline constexpr const char* ASSET_PLAYER_1_SHIELD =
    ":/assets/player1_shield.svg";
inline constexpr const char* ASSET_PLAYER_2 = ":/assets/player2.svg";
inline constexpr const char* ASSET_PLAYER_2_SHIELD =
    ":/assets/player2_shield.svg";

// Board elements
inline constexpr const char* ASSET_TREASURE = ":/assets/treasure.svg";
inline constexpr const char* ASSET_PORTAL = ":/assets/portal.svg";
inline constexpr const char* ASSET_SHIELD_TILE = ":/assets/shield_tile.svg";
inline constexpr const char* ASSET_FOG = ":/assets/fog.svg";
inline constexpr const char* ASSET_MUD = ":/assets/mud.svg";

// Power-ups
inline constexpr const char* ASSET_JUMP_WALL = ":/assets/jump_wall.svg";
inline constexpr const char* ASSET_DOUBLE_PLAY = ":/assets/double_play.svg";
inline constexpr const char* ASSET_CONTROL_ENEMY = ":/assets/control_enemy.svg";
