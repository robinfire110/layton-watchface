#include "layton-watchface.h"
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

static void update_time() 
{
  // Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  // Write the current hours and minutes into a buffer
  static char s_buffer[8];
  static char d_buffer[16];
  
  strftime(s_buffer, sizeof(s_buffer), ((settings.HourMode == 0 && clock_is_24h_style()) || settings.HourMode == 2) ? "%H:%M" : "%I:%M", tick_time);
  if (s_buffer[0] == '0' && (settings.HourMode == 1 || (settings.HourMode == 0 && !clock_is_24h_style())))
  {
    memmove(s_buffer, s_buffer+1, strlen(s_buffer));
  }

  //Date
  switch (settings.DateFormat)
  {
    case 0: strftime(d_buffer, sizeof(d_buffer), "%b %d, %G", tick_time); break;
    case 1: strftime(d_buffer, sizeof(d_buffer), "%d %b, %G", tick_time); break;
    case 2: strftime(d_buffer, sizeof(d_buffer), "%G, %b %d", tick_time); break;
    case 3: strftime(d_buffer, sizeof(d_buffer), "%D", tick_time); break;
    case 4: strftime(d_buffer, sizeof(d_buffer), "%d/%m/%y", tick_time); break;
    case 5: strftime(d_buffer, sizeof(d_buffer), "%y/%m/%d", tick_time); break;
    case 6: strftime(d_buffer, sizeof(d_buffer), " ", tick_time); break;
    case 7: strftime(d_buffer, sizeof(d_buffer), "%a, %b %d", tick_time); break;
  }
  
  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, s_buffer);
  text_layer_set_text(s_date_layer, d_buffer);
}

//Checks for repeats and repeat characters (so there won't be more than one Layton, Luke or Randall)
static bool check_repeat(int* array, int value)
{
  int layton_array[] = {0, 19, 20, 27};
  int luke_array[] = {1, 21, 24};
  int randall_array[] = {26, 29};
  for (int i = 0; i < 3; i++)
  {
    if (value == -1) { return false; }
    if (settings.Character1 == value || settings.Character2 == value || settings.Character3 == value) { return true; }
    if (array[i] == value) { return true; } 
    else
    {
      //Check Layton
      if (value == 0 || value == 19 || value == 20 || value == 27)
      {
        for (int j = 0; j < (int)(sizeof(layton_array)/sizeof(layton_array[0])); j++) 
        {
          if (layton_array[j] == array[i]) { return true; }
        }
      }
      else if (value == 1 || value == 21 || value == 24) //Check Luke
      {
        for (int j = 0; j < (int)(sizeof(luke_array)/sizeof(luke_array[0])); j++) 
        {
          if (luke_array[j] == array[i]) { return true; }
        }
      }
      else if (value == 26 || value == 29) //Check Randall
      {
        for (int j = 0; j < (int)(sizeof(randall_array)/sizeof(randall_array[0])); j++) 
        {
          if (randall_array[j] == array[i]) { return true; }
        }
      }
    }
  }
  return false;
}

