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


#ifndef _DEVICE_OPTIONS_H_
#define _DEVICE_OPTIONS_H_

#include <stdio.h>
#include <stdbool.h>
#include <vconf.h>
#include <Elementary.h>
#include <appcore-efl.h>
#include <Ecore_X.h>
#include <glib.h>
#include <dlog.h>
#include <feedback.h>

#undef LOG_TAG
#define LOG_TAG "DEVICE_OPTIONS"
#define _D(fmt, args...)   SLOGD(fmt, ##args)
#define _E(fmt, args...)   SLOGE(fmt, ##args)
#define _I(fmt, args...)   SLOGI(fmt, ##args)

#define FREE(arg) \
	do { \
		if(arg) { \
			free((void *)arg); \
			arg = NULL; \
		} \
	} while (0);

#define ARRAY_SIZE(name) (sizeof(name)/sizeof(name[0]))

#define max(a,b) \
	({ __typeof__ (a) _a = (a); \
	   __typeof__ (b) _b = (b);  \
	   _a > _b ? _a : _b; })

#define EDJ_NAME       DO_EDJE_DIR"/poweroff.edj"
#define IMAGE_LARGE   (56*elm_config_scale_get())
#define IMAGE_SMALL_W (65*elm_config_scale_get())
#define IMAGE_SMALL_H (42*elm_config_scale_get())
#define BUF_MAX        256

struct appdata {
	Evas_Object *win_main;
	Evas_Object *popup;
	Evas_Object *list;
};

struct device_option {
	/* The values can be changed by modules */
	const char                  *name;
	bool (*terminate)           (void);
	bool (*is_enabled)          (void);
	int  (*get_item_type)       (void);
	int  (*get_id)              (void);
	int  (*get_icon)            (char *icon, unsigned int len);
	bool (*icon_disabled)       (void);
	int  (*get_text)            (char *text, unsigned int len);
	int  (*get_sub_text)        (char *text, unsigned int len);
	int  (*response_clicked)    (void *data);
	void                        *clicked_data;
	int  (*register_handlers)   (void *data);
	int  (*unregister_handlers) (void *data);

	/* Do not change the values.
	 * The values will be assigned automatically */
	struct appdata              *ad;
	Elm_Genlist_Item_Class      itc;
	Elm_Object_Item             *item;
	Evas_Object                 *btn;
};

enum item_type {
	/* two items are on one entry of popup */
	HALF_ITEM,

	/* one item is on one entry of popup */
	FULL_ITEM_1TEXT_1ICON, /* item with 1 text and 1icon */
	FULL_ITEM_2TEXT,       /* item with 2 text */
};

enum power_saving_state {
	POWER_SAVING_OFF			= 0,
	POWER_SAVING_ON				= SETTING_PSMODE_WEARABLE,
	POWER_SAVING_ON_ENHANCED	= SETTING_PSMODE_WEARABLE_ENHANCED,
};

void popup_terminate(void);

void register_device_options_item(const struct device_option *opt);

int get_full_items_list(GList **l);
int get_half_items_list(GList **l);
unsigned int nr_full_items_list(void);
unsigned int nr_half_items_list(void);

void play_feedback(int type, int pattern);
int power_saving_mode(void);
bool enhanced_power_saving_mode(void);

#endif /* _DEVICE_OPTIONS_H_ */
