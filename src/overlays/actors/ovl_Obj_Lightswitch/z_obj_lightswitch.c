/*
 * File: z_obj_lightswitch.c
 * Overlay: ovl_Obj_Lightswitch
 * Description: Sun Switch / STT Flip switch
 */

#include "z_obj_lightswitch.h"
#include "objects/object_lightswitch/object_lightswitch.h"

#define FLAGS (ACTOR_FLAG_10)

#define THIS ((ObjLightswitch*)thisx)

void ObjLightswitch_Init(Actor* thisx, GlobalContext* globalCtx);
void ObjLightswitch_Destroy(Actor* thisx, GlobalContext* globalCtx);
void ObjLightswitch_Update(Actor* thisx, GlobalContext* globalCtx);
void ObjLightswitch_Draw(Actor* thisx, GlobalContext* globalCtx);
void ObjLightswitch_PlayCinema(ObjLightswitch* this, GlobalContext* globalCtx);
void ObjLightSwitch_SetupEnabled(ObjLightswitch* this);
void ObjLightSwitch_Enabled(ObjLightswitch* this, GlobalContext* globalCtx);
void ObjLightswitch_SetupIdle(ObjLightswitch* this);
void ObjLightSwitch_SetupFade(ObjLightswitch* this);
void ObjLightSwitch_Fade(ObjLightswitch* this, GlobalContext* globalCtx);
void ObjLightSwitch_SetupAsleep(ObjLightswitch* this);
void ObjLightSwitch_Asleep(ObjLightswitch* this, GlobalContext* globalCtx);
void ObjLightSwitch_SetupDisabled(ObjLightswitch* this);
void ObjLightSwitch_Disabled(ObjLightswitch* this, GlobalContext* globalCtx);
void ObjLightswitch_Idle(ObjLightswitch* this, GlobalContext* globalCtx);

const ActorInit Obj_Lightswitch_InitVars = {
    ACTOR_OBJ_LIGHTSWITCH,
    ACTORCAT_SWITCH,
    FLAGS,
    OBJECT_LIGHTSWITCH,
    sizeof(ObjLightswitch),
    (ActorFunc)ObjLightswitch_Init,
    (ActorFunc)ObjLightswitch_Destroy,
    (ActorFunc)ObjLightswitch_Update,
    (ActorFunc)ObjLightswitch_Draw,
};

static ColliderJntSphElementInit sJntSphElementsInit[1] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00202000, 0x00, 0x00 },
            TOUCH_NONE | TOUCH_SFX_NORMAL,
            BUMP_ON,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 19 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER | AC_TYPE_OTHER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_JNTSPH,
    },
    1,
    sJntSphElementsInit,
};

// different face addresses for (sleep -> waking -> awake) of light switch face
static TexturePtr sLightswitchFaceGfx[] = {
    object_lightswitch_Tex_000C20,
    object_lightswitch_Tex_000420,
    object_lightswitch_Tex_001420,
};

static Color_RGBA8 sLightswitchEffectPrimColor = { 255, 255, 160, 160 };
static Color_RGBA8 sLightswitchEffectEnvColor = { 255, 0, 0, 0 };

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 200, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 200, ICHAIN_STOP),
};

void ObjLightswitch_InitCollider(ObjLightswitch* this, GlobalContext* globalCtx) {
    s32 pad;

    Collider_InitJntSph(globalCtx, &this->collider);
    Collider_SetJntSph(globalCtx, &this->collider, &this->actor, &sJntSphInit, &this->elements);
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    Matrix_SetStateRotationAndTranslation(this->actor.world.pos.x,
                                          this->actor.world.pos.y + (this->actor.shape.yOffset * this->actor.scale.y),
                                          this->actor.world.pos.z, &this->actor.shape.rot);
    Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
    Collider_UpdateSpheres(0, &this->collider);
}

