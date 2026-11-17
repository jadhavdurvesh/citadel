#include "test.h"
#include "citadel/sandos.h"
#include <string.h>

typedef struct Fake { char launched[24]; int settings[4]; uint32_t set_calls; char notes[4][CITADEL_SAND_NOTE_MAX + 1]; uint32_t nnotes; uint32_t rsec[2]; char rtext[2][CITADEL_SAND_NOTE_MAX + 1]; uint32_t nrem; } Fake;
static int f_note_add(void *c, const char *t) { Fake *f = (Fake *)c; if (f->nnotes >= 4u) return -1; strcpy(f->notes[f->nnotes++], t); return 0; }
static uint32_t f_note_count(void *c) { return ((Fake *)c)->nnotes; }
static int f_note_get(void *c, uint32_t i, char *o, size_t cap) { Fake *f = (Fake *)c; if (i >= f->nnotes || strlen(f->notes[i]) + 1 > cap) return -1; strcpy(o, f->notes[i]); return 0; }
static int f_notes_clear(void *c) { ((Fake *)c)->nnotes = 0; return 0; }
static int f_rem_add(void *c, uint32_t sec, const char *t) { Fake *f = (Fake *)c; if (f->nrem >= 2u) return -1; f->rsec[f->nrem] = sec; strcpy(f->rtext[f->nrem++], t); return 0; }
static uint32_t f_rem_count(void *c) { return ((Fake *)c)->nrem; }
static int f_rem_get(void *c, uint32_t i, uint32_t *sec, char *o, size_t cap) { Fake *f = (Fake *)c; if (i >= f->nrem || strlen(f->rtext[i]) + 1 > cap) return -1; *sec = f->rsec[i]; strcpy(o, f->rtext[i]); return 0; }
static int f_rem_clear(void *c) { ((Fake *)c)->nrem = 0; return 0; }
static const char *const APPS[3] = { "cairn", "locus", "keystone" };
static uint32_t f_count(void *c) { (void)c; return 3u; }
static int f_name(void *c, uint32_t i, char *out, size_t cap) { (void)c; if (i >= 3u || strlen(APPS[i]) + 1 > cap) return -1; strcpy(out, APPS[i]); return 0; }
static int f_launch(void *c, const char *app) { Fake *f = (Fake *)c; uint32_t i; for (i = 0; i < 3u; ++i) if (!strcmp(app, APPS[i])) { strcpy(f->launched, app); return 0; } return -1; }
static int f_get(void *c, uint32_t k) { Fake *f = (Fake *)c; return k >= 1u && k <= 3u ? f->settings[k] : -1; }
static int f_set(void *c, uint32_t k, uint32_t v) { Fake *f = (Fake *)c; if (k < 1u || k > 3u) return -1; f->settings[k] = (int)v; f->set_calls++; return 0; }
static int f_time(void *c, CitadelSandTime *t) { (void)c; t->year = 2026; t->month = 11; t->day = 16; t->hour = 9; t->minute = 5; t->second = 0; return 0; }
static int f_info(void *c, CitadelSandInfo *i) { (void)c; i->uptime_seconds = 3725; i->free_kib = 98304; i->total_kib = 131072; i->process_count = 4; return 0; }

static CitadelStatus ask(const CitadelRegistry *r, const char *text, uint32_t perms, int confirmed, CitadelResult *out) {
    CitadelCommand cmd; CitadelPolicy pol; pol.granted_permissions = perms; pol.confirmation_granted = (uint8_t)confirmed;
    if (citadel_parse(text, strlen(text), &cmd) != CITADEL_OK) return CITADEL_ERR_INVALID_ARGUMENT;
    return citadel_dispatch(r, &cmd, &pol, out);
}

