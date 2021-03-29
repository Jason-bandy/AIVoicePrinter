/**
 ******************************************************************************
 *
 * @file rwnx_main.h
 *
 * Copyright (C) RivieraWaves 2012-2019
 *
 ******************************************************************************
 */

#ifndef _RWNX_MAIN_H_
#define _RWNX_MAIN_H_

#include "rwnx_defs.h"

int rwnx_cfg80211_init(struct rwnx_plat *rwnx_plat, void **platform_data);
void rwnx_cfg80211_deinit(struct rwnx_hw *rwnx_hw);

#endif /* _RWNX_MAIN_H_ */
