/*
 * File: z_en_prz.c
 * Overlay: ovl_En_Prz
 * Description: Skullfish - Defeated
 */

#include "z_en_prz.h"

#define FLAGS (ACTOR_FLAG_1 | ACTOR_FLAG_4 | ACTOR_FLAG_10)

#define THIS ((EnPrz*)thisx)

void EnPrz_Init(Actor* thisx, GlobalContext* globalCtx);
void EnPrz_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnPrz_Update(Actor* thisx, GlobalContext* globalCtx);
void EnPrz_Draw(Actor* thisx, GlobalContext* globalCtx);

#if 0
// static DamageTable sDamageTable = {
static DamageTable D_80A771C0 = {
    /* Deku Nut       */ DMG_ENTRY(0, 0x0),
    /* Deku Stick     */ DMG_ENTRY(0, 0x0),
    /* Horse trample  */ DMG_ENTRY(0, 0x0),
    /* Explosives     */ DMG_ENTRY(1, 0xF),
    /* Zora boomerang */ DMG_ENTRY(1, 0xF),
    /* Normal arrow   */ DMG_ENTRY(1, 0xF),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, 0x0),
    /* Hookshot       */ DMG_ENTRY(1, 0xF),
    /* Goron punch    */ DMG_ENTRY(0, 0x0),
    /* Sword          */ DMG_ENTRY(0, 0x0),
    /* Goron pound    */ DMG_ENTRY(0, 0x0),
    /* Fire arrow     */ DMG_ENTRY(1, 0xF),
    /* Ice arrow      */ DMG_ENTRY(1, 0xF),
    /* Light arrow    */ DMG_ENTRY(2, 0xF),
    /* Goron spikes   */ DMG_ENTRY(0, 0x0),
    /* Deku spin      */ DMG_ENTRY(0, 0x0),
    /* Deku bubble    */ DMG_ENTRY(1, 0xF),
    /* Deku launch    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, 0x0),
    /* Zora barrier   */ DMG_ENTRY(1, 0xF),
    /* Normal shield  */ DMG_ENTRY(0, 0x0),
    /* Light ray      */ DMG_ENTRY(0, 0x0),
    /* Thrown object  */ DMG_ENTRY(0, 0x0),
    /* Zora punch     */ DMG_ENTRY(1, 0xF),
    /* Spin attack    */ DMG_ENTRY(0, 0x0),
    /* Sword beam     */ DMG_ENTRY(0, 0x0),
    /* Normal Roll    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, 0x0),
    /* Unblockable    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, 0x0),
    /* Powder Keg     */ DMG_ENTRY(1, 0xF),
};

// static ColliderCylinderInit sCylinderInit = {
static ColliderCylinderInit D_80A771F4 = {
    { COLTYPE_NONE, AT_ON | AT_TYPE_ENEMY, AC_ON | AC_TYPE_PLAYER, OC1_NONE, OC2_TYPE_1, COLSHAPE_CYLINDER, },
    { ELEMTYPE_UNK4, { 0x20000000, 0x00, 0x04 }, { 0xF7CFFFFF, 0x00, 0x00 }, TOUCH_ON | TOUCH_SFX_NORMAL, BUMP_ON, OCELEM_NONE, },
    { 10, 10, 0, { 0, 0, 0 } },
};

const ActorInit En_Prz_InitVars = {
    ACTOR_EN_PRZ,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_PR,
    sizeof(EnPrz),
    (ActorFunc)EnPrz_Init,
    (ActorFunc)EnPrz_Destroy,
    (ActorFunc)EnPrz_Update,
    (ActorFunc)EnPrz_Draw,
};

#endif

extern DamageTable D_80A771C0;
extern ColliderCylinderInit D_80A771F4;

extern UNK_TYPE D_06004340;

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Prz/EnPrz_Init.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Prz/EnPrz_Destroy.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Prz/func_80A75F18.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Prz/func_80A75FA4.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Prz/func_80A76070.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Prz/func_80A762C0.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Prz/func_80A76388.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Prz/func_80A763E8.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Prz/func_80A76604.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Prz/func_80A76634.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Prz/func_80A76748.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Prz/func_80A767A8.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Prz/func_80A76A1C.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Prz/func_80A76B14.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Prz/EnPrz_Update.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Prz/func_80A76F70.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Prz/func_80A76FCC.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Prz/EnPrz_Draw.s")
