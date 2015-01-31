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
#include "device-options.h"

void play_feedback(int type, int pattern)
{
	int ret;

	ret = feedback_initialize();
	if (ret != FEEDBACK_ERROR_NONE) {
		_E("Cannot initialize feedback");
		return;
	}

	switch (type) {
	case FEEDBACK_TYPE_LED:
	case FEEDBACK_TYPE_SOUND:
	case FEEDBACK_TYPE_VIBRATION:
		ret = feedback_play_type(type, pattern);
		break;

	case FEEDBACK_TYPE_NONE:
		ret = feedback_play(pattern);
		break;
	default:
		_E("Play type is unknown");
		ret = 0;
	}

	if (ret != FEEDBACK_ERROR_NONE)
		_E("Cannot play feedback: %d", pattern);
	ret = feedback_deinitialize();
	if (ret != FEEDBACK_ERROR_NONE)
		_E("Cannot deinitialize feedback");
}

int power_saving_mode(void)
{
	int state;

	if (vconf_get_int(VCONFKEY_SETAPPL_PSMODE, &state) != 0)
		return 0;

	return state;
}

bool enhanced_power_saving_mode(void)
{
	if (power_saving_mode() == POWER_SAVING_ON_ENHANCED)
		return true;
	return false;
}
