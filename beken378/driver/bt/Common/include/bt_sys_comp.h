/*************************************************************
 * @file        bt_sys_comp.h
 * @brief       compement of deleted driver
 * @author      lv
 * @version     V1.0
 * @date        2019-09-17
 * @par         
 * @attention   
 *
 * @history     2019-09-17    create this file
 */

#ifndef __DRIVER_BT_SYS_COMP_H__

#define __DRIVER_BT_SYS_COMP_H__


#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */




void VICMR_disable_interrupts(unsigned int *interrupts_info_ptr);
void VICMR_restore_interrupts(unsigned int interrupts_info);





#ifdef __cplusplus
}
#endif  /* __cplusplus */


#endif      /* __DRIVER_WDT_H__ */
