#include <pebble.h>
#include "elements.h"

static TextLayer* text_layer_init(GRect location, GColor background, GTextAlignment alignment, int font)
{
	TextLayer *layer = text_layer_create(location);
	text_layer_set_text_color(layer, GColorBlack);
	text_layer_set_background_color(layer, background);
	text_layer_set_text_alignment(layer, alignment);
	if(font == 1){
		text_layer_set_font(layer, coda);
	}
	else if(font == 2){
		text_layer_set_font(layer, coda_small);
	}
	return layer;
}

void on_animation_stopped(Animation *anim, bool finished, void *context)
{
    property_animation_destroy((PropertyAnimation*) anim);
}
 
void animate_layer(Layer *layer, GRect *start, GRect *finish, int delay)
{
    PropertyAnimation *anim = property_animation_create_layer_frame(layer, start, finish);
     
    animation_set_duration((Animation*) anim, animationSpeed);
    animation_set_delay((Animation*) anim, delay);
     
	AnimationHandlers handlers = {
    .stopped = (AnimationStoppedHandler) on_animation_stopped
    };
    animation_set_handlers((Animation*) anim, handlers, NULL);
     
    animation_schedule((Animation*) anim);
}

void reset_data(int timer){
	if(timer == 0){
		min5_td.sec = 0;
		min5_td.min = 5;
	}
	else if(timer == 1){
		min21_td.sec = 0;
		min21_td.min = 21;
	}
	else if(timer == 2){
		hour4_td.sec = 0;
		hour4_td.min = 0;
		hour4_td.hour = 4;
	}
	else if(timer == 3){
		min5_td.sec = 0;
		min5_td.min = 5;
		min21_td.sec = 0;
		min21_td.min = 21;
		hour4_td.sec = 0;
		hour4_td.min = 0;
		hour4_td.hour = 4;
	}
}

void timeIsUp(int timerEnded){
	vibes_long_pulse();
	first_menu_items[timerEnded].subtitle = "Ended.";
	reset_data(timerEnded);
}
	
void tick_handler(struct tm *tick_time, TimeUnits units_changed){
	if(timer1_running){
		min5_td.sec--;
		if(min5_td.sec == 0 && min5_td.min == 0) 
		{
			timeIsUp(0);
			timer1_running = 0;
			return;
		}
		if(min5_td.sec < 0)
		{
			if(min5_td.min > 0){
				min5_td.min--;
				min5_td.sec = 59;
			}
		}
	}
	if(timer2_running){
		min21_td.sec--;
		if(min21_td.sec == 0 && min21_td.min == 0 && min21_td.hour == 0) 
		{
			timeIsUp(1);
			timer2_running = 0;
			return;
		}
		if(min21_td.sec < 0)
		{
			if(min21_td.min > 0){
				min21_td.min--;
				min21_td.sec = 59;
			}
		}
	}
	if(timer3_running){
		hour4_td.sec--;
		if(hour4_td.sec == 0 && hour4_td.min == 0 && hour4_td.hour == 0) 
		{
			timeIsUp(2);
			timer3_running = 0;
			return;
		}
		if(hour4_td.sec < 0)
		{
			if(hour4_td.min > 0){
				hour4_td.min--;
				hour4_td.sec = 59;
			}
			else if(hour4_td.min == 0 && hour4_td.hour > 0){
				hour4_td.sec = 59;
				hour4_td.min = 59;
				hour4_td.hour--;
			}
		}
	}
	if(min5_td.sec < 10){
		snprintf(timer1_buffer, sizeof(timer1_buffer), "%d:0%d", min5_td.min, min5_td.sec);
	}
	else{
		snprintf(timer1_buffer, sizeof(timer1_buffer), "%d:%d", min5_td.min, min5_td.sec);
	}
	
	if(min21_td.sec < 10){
		snprintf(timer2_buffer, sizeof(timer2_buffer), "%d:0%d", min21_td.min, min21_td.sec);
	}
	else{
		snprintf(timer2_buffer, sizeof(timer2_buffer), "%d:%d", min21_td.min, min21_td.sec);
	}
	
	if(hour4_td.sec < 10){
		if(hour4_td.min < 10){
			snprintf(timer3_buffer, sizeof(timer3_buffer), "%d:0%d:0%d", hour4_td.hour, hour4_td.min, hour4_td.sec);
		}
		else{
			snprintf(timer3_buffer, sizeof(timer3_buffer), "%d:%d:0%d", hour4_td.hour, hour4_td.min, hour4_td.sec);
		}
	}
	else{
		if(hour4_td.min < 10){
			snprintf(timer3_buffer, sizeof(timer3_buffer), "%d:0%d:%d", hour4_td.hour, hour4_td.min, hour4_td.sec);
		}
		else{
			snprintf(timer3_buffer, sizeof(timer3_buffer), "%d:%d:%d", hour4_td.hour, hour4_td.min, hour4_td.sec);
		}
	}
		
	text_layer_set_text(minute_5_t, timer1_buffer);
	text_layer_set_text(minute_21_t, timer2_buffer);
	text_layer_set_text(hour_4_t, timer3_buffer);
}