int test_sandos(void) {
    Fake fake; CitadelSandPlatform plat; CitadelRegistry reg; CitadelResult res; CitadelCommand cmd;
    memset(&fake, 0, sizeof(fake)); fake.settings[1] = 0;
    memset(&plat, 0, sizeof(plat));
    plat.context = &fake; plat.app_count = f_count; plat.app_name = f_name; plat.launch = f_launch;
    plat.setting_get = f_get; plat.setting_set = f_set; plat.wall_time = f_time; plat.system_info = f_info;
    plat.note_add = f_note_add; plat.note_count = f_note_count; plat.note_get = f_note_get; plat.notes_clear = f_notes_clear;
    plat.reminder_add = f_rem_add; plat.reminder_count = f_rem_count; plat.reminder_get = f_rem_get; plat.reminders_clear = f_rem_clear;
    CHECK(citadel_registry_init(&reg) == CITADEL_OK);
    CHECK(citadel_sandos_register(&reg, &plat) == CITADEL_OK);
    CHECK(citadel_sandos_register(&reg, &plat) != CITADEL_OK);                     /* second registration collides */

    CHECK(ask(&reg, "apps", CITADEL_SAND_POLICY_PERMISSIONS, 0, &res) == CITADEL_OK);
    CHECK(strcmp(res.text, "Apps: cairn locus keystone") == 0);
    CHECK(ask(&reg, "open locus", CITADEL_SAND_POLICY_PERMISSIONS, 0, &res) == CITADEL_OK);
    CHECK(strcmp(fake.launched, "locus") == 0 && strcmp(res.text, "Opening locus.") == 0);
    CHECK(ask(&reg, "launch terminal", CITADEL_SAND_POLICY_PERMISSIONS, 0, &res) == CITADEL_OK);   /* alias */
    CHECK(strcmp(fake.launched, "cairn") == 0);
    CHECK(ask(&reg, "open nosuchapp", CITADEL_SAND_POLICY_PERMISSIONS, 0, &res) == CITADEL_ERR_NOT_FOUND);
    CHECK(strstr(res.text, "could not open") != NULL);
    CHECK(ask(&reg, "open locus", CITADEL_PERMISSION_READ_SYSTEM, 0, &res) == CITADEL_ERR_DENIED);  /* policy without LAUNCH_APP */

    CHECK(ask(&reg, "what time is it", CITADEL_SAND_POLICY_PERMISSIONS, 0, &res) == CITADEL_OK);
    CHECK(strstr(res.text, "09:05 on 16 November 2026") != NULL);
    CHECK(ask(&reg, "date", CITADEL_SAND_POLICY_PERMISSIONS, 0, &res) == CITADEL_OK);
    CHECK(strcmp(res.text, "16 November 2026") == 0);
    CHECK(ask(&reg, "uptime", CITADEL_SAND_POLICY_PERMISSIONS, 0, &res) == CITADEL_OK);
    CHECK(strcmp(res.text, "Up for 1h 02m 05s.") == 0);
    CHECK(ask(&reg, "status", CITADEL_SAND_POLICY_PERMISSIONS, 0, &res) == CITADEL_OK);
    CHECK(strstr(res.text, "96 of 128 MiB free") != NULL && strstr(res.text, "processes: 4") != NULL);

    /* settings: changing needs the permission AND the user's confirmation */
    CHECK(ask(&reg, "set accent jade", CITADEL_SAND_POLICY_PERMISSIONS, 0, &res) == CITADEL_ERR_DENIED);
    CHECK(fake.set_calls == 0);
    CHECK(citadel_parse("set accent jade", 15, &cmd) == CITADEL_OK && citadel_requires_confirmation(&reg, &cmd) == 1);
    CHECK(citadel_parse("open locus", 10, &cmd) == CITADEL_OK && citadel_requires_confirmation(&reg, &cmd) == 0);
    CHECK(ask(&reg, "set accent jade", CITADEL_PERMISSION_READ_SYSTEM, 1, &res) == CITADEL_ERR_DENIED);  /* confirmed but not permitted */
    CHECK(fake.set_calls == 0);
    CHECK(ask(&reg, "set accent jade", CITADEL_SAND_POLICY_PERMISSIONS, 1, &res) == CITADEL_OK);
    CHECK(fake.settings[1] == 3 && strcmp(res.text, "Accent is now jade.") == 0);
    CHECK(ask(&reg, "set motion on", CITADEL_SAND_POLICY_PERMISSIONS, 1, &res) == CITADEL_OK && fake.settings[2] == 1);
    CHECK(ask(&reg, "set clock 24", CITADEL_SAND_POLICY_PERMISSIONS, 1, &res) == CITADEL_OK && fake.settings[3] == 1);
    CHECK(ask(&reg, "set accent pink", CITADEL_SAND_POLICY_PERMISSIONS, 1, &res) == CITADEL_ERR_INVALID_ARGUMENT);
    CHECK(ask(&reg, "set volume 11", CITADEL_SAND_POLICY_PERMISSIONS, 1, &res) == CITADEL_ERR_INVALID_ARGUMENT);
    CHECK(ask(&reg, "set accent", CITADEL_SAND_POLICY_PERMISSIONS, 1, &res) == CITADEL_ERR_INVALID_ARGUMENT);
    CHECK(fake.settings[1] == 3 && fake.set_calls == 3);                          /* bad input changed nothing */
    CHECK(ask(&reg, "show settings", CITADEL_SAND_POLICY_PERMISSIONS, 0, &res) == CITADEL_OK);
    CHECK(strcmp(res.text, "Accent: jade | reduced motion: on | clock: 24-hour") == 0);

    /* notes */
    CHECK(ask(&reg, "notes", CITADEL_SAND_POLICY_PERMISSIONS, 0, &res) == CITADEL_OK && strstr(res.text, "no notes") != NULL);
    CHECK(ask(&reg, "note buy milk and eggs", CITADEL_SAND_POLICY_PERMISSIONS, 0, &res) == CITADEL_OK && strcmp(res.text, "Noted: buy milk and eggs") == 0);
    CHECK(ask(&reg, "note call mum", CITADEL_SAND_POLICY_PERMISSIONS, 0, &res) == CITADEL_OK);
    CHECK(ask(&reg, "show notes", CITADEL_SAND_POLICY_PERMISSIONS, 0, &res) == CITADEL_OK && strcmp(res.text, "1) buy milk and eggs | 2) call mum") == 0);
    CHECK(ask(&reg, "note aaaaaaaaaa bbbbbbbbbb cccccccccc dddddddddd eeeeeeeeee ffffffffff gggggggggg", CITADEL_SAND_POLICY_PERMISSIONS, 0, &res) == CITADEL_ERR_INVALID_ARGUMENT);
    CHECK(fake.nnotes == 2u);
    CHECK(ask(&reg, "note x", CITADEL_PERMISSION_READ_SYSTEM, 0, &res) == CITADEL_ERR_DENIED);   /* needs FILE_ACCESS */
    CHECK(ask(&reg, "clear notes", CITADEL_SAND_POLICY_PERMISSIONS, 0, &res) == CITADEL_ERR_DENIED && fake.nnotes == 2u);  /* needs confirmation */
    CHECK(ask(&reg, "note three", CITADEL_SAND_POLICY_PERMISSIONS, 0, &res) == CITADEL_OK);
    CHECK(ask(&reg, "note four", CITADEL_SAND_POLICY_PERMISSIONS, 0, &res) == CITADEL_OK);
    CHECK(ask(&reg, "note five", CITADEL_SAND_POLICY_PERMISSIONS, 0, &res) == CITADEL_ERR_LIMIT);  /* store full */
    CHECK(ask(&reg, "clear notes", CITADEL_SAND_POLICY_PERMISSIONS, 1, &res) == CITADEL_OK && fake.nnotes == 0u);

    /* reminders */
    CHECK(ask(&reg, "reminders", CITADEL_SAND_POLICY_PERMISSIONS, 0, &res) == CITADEL_OK && strstr(res.text, "No reminders") != NULL);
    CHECK(ask(&reg, "remind me in 5 minutes to stretch", CITADEL_SAND_POLICY_PERMISSIONS, 0, &res) == CITADEL_OK);
    CHECK(fake.rsec[0] == 300u && strcmp(fake.rtext[0], "stretch") == 0 && strcmp(res.text, "Okay, I will remind you in 5 minutes: stretch") == 0);
    CHECK(ask(&reg, "remind me in 1 hour", CITADEL_SAND_POLICY_PERMISSIONS, 0, &res) == CITADEL_OK && fake.rsec[1] == 3600u);
    CHECK(strcmp(res.text, "Okay, I will remind you in 1 hour.") == 0);
    CHECK(ask(&reg, "reminders", CITADEL_SAND_POLICY_PERMISSIONS, 0, &res) == CITADEL_OK && strcmp(res.text, "5m 0s: stretch | 1h 0m: (timer)") == 0);
    CHECK(ask(&reg, "remind me in 1 second", CITADEL_SAND_POLICY_PERMISSIONS, 0, &res) == CITADEL_ERR_LIMIT);  /* none free */
    CHECK(ask(&reg, "remind me in 25 hours", CITADEL_SAND_POLICY_PERMISSIONS, 0, &res) == CITADEL_ERR_INVALID_ARGUMENT);
    CHECK(ask(&reg, "remind me in 0 minutes", CITADEL_SAND_POLICY_PERMISSIONS, 0, &res) == CITADEL_ERR_INVALID_ARGUMENT);
    CHECK(ask(&reg, "remind me in five minutes", CITADEL_SAND_POLICY_PERMISSIONS, 0, &res) == CITADEL_ERR_INVALID_ARGUMENT);
    CHECK(ask(&reg, "remind me in 5 fortnights", CITADEL_SAND_POLICY_PERMISSIONS, 0, &res) == CITADEL_ERR_INVALID_ARGUMENT);
    CHECK(ask(&reg, "remind me in 5 minutes to", CITADEL_SAND_POLICY_PERMISSIONS, 0, &res) == CITADEL_ERR_INVALID_ARGUMENT);
    CHECK(ask(&reg, "remind me later", CITADEL_SAND_POLICY_PERMISSIONS, 0, &res) == CITADEL_ERR_INVALID_ARGUMENT);
    CHECK(ask(&reg, "remind me in 5 minutes", CITADEL_PERMISSION_READ_SYSTEM, 0, &res) == CITADEL_ERR_DENIED);  /* needs SCHEDULE */
    CHECK(ask(&reg, "clear reminders", CITADEL_SAND_POLICY_PERMISSIONS, 0, &res) == CITADEL_OK && fake.nrem == 0u);

    /* missing platform hooks degrade to errors, not crashes */
    plat.note_add = NULL; plat.reminder_add = NULL;
    CHECK(ask(&reg, "note hi", CITADEL_SAND_POLICY_PERMISSIONS, 0, &res) == CITADEL_ERR_UNAVAILABLE);
    CHECK(ask(&reg, "remind me in 5 minutes", CITADEL_SAND_POLICY_PERMISSIONS, 0, &res) == CITADEL_ERR_UNAVAILABLE);
    plat.wall_time = NULL;
    CHECK(ask(&reg, "time", CITADEL_SAND_POLICY_PERMISSIONS, 0, &res) == CITADEL_ERR_UNAVAILABLE);
    CHECK(ask(&reg, "help", CITADEL_PERMISSION_NONE, 0, &res) == CITADEL_OK && strstr(res.text, "open <app>") != NULL);
    CHECK(ask(&reg, "tell me a joke", CITADEL_SAND_POLICY_PERMISSIONS, 0, &res) == CITADEL_ERR_NOT_FOUND);
    PASS();
}
