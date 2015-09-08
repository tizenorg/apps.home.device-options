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
#define BUF_MAX        256

struct appdata {
	Evas_Object *win_main;
	Evas_Object *popup;
	Evas_Object *list;
};

struct poweroff_option {
	const char *name;
	char *title;
	char *content;
	char *ltext;
	char *licon;
	void (*left)(void *data, Evas_Object * obj, void *event_info);
	char *rtext;
	char *ricon;
	void (*right)(void *data, Evas_Object * obj, void *event_info);
};

void popup_terminate(void);
void play_feedback(int type, int pattern);

#endif /* _DEVICE_OPTIONS_H_ */