void ObjLightswitch_UpdateSwitchFlags(ObjLightswitch* this, GlobalContext* globalCtx, s32 set) {
    if (this) {}

    if (set) {
        Flags_SetSwitch(globalCtx, GET_LIGHTSWITCH_SWITCHFLAG(this));
    } else {
        Flags_UnsetSwitch(globalCtx, GET_LIGHTSWITCH_SWITCHFLAG(this));
    }
}

void ObjLightswitch_SpawnEffects(ObjLightswitch* this, GlobalContext* globalCtx) {
    Vec3f effectPos;
    f32 sinResult;
    f32 cosResult;
    f32 tempResult;
    f32 tempResultDiff;
    f32 rand;
    s32 alpha;

    sinResult = Math_SinS(this->actor.shape.rot.y);
    cosResult = Math_CosS(this->actor.shape.rot.y);
    alpha = this->colorAlpha;

    if (alpha >= (100 << 6)) {
        tempResult = (1.0f - (alpha * (1.0f / (255 << 6)))) * 400.0f;
        tempResult = ((CLAMP_MAX(tempResult, 60.0f) - 30.0f) + 30.0f) * Rand_ZeroOne();
        tempResultDiff = tempResult - 30.0f;

        if (tempResult > 30.0f) {
            tempResult = 30.0f;
        } else {
            tempResult = SQ(30.0f) - SQ(tempResultDiff);
            tempResult = CLAMP_MIN(tempResult, SQ(10.0f));
            tempResult = sqrtf(tempResult);
        }

        tempResult = 2.0f * ((Rand_ZeroOne() - 0.5f) * tempResult);
        rand = ((30.0f - fabsf(tempResult)) * 0.5f) + (Rand_ZeroOne() * 10.0f);

        effectPos.x = this->actor.world.pos.x + ((rand * sinResult) + (tempResult * cosResult));
        effectPos.y = this->actor.world.pos.y + tempResultDiff + 10.0f;
        effectPos.z = this->actor.world.pos.z + ((rand * cosResult) - (tempResult * sinResult));

        EffectSsDeadDb_Spawn(globalCtx, &effectPos, &gZeroVec3f, &gZeroVec3f, &sLightswitchEffectPrimColor,
                             &sLightswitchEffectEnvColor, 100, 0, 9);
    }
}

void ObjLightswitch_Init(Actor* thisx, GlobalContext* globalCtx) {
    ObjLightswitch* this = THIS;
    s32 pad;
    u32 isSwitchActivated;
    s32 isTriggered;

    isSwitchActivated = Flags_GetSwitch(globalCtx, GET_LIGHTSWITCH_SWITCHFLAG(this));
    isTriggered = false;
    Actor_ProcessInitChain(&this->actor, sInitChain);
    Actor_SetFocus(&this->actor, 0.0f);

    if (isSwitchActivated) {
        if (GET_LIGHTSWITCH_TYPE(this) == LIGHTSWITCH_TYPE_FAKE) {
            isTriggered = true;
        } else {
            ObjLightSwitch_SetupEnabled(this);
        }
    } else {
        ObjLightswitch_SetupIdle(this);
    }

    ObjLightswitch_InitCollider(this, globalCtx);

    if (GET_LIGHTSWITCH_INVISIBLE(this)) {
        // the stone tower exterior switch is part of the scene mesh, the actor is invisble on top
        this->actor.draw = NULL;
    }

    if (isTriggered) {
        Actor_MarkForDeath(&this->actor);
    }
}

void ObjLightswitch_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    ObjLightswitch* this = THIS;
    Collider_DestroyJntSph(globalCtx, &this->collider);
}

void ObjLightswitch_SetAction(ObjLightswitch* this, ObjLightswitchSetupFunc setupFunc, u32 setState) {
    this->setupFunc = setupFunc;
    this->switchFlagSetType = setState;
    this->actionFunc = ObjLightswitch_PlayCinema;
}

