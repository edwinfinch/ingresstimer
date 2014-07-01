#define NUM_MENU_SECTIONS 2
#define NUM_FIRST_MENU_ITEMS 4
#define NUM_SECOND_MENU_ITEMS 1

Window *main_window, *timer_window;
TextLayer *minute_5_t, *minute_21_t, *hour_4_t;

GBitmap *play, *pause, *restart, *resistance, *enlightened;
BitmapLayer *res_layer, *enl_layer;
GFont *coda, *coda_small;

ActionBarLayer *ab;

InverterLayer *theme;

char timer1_buffer[] = "00:00.";
char timer2_buffer[] = "00:00.";
char timer3_buffer[] = "00:00:00.";

typedef struct mytimer_struct {
    int16_t min;
    int16_t sec;
	int16_t hour;
} timer;

static timer min5_td = {
	.min = 5,
	.sec = 0,
	.hour = 0,
};

static timer min21_td = {
	.min = 21,
	.sec = 0,
	.hour = 0,
};

static timer hour4_td = {
	.min = 0,
	.sec = 0,
	.hour = 4,
};

int timerFired;
int animationSpeed = 500;
bool timer1_running, timer2_running, timer3_running;

static SimpleMenuLayer *main_menu;
static SimpleMenuSection menu_sections[NUM_MENU_SECTIONS];
static SimpleMenuItem first_menu_items[NUM_FIRST_MENU_ITEMS];
static SimpleMenuItem second_menu_items[NUM_SECOND_MENU_ITEMS];