void timer_callback(int index, void *ctx){
	APP_LOG(APP_LOG_LEVEL_INFO, "Index: %d", index);
	timerFired = index;
	window_stack_push(timer_window, true);
	first_menu_items[timerFired].subtitle = "Running";
	if(timerFired == 3){
		first_menu_items[0].subtitle = "Running";
		first_menu_items[1].subtitle = "Running";
		first_menu_items[2].subtitle = "Running";
	}
}

void team_callback(int index, void *ctx){
	settings.team = !settings.team;
	if(settings.team){
		third_menu_items[0].subtitle = "Resistance";
	}
	else{
		third_menu_items[0].subtitle = "Enlightened";
	}
	layer_mark_dirty(simple_menu_layer_get_layer(main_menu));
}

void theme_callback(int index, void *ctx){
	settings.theme = !settings.theme;
	if(settings.theme){
		third_menu_items[1].subtitle = "Light";
	}
	else{
		third_menu_items[1].subtitle = "Dark";
	}
	layer_mark_dirty(simple_menu_layer_get_layer(main_menu));
	layer_set_hidden(inverter_layer_get_layer(menu_theme), settings.theme);
	layer_set_hidden(inverter_layer_get_layer(theme), settings.theme);
}
	
void window_load_main(Window *window){
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_frame(window_layer);
	
	first_menu_items[0] = (SimpleMenuItem){
		.title = "5 minutes",
		.subtitle = "Start",
		.callback = timer_callback,
	};
	first_menu_items[1] = (SimpleMenuItem){
		.title = "21 minutes",
		.subtitle = "Start",
		.callback = timer_callback,
	};
	first_menu_items[2] = (SimpleMenuItem){
		.title = "4 hours",
		.subtitle = "Start",
		.callback = timer_callback,
	};
	first_menu_items[3] = (SimpleMenuItem){
		.title = "All timers",
		.callback = timer_callback,
	};
	
	second_menu_items[0] = (SimpleMenuItem){
		.title = "Aboot",
		.subtitle = "Created by Edwin Finch",
	};
	
	third_menu_items[0] = (SimpleMenuItem){
		.title = "Team",
		.subtitle = "Resistance",
		.callback = team_callback,
	};
	third_menu_items[1] = (SimpleMenuItem){
		.title = "Theme",
		.subtitle = "Light",
		.callback = theme_callback,
	};
	
	menu_sections[0] = (SimpleMenuSection){
		.title = "Ingress Timers",
		.num_items = NUM_FIRST_MENU_ITEMS,
		.items = first_menu_items,
	};
	menu_sections[1] = (SimpleMenuSection){
		.title = "Other",
		.num_items = NUM_SECOND_MENU_ITEMS,
		.items = second_menu_items,
	};
	menu_sections[2] = (SimpleMenuSection){
		.title = "Settings",
		.num_items = NUM_THIRD_MENU_ITEMS,
		.items = third_menu_items,
	};
	
	if(settings.team){
		third_menu_items[0].subtitle = "Resistance";
	}
	else{
		third_menu_items[0].subtitle = "Enlightened";
	}
	if(settings.theme){
		third_menu_items[1].subtitle = "Light";
	}
	else{
		third_menu_items[1].subtitle = "Dark";
	}
	
	main_menu = simple_menu_layer_create(bounds, main_window, menu_sections, NUM_MENU_SECTIONS, NULL);
	menu_theme = inverter_layer_create(GRect(0, 0, 144, 168));
	layer_add_child(window_layer, simple_menu_layer_get_layer(main_menu));
	layer_add_child(window_layer, inverter_layer_get_layer(menu_theme));
	layer_set_hidden(inverter_layer_get_layer(menu_theme), settings.theme);
}

