#include <pebble.h>

#define SETTINGS_KEY 1
#if PBL_DISPLAY_HEIGHT == 228 
  #define CHARACTER_HEIGHT 140
#elif PBL_DISPLAY_HEIGHT == 180 
  #define CHARACTER_HEIGHT 84
#else
  #define CHARACTER_HEIGHT 84
#endif


static Window* s_main_window;
static TextLayer* s_time_layer;
static TextLayer* s_date_layer;

static BitmapLayer* s_background_layer;
static BitmapLayer* s_bluetooth_layer;
static Layer *s_character_layer;

static GBitmap* s_background_bitmap;
static GBitmap* s_bluetooth_on_bitmap;
static GBitmap* s_bluetooth_off_bitmap;
static GBitmap* s_character_bitmap[3];

static GFont s_time_font;
static GFont s_date_font;

/* Define Settings */
typedef struct ClaySettings {
  bool VibrateOnDisconnect;
  int HourMode;
  int DateFormat;
  int Character1;
  int Character2;
  int Character3;
  int Character1Spoiler;
  int Character2Spoiler;
  int Character3Spoiler;
  bool SpoilerMode;
  int RandomInterval;
  bool RandomIntervalUnit;
} ClaySettings;
static ClaySettings settings;

/* DEFINE */