static void set_characters()
{
  uint32_t characters[30] = {RESOURCE_ID_IMAGE_LAYTON_NORMAL,
                RESOURCE_ID_IMAGE_LUKE_NORMAL,
                RESOURCE_ID_IMAGE_FLORA,
                RESOURCE_ID_IMAGE_DAHLIA,
                RESOURCE_ID_IMAGE_MATTHEW,
                RESOURCE_ID_IMAGE_CHELMEY,
                RESOURCE_ID_IMAGE_BARTON,
                RESOURCE_ID_IMAGE_DON,
                RESOURCE_ID_IMAGE_STACHEN,
                RESOURCE_ID_IMAGE_GRANNY,
                RESOURCE_ID_IMAGE_SCHRADER,
                RESOURCE_ID_IMAGE_KATIA,
                RESOURCE_ID_IMAGE_ANTON,
                RESOURCE_ID_IMAGE_DIMITRI,
                RESOURCE_ID_IMAGE_BOSTRO,
                RESOURCE_ID_IMAGE_SUBJECT3,
                RESOURCE_ID_IMAGE_PUZZLETTE,
                RESOURCE_ID_IMAGE_CLIVE,
                RESOURCE_ID_IMAGE_CLAIRE,
                RESOURCE_ID_IMAGE_LAYTON_UF,
                RESOURCE_ID_IMAGE_LAYTON_HATLESS,
                RESOURCE_ID_IMAGE_LUKE_LS,
                RESOURCE_ID_IMAGE_EMMY,
                RESOURCE_ID_IMAGE_DESCOLE,
                RESOURCE_ID_IMAGE_LUKE_MM,
                RESOURCE_ID_IMAGE_MASKED,
                RESOURCE_ID_IMAGE_RANDALL_NORMAL,
                RESOURCE_ID_IMAGE_LAYTON_MM,
                RESOURCE_ID_IMAGE_LEON,
                RESOURCE_ID_IMAGE_RANDALL_UNMASKED};
  //Choice Arrays
  int normal_mode[] = {0, 1, 2, 3, 4, 5, 6, 8, 9, 10, 11, 13, 14, 15, 16, 17, 20, 21, 22, 24, 25, 26, 27};
  int spoiler_mode[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29};

  //Set Characters
  int repeat_array[] = {-1, -1, -1};
  for (int i = 0; i < 3; i++)
  {
    //Destroy Old
    if (s_character_bitmap[i])
    {
      gbitmap_destroy(s_character_bitmap[i]);
      s_character_bitmap[i] = NULL;
    } 

    //Get character
    int selected = -1;
    switch (i)
    {
      case 0: 
        if (settings.SpoilerMode) selected = settings.Character1Spoiler;
        else selected = settings.Character1;  
      break;

       case 1: 
        if (settings.SpoilerMode) selected = settings.Character2Spoiler;
        else selected = settings.Character2;  
      break;

       case 2: 
        if (settings.SpoilerMode) selected = settings.Character3Spoiler;
        else selected = settings.Character3;  
      break;
    }

    //Assign character
    if (selected != 100)
    {
      //Random number
      if (selected == -1)
      {
        
        //Check for repeats
        int rand_num;
        while (true)
        {
          //Get correct array
          if (settings.SpoilerMode)
          {
            rand_num = rand() % ((sizeof(spoiler_mode)/sizeof(spoiler_mode[0])-1));
            rand_num = spoiler_mode[rand_num];
          }
          else
          {
            rand_num = rand() % ((sizeof(normal_mode)/sizeof(normal_mode[0])-1));
            rand_num = normal_mode[rand_num];
          }

          if (!check_repeat(repeat_array, rand_num))
          {
            break;
          }
        }
        selected = rand_num;
      }
      repeat_array[i] = selected;

      //Set bitmap
      s_character_bitmap[i] = gbitmap_create_with_resource(characters[selected]);
    }
  }

  //Redraw Characters
  layer_mark_dirty(s_character_layer);
}

static void canvas_update_proc(Layer *layer, GContext *ctx) 
{ 
  //Draw
  GRect bounds_array[3];
  int image_width[] = {gbitmap_get_bounds(s_character_bitmap[0]).size.w, gbitmap_get_bounds(s_character_bitmap[1]).size.w, gbitmap_get_bounds(s_character_bitmap[2]).size.w};
  int padding = 4;
  int total_width = image_width[0] + image_width[1] + image_width[2] + (padding*4);
  int start = 8;
  int max_width;
  PBL_IF_RECT_ELSE(max_width = 130, max_width = 150);
  while (total_width >= max_width)
  {
    //Reduce
    if (padding > 1)
    {
      padding -= 1;
      if (padding <= 1) padding = 1;
    }
    else if (start > 0)
    {
      //No need to reduce start if round
      #if defined(PBL_ROUND)
        break;
      #endif

      //Recude Start
      start -= 1;
      if (start <= 0)
      {
        start = 0;
        padding = 0;
        break;
      } 
    }
    else break;

    //Set
    int add_start;
    PBL_IF_RECT_ELSE(add_start = (start-8), add_start = 0);
    total_width = image_width[0] + image_width[1] + image_width[2] + (padding*4) + add_start;
  }

  //Set
  #if defined(PBL_RECT)
    //Make sure image width is not 0 (if there is no one, make a slot)
    for (int i = 0; i < 3; i++)
    {
      if (image_width[i] == 0) image_width[i] = 30; //Size of Layton
    }

    //Set
    bounds_array[0] = GRect(start, CHARACTER_HEIGHT, image_width[0], 74);
    bounds_array[1] = GRect(start+image_width[0]+padding, CHARACTER_HEIGHT, image_width[1], 74);
    bounds_array[2] = GRect(start+image_width[0]+image_width[1]+padding*2, CHARACTER_HEIGHT, image_width[2], 74);
  #elif defined(PBL_ROUND)
    int r_start = (180-total_width)/2;
    printf("RoundStart: %d", r_start);
    bounds_array[0] = GRect(r_start+padding, CHARACTER_HEIGHT, image_width[0], 74);
    bounds_array[1] = GRect(r_start+image_width[0]+padding*2, CHARACTER_HEIGHT, image_width[1], 74);
    bounds_array[2] = GRect(r_start+image_width[0]+image_width[1]+padding*3, CHARACTER_HEIGHT, image_width[2], 74);
  #endif

  

  //Draw
  for (int i = 0; i < 3; i++)
  {
    //Draw
    graphics_context_set_compositing_mode(ctx, GCompOpSet);  
    graphics_draw_bitmap_in_rect(ctx, s_character_bitmap[i], bounds_array[i]);
  }
}