void up(ClickRecognizerRef recognizer, void *context){
	if(timerFired == 0){
		timer1_running = 1;
	}
	else if(timerFired == 1){
		timer2_running = 1;
	}
	else if(timerFired == 2){
		timer3_running = 1;
	}
	else if(timerFired == 3){
		timer1_running = 1;
		timer2_running = 1;
		timer3_running = 1;
	}
	first_menu_items[timerFired].subtitle = "Running";
}

void down(ClickRecognizerRef recognizer, void *context){
	if(timerFired == 0){
		timer1_running = 0;
	}
	else if(timerFired == 1){
		timer2_running = 0;
	}
	else if(timerFired == 2){
		timer3_running = 0;
	}
	else if(timerFired == 3){
		timer1_running = 0;
		timer2_running = 0;
		timer3_running = 0;
		first_menu_items[0].subtitle = "Paused";
		first_menu_items[1].subtitle = "Paused";
		first_menu_items[2].subtitle = "Paused";
	}
	first_menu_items[timerFired].subtitle = "Paused";
}

void select(ClickRecognizerRef recognizer, void *context){
	reset_data(timerFired);
}

void click_config_prov(void *context){
	window_single_click_subscribe(BUTTON_ID_UP, (ClickHandler) up);
	window_single_click_subscribe(BUTTON_ID_DOWN, (ClickHandler) down);
	window_single_click_subscribe(BUTTON_ID_SELECT, (ClickHandler) select);
}

void window_unload_main(Window *window){
	simple_menu_layer_destroy(main_menu);
}

