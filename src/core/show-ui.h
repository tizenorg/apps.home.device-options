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

#ifndef _SHOW_UI_H_
#define _SHOW_UI_H_

#include "device-options.h"

/* Core */
int create_device_options_popup(struct appdata *ad);
void unregister_toast_handlers(struct appdata *ad);

/* Feature */
void register_device_options_popup(int (*show)(struct appdata *ad));

/* Common */
void release_evas_object(Evas_Object **obj);
Evas_Object *create_win(const char *name);
void raise_window(Evas_Object *win);

void update_item(struct device_option *opt);
int load_popup_toast(struct appdata *ad, char *content);

#endif /* _SHOW_UI_H_ */