void ObjLightswitch_PlayCinema(ObjLightswitch* this, GlobalContext* globalCtx) {
    if (ActorCutscene_GetCanPlayNext(this->actor.cutscene)) {
        ActorCutscene_StartAndSetUnkLinkFields(this->actor.cutscene, &this->actor);
        ObjLightswitch_UpdateSwitchFlags(this, globalCtx, this->switchFlagSetType);
        this->cutsceneTimer = 50;
        this->setupFunc(this);
    } else {
        ActorCutscene_SetIntentToPlay(this->actor.cutscene);
    }
}

void ObjLightswitch_SetupIdle(ObjLightswitch* this) {
    this->faceState = LIGHTSWITCH_FACE_ASLEEP;
    this->colorR = 155 << 6;
    this->colorG = 125 << 6;
    this->colorB = 255 << 6;
    this->colorAlpha = 255 << 6;
    this->actionFunc = ObjLightswitch_Idle;
}

void ObjLightswitch_Idle(ObjLightswitch* this, GlobalContext* globalCtx) {
    s32 actorType = GET_LIGHTSWITCH_TYPE(this);

    if (this->hitState >= 10) {
        if (actorType == LIGHTSWITCH_TYPE_FAKE) {
            ObjLightswitch_SetAction(this, ObjLightSwitch_SetupFade, 1);
        } else {
            if (actorType == LIGHTSWITCH_TYPE_FLIP) {
                this->hitState = 0;
            }
            ObjLightswitch_SetAction(this, ObjLightSwitch_SetupAsleep, 1);
        }
    }
}

void ObjLightSwitch_SetupAsleep(ObjLightswitch* this) {
    this->faceState = LIGHTSWITCH_FACE_ASLEEP;
    this->colorShiftTimer = 0;
    this->actionFunc = ObjLightSwitch_Asleep;
}

void ObjLightSwitch_Asleep(ObjLightswitch* this, GlobalContext* globalCtx) {
    if (this->colorShiftTimer == 0) {
        Actor_PlaySfxAtPos(&this->actor, NA_SE_EV_SUN_MARK_FLASH);
    }
    this->colorShiftTimer++;

    Math_StepToS(&this->edgeRotSpeed, -0xAA, 0xA);
    this->edgeRot += this->edgeRotSpeed;
    this->colorR = (this->colorShiftTimer * (5 << 6)) + (155 << 6);
    this->colorG = (this->colorShiftTimer * ((6 << 6) + 32)) + (125 << 6);

    if (this->colorShiftTimer >= 20) {
        ObjLightSwitch_SetupEnabled(this);
    } else if (this->colorShiftTimer == 15) {
        this->faceState = LIGHTSWITCH_FACE_WAKING;
        Actor_PlaySfxAtPos(&this->actor, NA_SE_EV_FOOT_SWITCH);
    }
}

void ObjLightSwitch_SetupEnabled(ObjLightswitch* this) {
    this->faceState = LIGHTSWITCH_FACE_FULLAWAKE;
    this->colorR = 255 << 6;
    this->colorG = 255 << 6;
    this->colorB = 255 << 6;
    this->colorAlpha = 255 << 6;
    this->edgeRotSpeed = -0xAA;
    this->colorShiftTimer = 0;
    this->actionFunc = ObjLightSwitch_Enabled;
}

void ObjLightSwitch_Enabled(ObjLightswitch* this, GlobalContext* globalCtx) {
    s32 actorType = GET_LIGHTSWITCH_TYPE(this);

    if (actorType == LIGHTSWITCH_TYPE_REGULAR) {
        // switch can be disabled outside of this actor by flag
        if (!Flags_GetSwitch(globalCtx, GET_LIGHTSWITCH_SWITCHFLAG(this))) {
            ObjLightSwitch_SetupDisabled(this);
        }
    } else if (actorType == LIGHTSWITCH_TYPE_FLIP) {
        if (this->hitState >= 10) {
            this->hitState = 0;
            ObjLightswitch_SetAction(this, ObjLightSwitch_SetupDisabled, 0);
        }
    } else if (this->hitState == 0) {
        if (this->colorShiftTimer >= 13) {
            ObjLightswitch_UpdateSwitchFlags(this, globalCtx, 0);
            ObjLightSwitch_SetupDisabled(this);
        } else {
            this->colorShiftTimer++;
        }
    } else {
        this->colorShiftTimer = 0;
    }
    this->edgeRot += this->edgeRotSpeed;
}

