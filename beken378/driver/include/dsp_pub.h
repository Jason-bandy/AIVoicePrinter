#ifndef _DSP_PUB_H_
#define _DSP_PUB_H_
#define DSP_FAILURE                (1)
#define DSP_SUCCESS                (0)

#define DSP_DEV_NAME                "dsp"

#define DSP_CMD_MAGIC              (0xe560000)
enum
{
    WCMD_DSP_DISABLE = DSP_CMD_MAGIC + 1
};

/*******************************************************************************
* Function Declarations
*******************************************************************************/
extern void dsp_init(void);
extern void dsp_exit(void);
extern uint32_t dsp_is_inited(void);
#endif //_DSP_PUB_H_

