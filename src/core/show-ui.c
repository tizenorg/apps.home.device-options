/*
 * device options
 *
 * Copyright (c) 2014 Samsung Electronics Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/


#include <stdio.h>
#include <utilX.h>
#include <efl_assist.h>
#include <app.h>
#include "device-options.h"

#define TABLE_COLOR TABLE_XML_PATH"/poweroff-color.xml"
#define TABLE_FONT  TABLE_XML_PATH"/poweroff-font.xml"

static int (*show_device_options_popup)(struct appdata *ad) = NULL;

void register_device_options_popup(int (*show)(struct appdata *ad))
{
	if (show_device_options_popup)
		return;

	if (show)
		show_device_options_popup = show;
}

int create_device_options_popup(struct appdata *ad)
{
	if (show_device_options_popup)
		return show_device_options_popup(ad);
	else
		return -EINVAL;
}

/* Release evas object */
void release_evas_object(Evas_Object **obj)
{
	if (!obj || !(*obj))
		return;
	evas_object_del(*obj);
	*obj = NULL;
}

static void reset_window_priority(Evas_Object *win, int priority)
{
	Ecore_X_Window xwin = elm_win_xwindow_get(win);
	Display *dpy = ecore_x_display_get();

	ecore_x_netwm_window_type_set(xwin, ECORE_X_WINDOW_TYPE_NOTIFICATION);
	utilx_set_system_notification_level(dpy, xwin, priority);
}

static void win_del(void *data, Evas_Object * obj, void *event)
{
	popup_terminate();
}

Evas_Object *create_win(const char *name)
{
	Evas_Object *eo;
	Ea_Theme_Color_Table *color;
	Ea_Theme_Font_Table *font;

	int w, h;

	if (!name)
		return NULL;

	eo = app_get_preinitialized_window(name);
	if (!eo) {
		eo = elm_win_add(NULL, name, ELM_WIN_BASIC);
		if (!eo) {
			_E("FAIL: elm_win_add()");
			return NULL;
		}
	}

	elm_win_title_set(eo, name);
	elm_win_borderless_set(eo, EINA_TRUE);
	elm_win_alpha_set(eo, EINA_TRUE);
	elm_win_raise(eo);
	evas_object_smart_callback_add(eo, "delete,request", win_del, NULL);
	ecore_x_window_size_get(ecore_x_window_root_first_get(), &w, &h);
	evas_object_resize(eo, w, h);

	reset_window_priority(eo, UTILX_NOTIFICATION_LEVEL_HIGH);

	ea_theme_changeable_ui_enabled_set(EINA_TRUE);

	color = ea_theme_color_table_new(TABLE_COLOR);
	if (color) {
		ea_theme_colors_set(color, EA_THEME_STYLE_DEFAULT);
		ea_theme_color_table_free(color);
	}

	font = ea_theme_font_table_new(TABLE_FONT);
	if (font) {
		ea_theme_fonts_set(font);
		ea_theme_font_table_free(font);
	}

	return eo;
}

void raise_window(Evas_Object *win)
{
	if (win)
		elm_win_raise(win);
}

void update_item(struct device_option *opt)
{
	if (opt && opt->item)
		elm_genlist_item_update(opt->item);
}
