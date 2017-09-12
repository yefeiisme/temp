#ifndef __I_SIMULATOR_LOGIC_H_
#define __I_SIMULATOR_LOGIC_H_

class ISimulatorLogic
{
public:
	virtual bool		Initialize() = 0;
	virtual bool		SendRequest(const void *pPack, const unsigned int uPackLen) = 0;
	virtual const void	*GetRespond(unsigned int &uPackLen) = 0;
	virtual void		Stop() = 0;
	virtual void		Exit() = 0;
};

extern ISimulatorLogic	&g_ISimulatorLogic;
extern time_t			g_nSimulatorSecond;

#endif
