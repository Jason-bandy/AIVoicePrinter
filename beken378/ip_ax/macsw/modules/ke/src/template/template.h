/**
 ****************************************************************************************
 *
 * @file template.h
 *
 * @brief Declaration of the Template module environment.
 *
 * Copyright (C) RivieraWaves 2011-2019
 *
 ****************************************************************************************
 */
#ifndef _TEMPLATE_H_
#define _TEMPLATE_H_

/**
 ****************************************************************************************
 * @defgroup TEMPLATE TEMPLATE
 * @ingroup MACSW
 * @brief Description of the module.
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "co_int.h"

/**
 ****************************************************************************************
 * @brief Declaration of TEMPLATE environment.
 * Any module or task will retrieve the TEMPLATE environment on its own, it is
 * not passed anymore as a parameter to the function handlers
 * of the TEMPLATE task.  If an other module wants to make use of the
 * environment of TEMPLATE, it simply needs to include this file and use
 * the extern global variable.
 ****************************************************************************************
 */
struct template
{
    // NO STATE in this environment structure (this used to be
    // the case but not anymore)

    /// The first element of the TEMPLATE environment.
    uint16_t hit_cnt;

    /** This is an example of a longer comment for a single
     * element of the structure and therefore is using a block
     * comment that is still parsable by doxygen.  This can be
     * used but is not encouraged since long descriptions are
     * more useful when describing the entire structures.
     */
    uint8_t longer_element;
};

/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */
/// TEMPLATE module environment declaration.
extern struct template template_env;

/// @} end of group

#endif // _TEMPLATE_H_
