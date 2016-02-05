#include <pebble.h>
#include "main.h"
#include "stops.h"
#include "favorites.h"
#include "appmessage.h"


static Departure deps[5];
static char * cur_stop;
static Window* window;
static MenuLayer *menu_layer;
int num_departures = 5;
static int select_allowed = 1;


void stops_draw_row_callback(GContext *ctx, Layer *cell_layer, MenuIndex *cell_index, void *callback_context)
{
  menu_cell_basic_draw(ctx, cell_layer, deps[cell_index->row].headsign, deps[cell_index->row].expected_time, NULL);
}

static void stops_menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  menu_cell_basic_header_draw(ctx, cell_layer, "                CUMTD");
}

uint16_t stops_num_sections_callback(struct MenuLayer *menu_layer, void *callback_context) {
  return 1;
}

int16_t stops_header_height_callback(struct MenuLayer *menu_layer, uint16_t section_index, void *callback_context) {
  return 16;
}

uint16_t stops_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *callback_context)
{
  return num_departures;
}
 
void stops_select_click_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context)
{
  if(select_allowed == 1)
  {
    select_allowed = 0;
    send_stop(cur_stop, 1, window);
  }
}

void stops_window_load(Window *window)
{
  //Create it - 12 is approx height of the top bar
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  menu_layer = menu_layer_create(bounds);
  
  //Let it receive clicks
  menu_layer_set_click_config_onto_window(menu_layer, window);

  //Give it its callbacks
  MenuLayerCallbacks callbacks = {
    .draw_header = (MenuLayerDrawHeaderCallback) stops_menu_draw_header_callback,
    .draw_row = (MenuLayerDrawRowCallback) stops_draw_row_callback,
    .get_num_sections = (MenuLayerGetNumberOfSectionsCallback) stops_num_sections_callback,
    .get_num_rows = (MenuLayerGetNumberOfRowsInSectionsCallback) stops_num_rows_callback,
    .get_header_height = (MenuLayerGetHeaderHeightCallback) stops_header_height_callback,
    .select_click = (MenuLayerSelectCallback) stops_select_click_callback
  };
  menu_layer_set_callbacks(menu_layer, NULL, callbacks);

  //Add to Window
  layer_add_child(window_get_root_layer(window), menu_layer_get_layer(menu_layer));

}
 
void stops_window_unload(Window *window)
{
  menu_layer_destroy(menu_layer);

}

void stops_init(Departure departs[], int departure_num, char * stop)
{
  select_allowed = 1;
  APP_LOG(APP_LOG_LEVEL_DEBUG, "START COPYING OVER DEPARTURES");
    for (int i = 0; i < departure_num; i++) {
      strcpy(deps[i].headsign, departs[i].headsign);
      strcpy(deps[i].expected_time, departs[i].expected_time);
      
      APP_LOG(APP_LOG_LEVEL_DEBUG, "SIGN: %s", deps[i].headsign);
      APP_LOG(APP_LOG_LEVEL_DEBUG, "TIME: %s", deps[i].expected_time);
    }
    cur_stop = malloc(sizeof(char) * (strlen(stop) + 1));
    strcpy(cur_stop, stop);
    num_departures = departure_num;    
  
    APP_LOG(APP_LOG_LEVEL_DEBUG, "start init");
    window = window_create();
    WindowHandlers handlers = {
        .load = stops_window_load,
        .unload = stops_window_unload
    };

    window_set_window_handlers(window, (WindowHandlers) handlers);
    window_stack_push(window, true);
}
 
void stops_deinit()
{
    window_destroy(window);
}

