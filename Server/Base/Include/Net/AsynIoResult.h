#ifndef ASYNIORESULT_H
#define ASYNIORESULT_H

#include "AsynIoFrame.h"
#include "NetDefine.h"

struct IAsynIoResult;
struct IAsynIoBuffer;

class AsynIoResult : public IAsynIoResult
{
public:

	void SetAsynIoHandler(IAsynIoHandler* handler) override;

	void Complete() override;

	void PrepareBuffer(unsigned long nLen) override;

	void SetAsynIoBuffer(IAsynIoBuffer* pBufferList, unsigned long nBufferCount) override;

	IAsynIoBuffer* GetAsynIoBufferList() override;

	unsigned long GetAsynIoBufferCount() const override;

	unsigned long GetTransferredBytes() const override;

	void  SetTransferredBytes(unsigned long bytes) override;

	bool IsSuccess() const override;

	unsigned long GetErrorCode() const override;

	void SetErrorCode(unsigned long nCode) override;

	IAsynIoDevice* GetAsynIoDevice() const override;

	void SetAsynIoDevice(IAsynIoDevice* pDevice) override;

	AsynIoType GetAsynIoType() const override;

	void SetAsynIoType(AsynIoType btType) override;

	bool IsSimulation() override;

	void SetSimulation(bool is_simulation) override;

	void Release() override;

protected:
	//////////////////////////////////////////////////////////////////////////
	IAsynIoHandler *   m_pHandler;
	IAsynIoDevice  *   m_pDevice;
	AsynIoBuffer      m_BuffList[MAX_ASYN_IO_BUFF_COUNT];
	DWORD             m_dwBuffCount;
	DWORD             m_dwTransferredBytes;
	DWORD             m_dwErrorCode;
	AsynIoType        m_Type;
	bool              m_bSimulation;
};

#endif //ASYNIORESULT_H
