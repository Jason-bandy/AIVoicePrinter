/**
 ****************************************************************************************
 *
 * @file txl_cfm.h
 *
 * @brief Tx confirmation module.
 *
 * Copyright (C) RivieraWaves 2011-2019
 *
 ****************************************************************************************
 */

#ifndef _TXL_CFM_H_
#define _TXL_CFM_H_

/**
 ****************************************************************************************
 * @defgroup TX_CFM TX_CFM
 * @ingroup TX
 * @brief LMAC Tx confirmation module.
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "co_int.h"
#include "co_list.h"
#include "tx_swdesc.h"

/*
 * TYPE and STRUCT DEFINITIONS
 ****************************************************************************************
 */
/// Table mapping the TX confirmation event bit to the queue index
extern const uint32_t txl_cfm_evt_bit[NX_TXQ_CNT];

/// Context of the Tx Control block
struct txl_cfm_env_tag
{
    /// Tx confirmation list
    struct co_list cfmlist[NX_TXQ_CNT];
};

/*
 * GLOBAL VARIABLE DECLARATION
 ****************************************************************************************
 */
/// Tx Confirmation context variable
extern struct txl_cfm_env_tag txl_cfm_env;


/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * @brief Get the confirmation structure attached to a TX descriptor
 *
 * @param[in] txdesc  TX descriptor pointer
 *
 * @return The pointer to the confirmation structure attached to this TX descriptor
 ****************************************************************************************
 */
__INLINE struct tx_cfm_tag *txl_cfm_tag_get(struct txdesc *txdesc)
{
    #if NX_FULLY_HOSTED
    struct tx_cfm_tag *cfm = &txdesc->host.cfm;
    #else
    struct tx_cfm_tag *cfm = &txdesc->lmac.hw_desc->cfm;
    #endif

    return cfm;
}

/**
 ****************************************************************************************
 * @brief Initializes the CFM and BA queues.
 * These queues are useful for sending confirmation to UMAC for the txdescs in order.
 *
 * The CFM queue will handle the linked list of txdescs, each having a certain status
 * after handling the TX trigger from HW (acked, waiting for BA analysis, not acked at all)
 *
 * The BA queue holds the linked list of rxdescs of BA frames per AC, which will be used and
 * freed when the TX_CFM kernel event is handled in order to update the MPDU status
 * of those MPDUs part of AMPDUs in the confirm list.
 *
 * For singleton MPDUs the BA queue is of no use, they should be confirmed according to
 * their already set ok/not acked status.
 *
 ****************************************************************************************
 */
void txl_cfm_init(void);

/**
 ****************************************************************************************
 * @brief Push a Tx descriptor in the CFM queue
 *
 * @param[in] txdesc            Tx descriptor to be pushed in the CFM queue
 * @param[in] status            Status of the confirmation
 * @param[in] access_category   Access category on which the confirmation is pushed
 *
 ****************************************************************************************
 */
void txl_cfm_push(struct txdesc *txdesc, uint32_t status, uint8_t access_category);

/**
 ****************************************************************************************
 * @brief Background event handler of the Tx confirmation module
 *
 * TX descriptors are linked in the cfmlist, ACs mixed but in order of arrival per AC.
 * When the event is programmed, a txdesc in the cfmlist front is analyzed.
 *
 * It can be the 1st MPDU in an AMPDU whose BA reception status is known, because all the
 * MPDUs in an AMPDU are not moved from transmitting list (per AC) to cfmlist until
 * their BA reception status is known (updated by Hw in either the AMPDU THD or BAR THD.
 *
 * When it is such an MPDU, the first BA received in the ba_list queue is retrieved,
 * and the MPDUs in the AMPDU are taken one by one from the cfm list, their sattus is updated
 * and they are sent to host.
 *
 * The BA reception status is present in either AMPDU THD or BAR THD, but it doesn't matter
 * how it was received, but IF it was received, the bitmap is extracted for the AMPDU
 * MPDUs in any case.
 *
 * When the MPDU in the cfm list is a singleton MPDU, it already has its status so it is
 * released towards the host immediately without further handling.
 *
 * @param[in] access_category Access Category for which confirmations need to be handled
 *
 ****************************************************************************************
 */
void txl_cfm_evt(int access_category);

#if NX_AMPDU_TX
/**
 ****************************************************************************************
 * @brief Push a Rx descriptor containing a BA frame in the appropriate aggregate
 * descriptor on the right AC.
 * This function associates the received BA to an AGG descriptor and verifies its
 * validity.
 *
 * @param[in] rxdesc     Pointer to Rx descriptor to be pushed in the BA queue
 *
 ****************************************************************************************
 */
void txl_ba_push(struct rxdesc *rxdesc);
#endif

/**
 ****************************************************************************************
 * @brief Immediately confirm the descriptor passed as parameter
 *
 * @param[in] access_category  Access category corresponding to the list
 * @param[in] txdesc           Pointer to the descriptor of descriptors to be confirmed
 * @param[in] status           Status to be sent in the confirmation
 *
 ****************************************************************************************
 */
void txl_cfm_flush_desc(uint8_t access_category, struct txdesc *txdesc, uint32_t status);

/**
 ****************************************************************************************
 * @brief Immediately confirm all the descriptors of the list passed as parameter
 *
 * @param[in] access_category  Access category corresponding to the list
 * @param[in] list             Pointer to the list of descriptors to be confirmed
 * @param[in] status           Status to be sent in the confirmation
 *
 ****************************************************************************************
 */
void txl_cfm_flush(uint8_t access_category, struct co_list *list, uint32_t status);

/// @}

#endif // _TX_CFM_H_
