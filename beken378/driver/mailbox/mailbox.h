#ifndef _MAILBOX_H_
#define _MAILBOX_H_

#include "include.h"
#include "intc_pub.h"

#define MAILBOX_DEBUG

#ifdef MAILBOX_DEBUG
#define MAILBOX_PRT      os_printf
#define MAILBOX_WARN     warning_prf
#define MAILBOX_FATAL    fatal_prf
#else
#define MAILBOX_PRT      null_prf
#define MAILBOX_WARN     null_prf
#define MAILBOX_FATAL    null_prf
#endif

//MailBox cpu to dsp
#define MAILBOX_CPU2DSP_BASE                   (0x06000000)

#define MAILBOX_CPU2DSP_BOX0_CMD               (MAILBOX_CPU2DSP_BASE + 0 * 4)
#define MAILBOX_CPU2DSP_BOX0_PARAM1            (MAILBOX_CPU2DSP_BASE + 1 * 4)
#define MAILBOX_CPU2DSP_BOX0_PARAM2            (MAILBOX_CPU2DSP_BASE + 2 * 4)
#define MAILBOX_CPU2DSP_BOX0_PARAM3            (MAILBOX_CPU2DSP_BASE + 3 * 4)

#define MAILBOX_CPU2DSP_BOX1_CMD               (MAILBOX_CPU2DSP_BASE + 4 * 4)
#define MAILBOX_CPU2DSP_BOX1_PARAM1            (MAILBOX_CPU2DSP_BASE + 5 * 4)
#define MAILBOX_CPU2DSP_BOX1_PARAM2            (MAILBOX_CPU2DSP_BASE + 6 * 4)
#define MAILBOX_CPU2DSP_BOX1_PARAM3            (MAILBOX_CPU2DSP_BASE + 7 * 4)

#define MAILBOX_CPU2DSP_READY                  (MAILBOX_CPU2DSP_BASE + 8 * 4)

#define MAILBOX_CPU2DSP_CLEAR                  (MAILBOX_CPU2DSP_BASE + 12 * 4)

//MailBox dsp to cpu
#define MAILBOX_DSP2CPU_BASE                   (0x06000040)

#define MAILBOX_DSP2CPU_BOX0_CMD               (MAILBOX_DSP2CPU_BASE + 0 * 4)
#define MAILBOX_DSP2CPU_BOX0_PARAM1            (MAILBOX_DSP2CPU_BASE + 1 * 4)
#define MAILBOX_DSP2CPU_BOX0_PARAM2            (MAILBOX_DSP2CPU_BASE + 2 * 4)
#define MAILBOX_DSP2CPU_BOX0_PARAM3            (MAILBOX_DSP2CPU_BASE + 3 * 4)

#define MAILBOX_DSP2CPU_BOX1_CMD               (MAILBOX_DSP2CPU_BASE + 4 * 4)
#define MAILBOX_DSP2CPU_BOX1_PARAM1            (MAILBOX_DSP2CPU_BASE + 5 * 4)
#define MAILBOX_DSP2CPU_BOX1_PARAM2            (MAILBOX_DSP2CPU_BASE + 6 * 4)
#define MAILBOX_DSP2CPU_BOX1_PARAM3            (MAILBOX_DSP2CPU_BASE + 7 * 4)

#define MAILBOX_DSP2CPU_READY                  (MAILBOX_DSP2CPU_BASE + 8 * 4)

#define MAILBOX_DSP2CPU_CLEAR                  (MAILBOX_DSP2CPU_BASE + 12 * 4)

//MailBox cpu to bt
#define MAILBOX_CPU2BT_BASE                    (0x06000080)

#define MAILBOX_CPU2BT_BOX0_CMD                (MAILBOX_CPU2BT_BASE + 0 * 4)
#define MAILBOX_CPU2BT_BOX0_PARAM1             (MAILBOX_CPU2BT_BASE + 1 * 4)
#define MAILBOX_CPU2BT_BOX0_PARAM2             (MAILBOX_CPU2BT_BASE + 2 * 4)
#define MAILBOX_CPU2BT_BOX0_PARAM3             (MAILBOX_CPU2BT_BASE + 3 * 4)

#define MAILBOX_CPU2BT_BOX1_CMD                (MAILBOX_CPU2BT_BASE + 4 * 4)
#define MAILBOX_CPU2BT_BOX1_PARAM1             (MAILBOX_CPU2BT_BASE + 5 * 4)
#define MAILBOX_CPU2BT_BOX1_PARAM2             (MAILBOX_CPU2BT_BASE + 6 * 4)
#define MAILBOX_CPU2BT_BOX1_PARAM3             (MAILBOX_CPU2BT_BASE + 7 * 4)

#define MAILBOX_CPU2BT_READY                   (MAILBOX_CPU2BT_BASE + 8 * 4)

#define MAILBOX_CPU2BT_CLEAR                   (MAILBOX_CPU2BT_BASE + 12 * 4)

//MailBox bt to cpu
#define MAILBOX_BT2CPU_BASE                    (0x060000C0)

#define MAILBOX_BT2CPU_BOX0_CMD                (MAILBOX_BT2CPU_BASE + 0 * 4)
#define MAILBOX_BT2CPU_BOX0_PARAM1             (MAILBOX_BT2CPU_BASE + 1 * 4)
#define MAILBOX_BT2CPU_BOX0_PARAM2             (MAILBOX_BT2CPU_BASE + 2 * 4)
#define MAILBOX_BT2CPU_BOX0_PARAM3             (MAILBOX_BT2CPU_BASE + 3 * 4)

#define MAILBOX_BT2CPU_BOX1_CMD                (MAILBOX_BT2CPU_BASE + 4 * 4)
#define MAILBOX_BT2CPU_BOX1_PARAM1             (MAILBOX_BT2CPU_BASE + 5 * 4)
#define MAILBOX_BT2CPU_BOX1_PARAM2             (MAILBOX_BT2CPU_BASE + 6 * 4)
#define MAILBOX_BT2CPU_BOX1_PARAM3             (MAILBOX_BT2CPU_BASE + 7 * 4)

#define MAILBOX_BT2CPU_READY                   (MAILBOX_BT2CPU_BASE + 8 * 4)

#define MAILBOX_BT2CPU_CLEAR                   (MAILBOX_BT2CPU_BASE + 12 * 4)

//Ready bit for mailbox0 and mailbox1
#define MAILBOX_READY_MASK                     (0x3 << 0)
#define MAILBOX_READY_BOX0                     (0x1 << 0)
#define MAILBOX_READY_BOX1                     (0x1 << 1)

//Clear bit for mailbox0 and mailbox1
#define MAILBOX_CLEAR_MASK                     (0x3 << 0)
#define MAILBOX_CLEAR_BOX0                     (0x1 << 0)
#define MAILBOX_CLEAR_BOX1                     (0x1 << 1)

#define IRQ_MAILBOX_BT                      IRQ_MAILBOX2
#define IRQ_MAILBOX_DSP                     IRQ_MAILBOX1
#define IRQ_MAILBOX_BT_BIT                  IRQ_MAILBOX2_BIT
#define IRQ_MAILBOX_DSP_BIT                 IRQ_MAILBOX1_BIT
#define MAILBOX_CALLBACK_COUNT              4

static void mailbox_bt2cpu_isr(void);
static void mailbox_dsp2cpu_isr(void);
#endif
// eof

