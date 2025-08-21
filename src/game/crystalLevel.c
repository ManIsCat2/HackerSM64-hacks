#include <PR/ultratypes.h>

#include "sm64.h"
#include "area.h"
#include "audio/external.h"
#include "behavior_actions.h"
#include "behavior_data.h"
#include "camera.h"
#include "course_table.h"
#include "dialog_ids.h"
#include "engine/behavior_script.h"
#include "engine/math_util.h"
#include "engine/surface_collision.h"
#include "envfx_bubbles.h"
#include "game_init.h"
#include "ingame_menu.h"
#include "interaction.h"
#include "level_misc_macros.h"
#include "level_table.h"
#include "level_update.h"
#include "mario.h"
#include "mario_actions_cutscene.h"
#include "mario_misc.h"
#include "memory.h"
#include "obj_behaviors.h"
#include "object_helpers.h"
#include "object_list_processor.h"
#include "rendering_graph_node.h"
#include "save_file.h"
#include "spawn_object.h"
#include "spawn_sound.h"
#include "rumble_init.h"
#include "print.h"

#define m gMarioState

u8 starCollected(u8 id) {
    u32 starflags = save_file_get_star_flags(gCurrSaveFileNum - 1, gCurrCourseNum - 1);

    if (starflags & (1 << id)) {
        return TRUE;
    }
    return FALSE;
}

void star_crystal(void) {
    if (!o->oHiddenBlueCoinSwitch) {
        o->oHealth = 0;
        o->oHiddenBlueCoinSwitch = spawn_object_relative(0, 0, 60, 0, o, starCollected(0) ? MODEL_TRANSPARENT_STAR : MODEL_STAR, bhvStaticObject);
        o->oHiddenBlueCoinSwitch->oFaceAngleYaw -= 16384;
    }

    if (cur_obj_is_mario_ground_pounding_platform()) {
        o->oHealth += 1;
    }

    if (o->oHealth >= (7*7)) {
        spawn_triangle_break_particles(20, MODEL_DIRT_ANIMATION, 3.0f, 4);
        obj_mark_for_deletion(o->oHiddenBlueCoinSwitch);
        obj_mark_for_deletion(o);
        spawn_default_star(o->oPosX, o->oPosY, o->oPosZ);
    }
   // print_text_fmt_int(20, 20, "P %d", o->oHealth);
}

void goombacrystal_npc(void) {
    cur_obj_scale(2.5f);
    o->oInteractionSubtype = INT_SUBTYPE_NPC;
    o->hitboxHeight = 80;
    o->hitboxRadius = 90;
    o->oIntangibleTimer = 0;

    o->oInteractType = INTERACT_TEXT;

    struct Object*crystalobj = cur_obj_nearest_object_with_behavior(bhvCrystalStar);

    u8 dialog = crystalobj ? 0 : 1;

    if (o->oInteractStatus & INT_STATUS_INTERACTED) {
        if (cur_obj_update_dialog_with_cutscene(MARIO_DIALOG_LOOK_FRONT, DIALOG_FLAG_TURN_TO_MARIO, CUTSCENE_DIALOG, dialog)) {
            o->oInteractStatus = 0;
        }
    }
}