// Initialize the default settings
static void prv_default_settings() {
  settings.VibrateOnDisconnect = true;
  settings.HourMode = 0;
  settings.DateFormat = 0;
  settings.Character1 = 0;
  settings.Character2 = 1;
  settings.Character3 = 100;
  settings.Character1Spoiler = 0;
  settings.Character2Spoiler = 1;
  settings.Character3Spoiler = 100;
  settings.RandomInterval = 0;
  settings.RandomIntervalUnit = true;
}

// Read settings from persistent storage
static void prv_load_settings() {
  // Load the default settings
  prv_default_settings();

  // Read settings from persistent storage, if they exist
  persist_read_data(SETTINGS_KEY, &settings, sizeof(settings));

  //Update
  update_time();
  set_characters();
  printf("previoussettings");
}

// Save the settings to persistent storage
static void prv_save_settings() {
  persist_write_data(SETTINGS_KEY, &settings, sizeof(settings));
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) 
{
  /* GENERAL SETTINGS */
  //Vibrate
  Tuple *vibrate_t = dict_find(iterator, MESSAGE_KEY_VibrateOnDisconnect);
  if (vibrate_t)
  {
    settings.VibrateOnDisconnect = vibrate_t->value->uint32;
  }

  //Hour Mode
  Tuple *hour_t = dict_find(iterator, MESSAGE_KEY_HourMode);
  if (hour_t)
  {
    settings.HourMode = atoi(hour_t->value->cstring);
  }

  //Date Format
  Tuple *data_t = dict_find(iterator, MESSAGE_KEY_DateFormat);
  if (data_t)
  {
    settings.DateFormat = atoi(data_t->value->cstring);
  }

  /* NORMAL CHARACTERS */
  //Character1
  Tuple *character1_t = dict_find(iterator, MESSAGE_KEY_Character1);
  if (character1_t)
  {
    int c = atoi(character1_t->value->cstring);
    settings.Character1 = c;
  }

  //Character2
  Tuple *character2_t = dict_find(iterator, MESSAGE_KEY_Character2);
  if (character2_t)
  {
    int c = atoi(character2_t->value->cstring);
    settings.Character2 = c;
  }

  //Character3
  Tuple *character3_t = dict_find(iterator, MESSAGE_KEY_Character3);
  if (character3_t)
  {
    int c = atoi(character3_t->value->cstring);
    settings.Character3 = c;
  }

  /* SPOILER CHARACTERS */
  //Spoiler Character1
  Tuple *scharacter1_t = dict_find(iterator, MESSAGE_KEY_Character1Spoiler);
  if (scharacter1_t)
  {
    int c = atoi(scharacter1_t->value->cstring);
    settings.Character1Spoiler = c;
  }

  //Spoiler Character2
  Tuple *scharacter2_t = dict_find(iterator, MESSAGE_KEY_Character2Spoiler);
  if (scharacter2_t)
  {
    int c = atoi(scharacter2_t->value->cstring);
    settings.Character2Spoiler = c;
  }

  //Spoiler Character3
  Tuple *scharacter3_t = dict_find(iterator, MESSAGE_KEY_Character3Spoiler);
  if (scharacter3_t)
  {
    int c = atoi(scharacter3_t->value->cstring);
    settings.Character3Spoiler = c;
  }

  //Spoiler Mode
  Tuple *spoiler_mode_t = dict_find(iterator, MESSAGE_KEY_SpoilerMode);
  if (spoiler_mode_t)
  {
    settings.SpoilerMode = spoiler_mode_t->value->uint32;
  }

  /* Random */
  //RandomInterval
  Tuple *random_t = dict_find(iterator, MESSAGE_KEY_RandomInterval);
  if (random_t)
  {
    settings.RandomInterval = atoi(random_t->value->cstring);
    printf("RandomInterval: %d", settings.RandomInterval);
  }

  //Hour or Minutes
  Tuple *random_unit_t = dict_find(iterator, MESSAGE_KEY_RandomIntervalUnit);
  if (random_unit_t)
  {
    settings.RandomIntervalUnit = atoi(random_unit_t->value->cstring);
    printf("RandomHour?: %d", settings.RandomIntervalUnit);
  }

  //Set Characters
  update_time();
  set_characters();

  //Save Settings
  prv_save_settings();
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) 
{
  update_time();

  //Update character
  bool minute_check = !settings.RandomIntervalUnit && (tick_time->tm_min % settings.RandomInterval) == 0;
  bool hour_check = settings.RandomIntervalUnit && tick_time->tm_min == 0 && ((tick_time->tm_hour) % settings.RandomInterval) == 0;
  printf("MC: %d, HC: %d", minute_check, hour_check);
  if (settings.RandomInterval >= 1 && (minute_check || hour_check)) 
  {
    printf("tickhandler");
    set_characters();
  }  
}