void ObjLightSwitch_SetupDisabled(ObjLightswitch* this) {
    this->colorShiftTimer = 20;
    this->faceState = LIGHTSWITCH_FACE_WAKING;
    this->actionFunc = ObjLightSwitch_Disabled;
}

void ObjLightSwitch_Disabled(ObjLightswitch* this, GlobalContext* globalCtx) {
    this->colorShiftTimer--;
    Math_StepToS(&this->edgeRotSpeed, 0, 0xA);
    this->edgeRot += this->edgeRotSpeed;
    this->colorR = (this->colorShiftTimer * (5 << 6)) + (155 << 6);
    this->colorG = (this->colorShiftTimer * ((6 << 6) + 32)) + (125 << 6);

    if (this->colorShiftTimer <= 0) {
        ObjLightswitch_SetupIdle(this);
    } else if (this->colorShiftTimer == 15) {
        this->faceState = LIGHTSWITCH_FACE_ASLEEP;
        Actor_PlaySfxAtPos(&this->actor, NA_SE_EV_FOOT_SWITCH);
    }
}

void ObjLightSwitch_SetupFade(ObjLightswitch* this) {
    this->colorAlpha = 255 << 6;
    this->actionFunc = ObjLightSwitch_Fade;
}

void ObjLightSwitch_Fade(ObjLightswitch* this, GlobalContext* globalCtx) {
    this->colorAlpha -= 200;
    ObjLightswitch_SpawnEffects(this, globalCtx); // spawn burning fire effect
    if (this->colorAlpha < 0) {
        Actor_MarkForDeath(&this->actor);
    } else {
        func_800B9010(&this->actor, NA_SE_EN_COMMON_EXTINCT_LEV - SFX_FLAG); // "burn into ashes"
    }
}

void ObjLightswitch_Update(Actor* thisx, GlobalContext* globalCtx) {
    ObjLightswitch* this = THIS;
    s32 pad;

    if (this->collider.base.acFlags & AC_HIT) {
        // dmgFlags enum doesn't exist yet, 0x2000 is light arrows
        if ((this->collider.elements->info.acHitInfo->toucher.dmgFlags & 0x2000) != 0) {
            this->hitState = 10;
        } else if (GET_LIGHTSWITCH_TYPE(this) == LIGHTSWITCH_TYPE_FLIP) {
            if (this->hitState == 0) {
                if ((this->previousACFlags & AC_HIT) == 0) {
                    this->hitState = 1;
                }
            } else if (this->hitState < 10) {
                this->hitState++;
            }
        } else {
            if (this->hitState < 10) {
                this->hitState++;
            }
        }
    } else {
        this->hitState = 0;
    }

    // why wouldn't this be in the action func?
    if ((this->cutsceneTimer > 0) && ((s32)this->actionFunc != (s32)ObjLightswitch_PlayCinema)) {
        this->cutsceneTimer--;
        if (this->cutsceneTimer == 0) {
            ActorCutscene_Stop(this->actor.cutscene);
        }
    }

    this->actionFunc(this, globalCtx);
    if (this->actor.update != NULL) {
        this->previousACFlags = this->collider.base.acFlags;
        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &this->collider.base);
        CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &this->collider.base);
    }
}

