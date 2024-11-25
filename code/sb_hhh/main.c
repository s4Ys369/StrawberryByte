#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>
#include <t3d/t3dskeleton.h>
#include <t3d/t3danim.h>
#include <t3d/t3ddebug.h>

// May make this an easter egg
//#define AI_BATTLE

#define ACTOR_COUNT 4
#define PLAYER_COUNT core_get_playercount()

#ifndef AI_BATTLE
#define AI_COUNT ACTOR_COUNT - PLAYER_COUNT
#else
#define AI_COUNT ACTOR_COUNT
#endif

#define SCENERY_COUNT 1
#define PLATFORM_COUNT 19

#define S4YS 0
#define WOLFIE 1
#define MEW 2
#define DOGMAN 3

#include "../../core.h"
#include "../../minigame.h"

#include "screen/screen.h"
#include "control/controls.h"
#include "time/time.h"

#include "physics/physics.h"

#include "camera/camera.h"
#include "camera/camera_states.h"
#include "camera/camera_control.h"

#include "sound/sound.h"

#include "actor/actor.h"
#include "actor/actor_states.h"
#include "actor/actor_motion.h"
#include "actor/actor_control.h"
#include "actor/actor_animation.h"

#include "player/player.h"

#include "scene/scene.h"
#include "scene/scenery.h"
#include "scene/platform.h"
#include "scene/room.h"

#include "actor/collision/actor_collision_detection.h"
#include "actor/collision/actor_collision_response.h"

#include "player/ai.h"

#include "ui/ui.h"

// TPX
#include <t3d/tpx.h>
#include "scene/particles.h"

#include "game/game.h"
#include "game/game_control.h"
#include "game/game_states.h"


const MinigameDef minigame_def = {
    .gamename = "Hot Hot Hexagons",
    .developername = "Strawberry Byte: .zoncabe, s4ys, mewde",
    .description = "The floor is lava!",
    .instructions = "Jump from platform to platform\nto avoid a terrible fate."
};

Game minigame = {
	.state = INTRO
};

Player player[MAXPLAYERS];

AI aiPlayer[MAXPLAYERS];

Actor actors[ACTOR_COUNT];

ActorCollider actor_collider[ACTOR_COUNT];

ActorContactData actor_contact[ACTOR_COUNT];

Scenery scenery[SCENERY_COUNT];

void minigame_init()
{      
	game_init(&minigame);
#ifdef PROFILING
    //rdpq_debug_start();
    profile_data.frame_count = 0;
    rspq_profile_start();
#endif

    display_set_fps_limit((display_get_refresh_rate() / 3) * 2); // @TODO

    // actors
    actors[S4YS] = actor_create(0, "rom:/sb_hhh/s4ys.t3dm");
    actors[WOLFIE] = actor_create(1, "rom:/sb_hhh/wolfie.t3dm");
    actors[MEW] = actor_create(2, "rom:/sb_hhh/mew.t3dm");
    actors[DOGMAN] = actor_create(3, "rom:/sb_hhh/dogman.t3dm");

    for (uint8_t i = 0; i < ACTOR_COUNT; i++) {
        actor_init(&actors[i]);
        actorCollider_init(&actor_collider[i]);
        actor_collider[i].settings.body_radius = 35.0f;
        actor_collider[i].settings.body_height = 190.f;

		actors[i].body.position.y = -800.0f;
        actors[i].body.rotation.x = 25.0f;
		actors[i].body.position.z = 450.0f;

        // Evenly space characters along x-axis
        float spacing = 100.0f;  // Distance between characters
        actors[i].body.position.x = -((ACTOR_COUNT - 1) * spacing) / 2 + i * spacing;

        actors[i].home = actors[i].body.position;

        player_init(&player[i], i, i);
    }

    // AI
    for (uint8_t i = 0; i < AI_COUNT; i++) {
        ai_init(&aiPlayer[i], core_get_aidifficulty());
    }
    
	// scenery
    scenery[0] = scenery_create(0, "rom:/sb_hhh/lava.t3dm");

    for (uint8_t i = 0; i < SCENERY_COUNT; i++)
	{
        scenery_set(&scenery[i]);
    }

    // platforms
    platform_hexagonGrid(hexagons, t3d_model_load("rom:/sb_hhh/platform.t3dm"), 250.0f, ui_color(N_YELLOW));

    // Sound: Play lava SFX
    sound_wavPlay(SFX_LAVA, true);

}

void minigame_fixedloop(float dt)
{

    game_play(&minigame, player, aiPlayer, actors, scenery, actor_collider, actor_contact);
    
}


void minigame_loop(float dt)
{
    minigame.timing.frame_time_s = dt;
}

void minigame_cleanup()
{

#ifdef PROFILING
    rspq_profile_stop();
#endif

    // Step 1: Disable Frame Limiter
    display_set_fps_limit(0);

    // Step 2: Clean up Subsystems
    sound_cleanup();
    ui_cleanup();

    // TPX
    ptx_cleanup(&lavaBubbles);

    // Step 3: Destroy Tiny3D models, matrices, animations and RSPQ blocks
	for (uint8_t i = 0; i < ACTOR_COUNT; i++) {

		actor_delete(&actors[i]);
	};

    for (uint8_t i = 0; i < SCENERY_COUNT; i++) {

		scenery_delete(&scenery[i]);
	}
    platform_destroy(hexagons);
    t3d_destroy(); // Then destroy library

    // Step 4: Free allocated surface buffers
    surface_free(&minigame.screen.depthBuffer);
    display_close();
}