void window_load_timer(Window *window){
	Layer *window_layer = window_get_root_layer(window);
	minute_5_t = text_layer_init(GRect(0, 30, 144, 168), GColorClear, GTextAlignmentCenter, 1);
	minute_21_t = text_layer_init(GRect(0, 70, 144, 168), GColorClear, GTextAlignmentCenter, 1);
	hour_4_t = text_layer_init(GRect(0, 120, 144, 168), GColorClear, GTextAlignmentCenter, 2);
	theme = inverter_layer_create(GRect(0, 0, 144, 168));
	ab = action_bar_layer_create();
	
	if(settings.team){
		bitmap_layer_set_bitmap(team_layer, resistance);
	}
	else{
		bitmap_layer_set_bitmap(team_layer, enlightened);
	}
	
	layer_add_child(window_layer, text_layer_get_layer(minute_5_t));
	layer_add_child(window_layer, text_layer_get_layer(minute_21_t));
	layer_add_child(window_layer, text_layer_get_layer(hour_4_t));
	layer_add_child(window_layer, bitmap_layer_get_layer(team_layer));
	layer_set_hidden(inverter_layer_get_layer(theme), settings.theme);
	action_bar_layer_add_to_window(ab, window);
	layer_add_child(window_layer, inverter_layer_get_layer(theme));
	action_bar_layer_set_click_config_provider(ab, click_config_prov);
	action_bar_layer_set_icon(ab, BUTTON_ID_UP, play);
	action_bar_layer_set_icon(ab, BUTTON_ID_SELECT, restart);
	action_bar_layer_set_icon(ab, BUTTON_ID_DOWN, pause);
	
	text_layer_set_text(minute_5_t, "Timer 5m");
	text_layer_set_text(minute_21_t, "Timer 21m");
	text_layer_set_text(hour_4_t, "Timer 4h");
	//5 min
	if(timerFired == 0){
		timer1_running = 1;
		layer_set_hidden(text_layer_get_layer(minute_21_t), true);
		layer_set_hidden(text_layer_get_layer(hour_4_t), true);
		GRect start = GRect(-144, 20, 144, 168);
		GRect finish = GRect(0, 20, 144, 168);
		animate_layer(text_layer_get_layer(minute_5_t), &start, &finish, 10);
	}
	//21 min
	else if(timerFired == 1){
		timer2_running = 1;
		layer_set_hidden(text_layer_get_layer(minute_5_t), true);
		layer_set_hidden(text_layer_get_layer(hour_4_t), true);
		GRect start = GRect(-144, 20, 144, 168);
		GRect finish = GRect(0, 20, 144, 168);
		animate_layer(text_layer_get_layer(minute_21_t), &start, &finish, 10);	
	}
	//4 hours
	else if(timerFired == 2){
		timer3_running = 1;
		layer_set_hidden(text_layer_get_layer(minute_21_t), true);
		layer_set_hidden(text_layer_get_layer(minute_5_t), true);
		GRect start = GRect(-144, 20, 144, 168);
		GRect finish = GRect(0, 20, 144, 168);
		animate_layer(text_layer_get_layer(hour_4_t), &start, &finish, 10);	
	}
	//4 hours
	else if(timerFired == 3){
		timer1_running = 1;
		timer2_running = 1;
		timer3_running = 1;
		GRect start = GRect(-144, 0, 144, 168);
		GRect finish = GRect(0, 0, 144, 168);
		animate_layer(text_layer_get_layer(minute_5_t), &start, &finish, 10);
		start = GRect(-144, 40, 144, 168);
		finish = GRect(0, 40, 144, 168);
		animate_layer(text_layer_get_layer(minute_21_t), &start, &finish, 10);	
		start = GRect(-144, 80, 144, 168);
		finish = GRect(0, 80, 144, 168);
		animate_layer(text_layer_get_layer(hour_4_t), &start, &finish, 10);
		layer_set_hidden(bitmap_layer_get_layer(team_layer), true);
	}
	if(timerFired != 3){
		layer_set_hidden(bitmap_layer_get_layer(team_layer), false);
		GRect start = GRect(0, 90, 144, 168);
		GRect finish = GRect(0, 30, 144, 168);
		animate_layer(bitmap_layer_get_layer(team_layer), &start, &finish, 10);
	}
	struct tm *t;
  	time_t temp;        
  	temp = time(NULL);        
  	t = localtime(&temp);
	
	tick_handler(t, SECOND_UNIT);
}

void window_unload_timer(Window *window){
	text_layer_destroy(minute_5_t);
	text_layer_destroy(minute_21_t);
	text_layer_destroy(hour_4_t);
	inverter_layer_destroy(theme);
	animation_unschedule_all();
}
	
void init(void){
	main_window = window_create();
	timer_window = window_create();
	
	window_set_window_handlers(main_window, (WindowHandlers){
		.load = window_load_main,
		.unload = window_unload_main,
	});
	
	window_set_window_handlers(timer_window, (WindowHandlers){
		.load = window_load_timer,
		.unload = window_unload_timer,
	});
	
	value = persist_read_data(SETTINGS_KEY, &settings, sizeof(settings));
	APP_LOG(APP_LOG_LEVEL_INFO, "%d bytes read from storage", value);
	
	play = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PLAY);
	pause = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PAUSE);
	restart = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_RESTART);
	coda = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_CODA_35));
	coda_small = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_CODA_29));
	resistance = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_RESISTANCE);
	enlightened = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_ENLIGHTENED);
	
	team_layer = bitmap_layer_create(GRect(0, 30, 144, 168));
	
	tick_timer_service_subscribe(SECOND_UNIT, &tick_handler);
	window_stack_push(main_window, true);
	timerFired = 34;
	window_stack_push(timer_window, true);
	window_stack_pop(true);
}

void deinit(void){
	window_destroy(main_window);
	window_destroy(timer_window);
	gbitmap_destroy(play);
	gbitmap_destroy(pause);
	gbitmap_destroy(restart);
	fonts_unload_custom_font(coda);
	fonts_unload_custom_font(coda_small);
	value = persist_write_data(SETTINGS_KEY, &settings, sizeof(settings));
	APP_LOG(APP_LOG_LEVEL_INFO, "%d bytes written to storage", value);
}

int main(){
	init();
	app_event_loop();
	deinit();
}