void ObjLightSwitch_DrawOpa(ObjLightswitch* this, GlobalContext* globalCtx) {
    s32 pad;
    Vec3f tempPos;
    Vec3s tempRot;

    OPEN_DISPS(globalCtx->state.gfxCtx);

    func_8012C28C(globalCtx->state.gfxCtx);
    gDPSetEnvColor(POLY_OPA_DISP++, (u8)(this->colorR >> 6), (u8)(this->colorG >> 6),
                   ((void)0, (u8)(this->colorB >> 6)), (u8)(this->colorAlpha >> 6));
    gSPSegment(POLY_OPA_DISP++, 0x09, D_801AEFA0);

    tempPos.x = this->actor.world.pos.x;
    tempPos.y = this->actor.world.pos.y + (this->actor.shape.yOffset * this->actor.scale.y);
    tempPos.z = this->actor.world.pos.z;
    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(sLightswitchFaceGfx[this->faceState]));
    gSPDisplayList(POLY_OPA_DISP++, object_lightswitch_DL_000260);

    tempRot.x = this->actor.shape.rot.x;
    tempRot.y = this->actor.shape.rot.y;
    tempRot.z = this->actor.shape.rot.z + this->edgeRot;
    Matrix_SetStateRotationAndTranslation(tempPos.x, tempPos.y, tempPos.z, &tempRot);
    Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, object_lightswitch_DL_000398);

    tempRot.z = this->actor.shape.rot.z - this->edgeRot;
    Matrix_SetStateRotationAndTranslation(tempPos.x, tempPos.y, tempPos.z, &tempRot);
    Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, object_lightswitch_DL_000408);

    CLOSE_DISPS(globalCtx->state.gfxCtx);
}

void ObjLightSwitch_DrawXlu(ObjLightswitch* this, GlobalContext* globalCtx) {
    s32 pad;
    Vec3f tempPos;
    Vec3s tempRot;

    OPEN_DISPS(globalCtx->state.gfxCtx);

    func_8012C2DC(globalCtx->state.gfxCtx);
    gDPSetEnvColor(POLY_XLU_DISP++, (u8)(this->colorR >> 6), (u8)(this->colorG >> 6),
                   ((void)0, (u8)(this->colorB >> 6)), (u8)(this->colorAlpha >> 6));
    gSPSegment(POLY_XLU_DISP++, 0x09, D_801AEF88);

    tempPos.x = this->actor.world.pos.x;
    tempPos.y = this->actor.world.pos.y + (this->actor.shape.yOffset * this->actor.scale.y);
    tempPos.z = this->actor.world.pos.z;
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPSegment(POLY_XLU_DISP++, 0x08, Lib_SegmentedToVirtual(sLightswitchFaceGfx[this->faceState]));
    gSPDisplayList(POLY_XLU_DISP++, object_lightswitch_DL_000260);

    tempRot.x = this->actor.shape.rot.x;
    tempRot.y = this->actor.shape.rot.y;
    tempRot.z = this->actor.shape.rot.z + this->edgeRot;
    Matrix_SetStateRotationAndTranslation(tempPos.x, tempPos.y, tempPos.z, &tempRot);
    Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, object_lightswitch_DL_000398);

    tempRot.z = this->actor.shape.rot.z - this->edgeRot;
    Matrix_SetStateRotationAndTranslation(tempPos.x, tempPos.y, tempPos.z, &tempRot);
    Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, object_lightswitch_DL_000408);

    CLOSE_DISPS(globalCtx->state.gfxCtx);
}

void ObjLightswitch_Draw(Actor* thisx, GlobalContext* globalCtx) {
    ObjLightswitch* this = THIS;
    s32 alpha = (u8)(this->colorAlpha >> 6);

    if ((GET_LIGHTSWITCH_TYPE(this) == LIGHTSWITCH_TYPE_FAKE) && (alpha > 0) && (alpha < 255)) {
        ObjLightSwitch_DrawXlu(this, globalCtx);
    } else {
        ObjLightSwitch_DrawOpa(this, globalCtx);
    }
}