static void bluetooth_callback(bool connected)
{
  //Set depending on status of bluetooth
  if (connected)
  {
    bitmap_layer_set_bitmap(s_bluetooth_layer, s_bluetooth_on_bitmap);
  }
  else
  {
    bitmap_layer_set_bitmap(s_bluetooth_layer, s_bluetooth_off_bitmap);
    //Vibrate on disconnected
    if (settings.VibrateOnDisconnect)
    {
      vibes_double_pulse();
    }
  }
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

static void main_window_load(Window *window) 
{
  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  /* Background */
  // Create GBitmap
  PBL_IF_RECT_ELSE(s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND), s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND_ROUND));

  // Create BitmapLayer to display the GBitmap
  s_background_layer = bitmap_layer_create(bounds);
  
  // Set the bitmap onto the layer and add to the window
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  bitmap_layer_set_compositing_mode(s_background_layer, GCompOpSet);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_background_layer));

  /* Bluetooth */
  s_bluetooth_on_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BLUETOOTH_ON);
  s_bluetooth_off_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BLUETOOTH_OFF);
  s_bluetooth_layer = bitmap_layer_create(GRect(PBL_IF_RECT_ELSE(54, 49), PBL_IF_RECT_ELSE(14, 20), bounds.size.w, 30));
  bitmap_layer_set_bitmap(s_bluetooth_layer, s_bluetooth_on_bitmap);
  bitmap_layer_set_compositing_mode(s_bluetooth_layer, GCompOpSet);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_bluetooth_layer));

  // Register for Bluetooth connection updates
  connection_service_subscribe((ConnectionHandlers) {
    .pebble_app_connection_handler = bluetooth_callback
  });

  /* Characters */
  s_character_layer = layer_create(bounds);
  layer_set_update_proc(s_character_layer, canvas_update_proc);
  layer_add_child(window_layer, s_character_layer);
  bool character_check = (!settings.SpoilerMode && (settings.Character1 == -1 || settings.Character2 == -1 || settings.Character3 == -1)) || (settings.SpoilerMode && (settings.Character1Spoiler == -1 || settings.Character2Spoiler == -1 || settings.Character3Spoiler == -1));
  if (settings.RandomInterval == 0 && character_check)
  {
    printf("reload");
    set_characters();
  }

  /* Time & Date */
  // Create the TextLayer with specific bounds (x, y)
  s_time_layer = text_layer_create(GRect(PBL_IF_RECT_ELSE(11, 1), PBL_IF_RECT_ELSE(14, 20), bounds.size.w, 100));
  s_date_layer = text_layer_create(GRect(PBL_IF_RECT_ELSE(11, 1), PBL_IF_RECT_ELSE(17, 23), bounds.size.w, 50));

  // Create GFont
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_LAYTON_48));
  text_layer_set_font(s_time_layer, s_time_font); 
  s_date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_LAYTON_16));
  text_layer_set_font(s_date_layer, s_date_font); 

  // Set Layer Properties
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_text(s_time_layer, "12:00");
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_text_color(s_date_layer, GColorWhite);
  text_layer_set_text(s_date_layer, "Jan 1, 2023");
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_date_layer));
}

static void main_window_unload(Window *window) 
{
  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_date_layer);

  // Unload GFont
  fonts_unload_custom_font(s_time_font);
  fonts_unload_custom_font(s_date_font);

  // Destroy GBitmap
  gbitmap_destroy(s_background_bitmap);
  gbitmap_destroy(s_bluetooth_on_bitmap);
  gbitmap_destroy(s_bluetooth_off_bitmap);
  for (int i = 0; i < 3; i++) {gbitmap_destroy(s_character_bitmap[i]);}

  // Destroy BitmapLayer
  bitmap_layer_destroy(s_background_layer);
  bitmap_layer_destroy(s_bluetooth_layer);
  layer_destroy(s_character_layer);
  
}

static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) 
  {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);

  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

  // Update bluetooth status
  bluetooth_callback(connection_service_peek_pebble_app_connection());

  // Register MessageKeys callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  app_message_open(128, 128);

  // Make sure the time is displayed from the start
  srand(time(NULL));
  //update_time(); Done in "prv_load_settings()"

  //Load Settings
  printf("init");
  prv_load_settings();
}

static void deinit() 
{
  // Destroy Window
  window_destroy(s_main_window);
}

int main(void) 
{
  init();
  app_event_loop();
  deinit();
}